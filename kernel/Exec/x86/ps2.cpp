#include <Exec/Types.h>
#include <Exec/x86/ps2.h>

#ifdef ENABLE_PS2
static TBool ps2_can_read() {
  return inb(PS2_CMD) & 1;
}

/* Loops until data is available in the controller's output buffer or until our
    * timer has expired.
     * Call this function before reading from the controller's data port, PS2_DATA.
      */
static TBool ps2_wait_read() {
  int timer = PS2_TIMEOUT;

  while (!(inb(0x64) & 1) && timer-- >= 0) {
    asm("pause");
  }

  return timer != 0;
}

/* Loops until the controller's input buffer is empty or our timer has expired.
    * Call this function before writing to the controller's command port, PS2_CMD.
     */
static TBool ps2_wait_write() {
  int timer = PS2_TIMEOUT;

  while ((inb(0x64) & 2) && timer-- > 0) {
    asm("pause");
  }

  return timer != 0;
}

/* Converts the (one or two) bytes sent by a PS/2 device in response to an
    * `identity` request to a code representing the device type.
     */
static TUint32 ps2_identity_bytes_to_type(TUint8 first, TUint8 second) {
  if (first == 0x00 || first == 0x03 || first == 0x04) {
    return first; // PS2_MOUSE* match the first byte
  }
  else if (first == 0xAB) {
    if (second == 0x41 || second == 0xC1) {
      return PS2_KEYBOARD_TRANSLATED;
    }
    else if (second == 0x83) {
      return PS2_KEYBOARD;
    }
  }

  return PS2_DEVICE_UNKNOWN;
}

/* Performs discovery and initialisation of PS/2 controllers and devices.
    * A lot of redundancy here because of poorly-indexed identifiers.
     */
PS2::PS2() {
  dprint("[PS2] Initializing PS/2 devices\n");

  // Describes the available PS/2 controllers
  static TBool controllers[] = { ETrue, ETrue };

  mKeyboard = mMouse = EFalse;

  TBool dual_channel = ETrue;

  //  cli();

  // Disable both PS/2 device ports
  // Even if only one is present, disabling the second is harmless
  PS2::Write(PS2_CMD, PS2_DISABLE_FIRST);
  PS2::Write(PS2_CMD, PS2_DISABLE_SECOND);

  // Flush output bufffer: if the controller had anything to say, ignore it
  inb(PS2_DATA);

  // Get the controller configuration byte
  PS2::Write(PS2_CMD, PS2_READ_CONFIG);
  TUint8 config = PS2::Read(PS2_DATA);

  // Check the basics
  config |= PS2_CFG_SYSTEM_FLAG;

  if (config & PS2_CFG_MUST_BE_ZERO) {
    dprint("[PS2] Invalid bit set in configuration byte\n");
  }

  // Disable interrupts and scan code translation
  config &= ~(PS2_CFG_FIRST_PORT | PS2_CFG_SECOND_PORT | PS2_CFG_TRANSLATION);

  // Update configuration byte
  PS2::Write(PS2_CMD, PS2_WRITE_CONFIG);
  PS2::Write(PS2_DATA, config);

  // Controller self-test
  PS2::Write(PS2_CMD, PS2_SELF_TEST);

  if (PS2::Read(PS2_DATA) != PS2_SELF_TEST_OK) {
    dprint("[PS2] Controller failed self-test\n");

    controllers[0] = EFalse;
    controllers[1] = EFalse;

    return;
  }

  // The last write may have reset our controller:
  // better reset our configuration byte just in case
  PS2::Write(PS2_CMD, PS2_WRITE_CONFIG);
  PS2::Write(PS2_DATA, config);

  // Check if we _really_ have two channels
  PS2::Write(PS2_CMD, PS2_ENABLE_SECOND);
  PS2::Write(PS2_CMD, PS2_READ_CONFIG);
  config = PS2::Read(PS2_DATA);

  if (config & PS2_CFG_SECOND_CLOCK) {
    dprint("[PS2] Only one PS/2 controller\n");
    dual_channel = EFalse;
    controllers[1] = EFalse;
  }
  else {
    PS2::Write(PS2_CMD, PS2_DISABLE_SECOND); // Re-disable the second controller
  }

  // Test the controllers
  PS2::Write(PS2_CMD, PS2_TEST_FIRST);

  if (PS2::Read(PS2_DATA) != PS2_TEST_OK) {
    dprint("[PS2] First PS/2 port failed testing\n");
    controllers[0] = EFalse;
  }

  if (dual_channel) {
    PS2::Write(PS2_CMD, PS2_TEST_SECOND);

    if (PS2::Read(PS2_DATA) != PS2_TEST_OK) {
      dprint("[PS2] Second PS/2 port failed testing\n");
      controllers[1] = EFalse;
    }
  }

  // Enable available ports
  if (controllers[0]) {
    PS2::Write(PS2_CMD, PS2_ENABLE_FIRST);
    config |= PS2_CFG_FIRST_PORT;
    config &= ~PS2_CFG_FIRST_CLOCK;
  }

  if (controllers[1]) {
    PS2::Write(PS2_CMD, PS2_ENABLE_SECOND);
    config |= PS2_CFG_SECOND_PORT;
    config &= ~PS2_CFG_SECOND_CLOCK;
  }

  // Enable interrupts from detected controllers
  PS2::Write(PS2_CMD, PS2_WRITE_CONFIG);
  PS2::Write(PS2_DATA, config);

  // Reset devices
  for (TUint32 i = 0; i < 2; i++) {
    if (!controllers[i]) {
      continue;
    }

    PS2::WriteDevice(i, PS2_DEV_RESET);
    TUint8 ret = PS2::Read(PS2_DATA);

    // If it fails, disable the device's port
    if (ret != PS2_DEV_ACK || PS2::Read(PS2_DATA) != PS2_DEV_RESET_ACK) {
      dprint("[PS2] Failed to reset device %d\n", i);

      controllers[i] = EFalse;
      config &= ~(i == 0 ? PS2_CFG_FIRST_PORT : PS2_CFG_SECOND_PORT);

      PS2::Write(PS2_CMD, PS2_WRITE_CONFIG);
      PS2::Write(PS2_DATA, config);
    }

    // For some reason, mice send an additional 0x00 byte
    if (ps2_can_read()) {
      PS2::Read(0x60);
    }
  }

  for (TUint32 i = 0; i < 2; i++) {
    if (controllers[i]) {
      TUint32 type = PS2::IdentifyDevice(i);

      switch (type) {
        case PS2_MOUSE:
        case PS2_MOUSE_SCROLL_WHEEL:
        case PS2_MOUSE_FIVE_BUTTONS:
          dprint("[PS2] Mouse\n");
          mMouse = ETrue;
          break;
        case PS2_KEYBOARD:
        case PS2_KEYBOARD_TRANSLATED:
          dprint("[PS2] Keyboard\n");
          mKeyboard = ETrue;
          PS2::WriteDevice(i, PS2_DEV_ENABLE_SCAN);
          PS2::ExpectAck();
          break;
      }
    }
  }

  sti();
}

/* Tries to write a byte on the specified port, and returns whether the
    * operation was successful.
     */
TBool PS2::Write(TUint32 port, TUint8 b) {
  if (ps2_wait_write()) {
    outb(port, b);
    return ETrue;
  }

  dprint("[PS2] Write failed\n");

  return EFalse;
}

/* Returns the first byte of data available from `port`.
    * Returns `(TUint8) -1` on read error, and also when this value was
     * legitimately read. TODO: room for improvement.
      */
TUint8 PS2::Read(TUint32 port) {
  if (ps2_wait_read()) {
    return inb(port);
  }

  dprint("[PS2] Read failed\n");

  return -1;
}

/* Write a byte to the specified `device` input buffer.
    * This function is used to send command to devices.
     */
TBool PS2::WriteDevice(TUint32 device, TUint8 b) {
  if (device != 0) {
    if (!PS2::Write(PS2_CMD, PS2_WRITE_SECOND)) {
      return EFalse;
    }
  }

  return PS2::Write(PS2_DATA, b);
}

/* Returns ETrue if a device replied with `PS2_DEV_ACK`.
    * This is usually in reply to a command sent to that device.
     */
TBool PS2::ExpectAck() {
  TUint8 ret = PS2::Read(PS2_DATA);

  if (ret != PS2_DEV_ACK) {
    dprint("[PS2] Device failed to acknowledge command\n");
    return EFalse;
  }

  return ETrue;
}

/* Asks the device to identify itself, returns an enum value.
    * Useful as some devices's identities are several bytes long.
     */
TUint32 PS2::IdentifyDevice(TUint32 num) {
  PS2::WriteDevice(num, PS2_DEV_DISABLE_SCAN); // Disables scanning
  PS2::ExpectAck();
  PS2::WriteDevice(num, PS2_DEV_IDENTIFY); // Identify
  PS2::ExpectAck();

  TUint32 first_id_byte = PS2::Read(PS2_DATA);
  TUint32 second_id_byte = PS2::Read(PS2_DATA);

  return ps2_identity_bytes_to_type(first_id_byte, second_id_byte);
}
#endif

