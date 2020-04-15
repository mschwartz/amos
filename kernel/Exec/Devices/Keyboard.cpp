#include <Devices/Keyboard.h>
#include <Devices/Timer.h>
#include <x86/kprint.h>
#include <Devices/PIC.h>
#include <x86/bochs.h>

#include <Exec/ExecBase.h>


TUint16 KEYB_DR = 0x60; /* data register */
TUint16 KEYB_SR = 0x64; /* status register */
TUint8 PORT1 = 1;
TUint8 PORT2 = 2;

const int TIMEOUT = 10000;

enum {
  SR_OUTPUT_EMPTY = (1 << 0),  // 0 = empty, 1 = full.  Must be set before attempting to read from KEYB_DR
  SR_INPUT_EMPTY = (1 << 1),   // 0 = empty, 1 = full.  Must be clear before writing data to KEYB_DR or KEYB_SR
  SR_SYSTEM = (1 << 2),        // should be cleared by firmware during POST
  SR_COMMAND_DATA = (1 << 3),  // 0 = data written to input buffer is data for PS/2 device, 1 data is for PS/2 controller command
  SR_UNKNOWN1 = (1 << 4),      // unknown
  SR_UNKNOWN2 = (1 << 5),      // unknown
  SR_TIMEOUT_ERROR = (1 << 6), // 0 = no error, 1 = error
  SR_PARITY_ERROR = (1 << 7),  // 0 = no error, 1 = error
};

//
// PS/2 CONTROLLER COMMANDS
//

enum {
  // CMD_READ_BYTE_n = read byte n from internal RAM (controller configuration)
  CMD_READ_BYTE_00 = 0x20,
  CMD_READ_BYTE_01,
  CMD_READ_BYTE_02,
  CMD_READ_BYTE_03,
  CMD_READ_BYTE_04,
  CMD_READ_BYTE_05,
  CMD_READ_BYTE_06,
  CMD_READ_BYTE_07,
  CMD_READ_BYTE_08,
  CMD_READ_BYTE_09,
  CMD_READ_BYTE_0a,
  CMD_READ_BYTE_0b,
  CMD_READ_BYTE_0c,
  CMD_READ_BYTE_0d,
  CMD_READ_BYTE_0e,
  CMD_READ_BYTE_0f,
  CMD_READ_BYTE_10,
  CMD_READ_BYTE_11,
  CMD_READ_BYTE_12,
  CMD_READ_BYTE_13,
  CMD_READ_BYTE_14,
  CMD_READ_BYTE_15,
  CMD_READ_BYTE_16,
  CMD_READ_BYTE_17,
  CMD_READ_BYTE_18,
  CMD_READ_BYTE_19,
  CMD_READ_BYTE_1a,
  CMD_READ_BYTE_1b,
  CMD_READ_BYTE_1c,
  CMD_READ_BYTE_1d,
  CMD_READ_BYTE_1e,
  CMD_READ_BYTE_1f,

  // CMD_WRITE_BYTE_n = write byte n to internal RAM (controller configuration)
  CMD_WRITE_BYTE_00 = 0x60,
  CMD_WRITE_BYTE_01,
  CMD_WRITE_BYTE_02,
  CMD_WRITE_BYTE_03,
  CMD_WRITE_BYTE_04,
  CMD_WRITE_BYTE_05,
  CMD_WRITE_BYTE_06,
  CMD_WRITE_BYTE_07,
  CMD_WRITE_BYTE_08,
  CMD_WRITE_BYTE_09,
  CMD_WRITE_BYTE_0a,
  CMD_WRITE_BYTE_0b,
  CMD_WRITE_BYTE_0c,
  CMD_WRITE_BYTE_0d,
  CMD_WRITE_BYTE_0e,
  CMD_WRITE_BYTE_0f,
  CMD_WRITE_BYTE_10,
  CMD_WRITE_BYTE_11,
  CMD_WRITE_BYTE_12,
  CMD_WRITE_BYTE_13,
  CMD_WRITE_BYTE_14,
  CMD_WRITE_BYTE_15,
  CMD_WRITE_BYTE_16,
  CMD_WRITE_BYTE_17,
  CMD_WRITE_BYTE_18,
  CMD_WRITE_BYTE_19,
  CMD_WRITE_BYTE_1a,
  CMD_WRITE_BYTE_1b,
  CMD_WRITE_BYTE_1c,
  CMD_WRITE_BYTE_1d,
  CMD_WRITE_BYTE_1e,
  CMD_WRITE_BYTE_1f,

  CMD_DISABLE_PORT2 = 0xa7,
  CMD_ENABLE_PORT2 = 0xa8, // enable/disable second PS/2 port only if 2 PS/2 ports supported

  // test second PS/2 port (only if 2 ports supported) 0 = test passed
  // 01 = clock line stuck low, 02 = clock line stuck high, 3 = data line stuck low, 4 = data line stuck high
  CMD_TEST_PORT2 = 0xa9,

  CMD_TEST_CONTROLLER = 0xaa, // test PS/2 controller (0x55 passed, 0xfc failed)

  // test first PS/2 port  0 = test passed
  // 01 = clock line stuck low, 02 = clock line stuck high, 3 = data line stuck low, 4 = data line stuck high
  CMD_TEST_PORT1 = 0xab,

  CMD_DIAGNOSTIC_DUMP = 0xac, // diagnostic dump (read all btes of internal RAM)

  // enable/disable first PS/2 port
  CMD_DISABLE_PORT1 = 0xad,
  CMD_ENABLE_PORT1 = 0xae,

  CMD_READ_CONTROLLER_INPUT_PORT = 0xc0, // unkonwn - none of these bits have a standard/predefined purpose
  CMD_COPY_LOWER_BITS,                   // copy bits 0 to 3 of input port to status bits 4 to 7
  CMD_COPY_UPPER_BITS,                   // copy bits 4 to 7 of input port to status bits 4-7

  CMD_READ_CONTROLLER_OUTPUT_PORT = 0xd0,       // see below
  CMD_WRITE_NEXT_CONTROLLER_OUTPUT_BYTE = 0xd1, // write next byte to controller output port - check if output buffer is empty first!
  CMD_WRITE_NEXT_BYTE_FIRST_OUTPUT_BUFFER = 0xd2,
  CMD_WRITE_NEXT_BYTE_SECOND_OUTPUT_BUFFER = 0xd3,
  CMD_WRITE_NEXT_BYTE_SECOND_INPUT_BUFFER = 0xd4,

  //
  // 0xf0 to 0xff pulise line low for 6ms.
  // Bits 0-3 are used as a mask: 0: pulse line, 1 don't pulse line, and correspond to 4 different output lines
  PULSE_OUTPUT_LINE_LOW = 0xf0,
  PULSE_RESET_LINE = 0xfe,
  PULSE_RESET_PORT,
};

// bits for configuraiton bit (commands 0x20 and 0x60)
enum {
  CONFIG_FIRST_PORT_ENABLE = (1 << 0),
  CONFIG_SECOND_PORT_ENABLE = (1 << 1),
  CONFIG_SYSTEM_FLAG = (1 << 2),
  CONFIG_ZERO1 = (1 << 3),
  CONFIG_FIRST_PORT_CLOCK = (1 << 4),
  CONFIG_SECOND_PORT_CLOCK = (1 << 5),
  CONFIG_FIRST_PORT_TRANSLATION = (1 << 6),
  CONFIG_ZERO2 = (1 << 7),
};

// bits for controller output port (0xd0 and 0xd1)

enum {
  OUTPUT_SYSTEM_RESET = (1 << 0), // ALWAYS SET TO 1.  Pulse reset line using command 0xfe instead.
  OUTPUT_A20_GATE = (1 << 1),
  OUTPUT_SECOND_PORT_CLODK = (1 << 2),
  OUTPUT_SECOND_PORT_DATA = (1 << 3),
  OUTPUT_PORT1_BUFFER_FULL = (1 << 4), // connected to IRQ1
  OUTPUT_PORT2_BUFFER_FULL = (1 << 5), // conntected to IRQ12
  OUTPUT_FIRST_PORT_CLOCK = (1 << 6),
  OUTPUT_FIRST_PORT_DATA = (1 << 7),
};

Keyboard *gKeyboard;

static inline TUint8 read_config_byte(TUint8 offset) {
  outb(CMD_READ_BYTE_00 + offset, KEYB_SR);
  return inb(KEYB_DR);
}

static inline TUint16 read_status() {
  return inb(KEYB_SR);
}

static inline void write_status(TUint8 value) {
  outb(value, KEYB_SR);
}

// wait for bit(s) to be set in status register
static inline bool wait_status(TUint8 bits) {
  int timeout = TIMEOUT;
  while (timeout-- > 0 && (read_status() & bits))
    ;
  return timeout > 0;
}

static inline bool write_data(TUint8 value, TUint16 port = 1) {
  if (port == PORT2) {
    write_status(CMD_WRITE_NEXT_BYTE_SECOND_INPUT_BUFFER);
  }

  if (!wait_status(SR_INPUT_EMPTY)) {
    return false;
  }

  outb(value, KEYB_DR);
  return true;
}

static inline TUint16 read_data() {
  if (!wait_status(SR_OUTPUT_EMPTY)) {
    return 0xffff;
  }
  return inb(KEYB_DR);
}

static inline TUint16 command(TUint8 cmd, TUint8 port = 1) {
  write_status(cmd);
  return read_data();
}

bool keyboard_isr(void *aData) {
  TUint16 *ptr = (TUint16 *)0xb8000;
  TUint16 t = inb(KEYB_DR);
  TUint64 c = gTimer->GetTicks();
  dprint(" keyboard data: %x %d\n", t, c);
  *ptr = 0x1f42;
  gPIC->ack(IRQ_KEYBOARD);
  return true;
}

Keyboard::Keyboard() {
  dprint("Construct Keyboard\n");

#if 0
  dprint("configure PS/2\n");

  // configure PS/2
  // 1) disable devices
  dprint("  disable port 1\n");
  write_status(CMD_DISABLE_PORT1);
  dprint("  disable port 2\n");
  write_status(CMD_DISABLE_PORT2);

  // 2 flush output buffer
  dprint("read data %x\n", read_data());

  // 3 set controller configuration byte
  TUint8 config = read_config_byte(0);
  dprint("  config = %x... ", config);
  config &= ~CONFIG_FIRST_PORT_ENABLE | CONFIG_SECOND_PORT_ENABLE | CONFIG_FIRST_PORT_TRANSLATION;
  dprint("=> %x...\n", config);
  write_status(CMD_WRITE_BYTE_00);
  write_data(config);

  // 4 perform self test
  TUint16 st = command(CMD_TEST_CONTROLLER);
  dprint("  controller test: %x\n", st);

  // 5 determine if there are 2 channels
  if (config & CONFIG_SECOND_PORT_CLOCK) {
    dprint("  enable port2\n");
    write_status(CMD_ENABLE_PORT2);
    // disable it again
    dprint("  disable port2\n");
    write_status(CMD_DISABLE_PORT2);
  }
  // 5 perform interface tests
  dprint("  test port 1\n");
  TUint16 it = command(CMD_TEST_PORT1);
  dprint("  port 1 interface test: %x\n", it);
  if (config & CONFIG_SECOND_PORT_CLOCK) {
    dprint("  test port 2\n");
    it = command(CMD_TEST_PORT2);
    dprint("  port 2 interface test: %x\n", it);
  }
  // 6 enable devices
  dprint("  enable port 1\n");
  write_status(CMD_ENABLE_PORT1);
  if (config & CONFIG_SECOND_PORT_CLOCK) {
    dprint("  enable port 2\n");
    write_status(CMD_ENABLE_PORT2);
  }
  // 7 reset devices
  TUint16 reset1 = command(PULSE_RESET_PORT, PORT1);
  dprint("  reset port 1 = %x\n", reset1);
  if (config & CONFIG_SECOND_PORT_CLOCK) {
    TUint16 reset2 = command(PULSE_RESET_PORT, PORT2);
    dprint("  reset port 2 = %x\n", reset2);
  }
#endif

  //
  ptr1 = ptr2 = 0;
  // install kernel handler
  gExecBase.AddInterruptHandler(IRQ_KEYBOARD, keyboard_isr, this, "");
  // enable the keyboard interrupt
  gPIC->enable_interrupt(IRQ_KEYBOARD);
}

Keyboard::~Keyboard() {
}
