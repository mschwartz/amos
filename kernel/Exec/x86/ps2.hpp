#ifndef EXEC_X86_PS2_H
#define EXEC_X86_PS2_H

#define ENABLE_PS2
#undef ENABLE_PS2

// Ports
#define PS2_CMD 0x64
#define PS2_DATA 0x60

// Configuration byte
#define PS2_CFG_FIRST_PORT 1
#define PS2_CFG_SECOND_PORT (1 << 1)
#define PS2_CFG_SYSTEM_FLAG (1 << 2)
#define PS2_CFG_FIRST_CLOCK (1 << 4)
#define PS2_CFG_SECOND_CLOCK (1 << 5)
#define PS2_CFG_TRANSLATION (1 << 6)
#define PS2_CFG_MUST_BE_ZERO (1 << 7)

// Controller commands
#define PS2_DISABLE_FIRST 0xAD
#define PS2_DISABLE_SECOND 0xA7
#define PS2_ENABLE_FIRST 0xAE
#define PS2_ENABLE_SECOND 0xA8
#define PS2_READ_CONFIG 0x20
#define PS2_WRITE_CONFIG 0x60
#define PS2_SELF_TEST 0xAA
#define PS2_TEST_FIRST 0xAB
#define PS2_TEST_SECOND 0xA9
#define PS2_WRITE_SECOND 0xD4

// Controller responses
#define PS2_SELF_TEST_OK 0x55
#define PS2_TEST_OK 0x00

// Device commands
#define PS2_DEV_RESET 0xFF
#define PS2_DEV_IDENTIFY 0xF2
#define PS2_DEV_ENABLE_SCAN 0xF4
#define PS2_DEV_DISABLE_SCAN 0xF5

// Device responses
#define PS2_DEV_ACK 0xFA
#define PS2_DEV_RESET_ACK 0xAA

// Number of iteration in our PS/2 IO spin loops
#define PS2_TIMEOUT 500

enum {
  PS2_MOUSE = 0x00,
  PS2_MOUSE_SCROLL_WHEEL = 0x03,
  PS2_MOUSE_FIVE_BUTTONS = 0x04,
  PS2_KEYBOARD,
  PS2_KEYBOARD_TRANSLATED,
  PS2_DEVICE_UNKNOWN
};

#if 0
typedef struct {
  // True if this device exist
  bool present;
  // The controller in which this device is plugged in
  TUint32 num;
  TUint32 type;
  // Raw identifiers
  TUint8 first_id_byte;
  TUint8 second_id_byte; // Validity depends on device
} TPs2Device;
#endif

#ifdef ENABLE_PS2
class PS2 {
public:
  PS2();

public:
  TBool HasKeyboard() { return mKeyboard; }
  TBool HasMouse() { return mMouse; }

public:
  static TUint8 Read(TUint32 aPort);
  static TBool Write(TUint32 aPort, TUint8 aData);

  static TUint32 IdentifyDevice(TUint32 num);
  static TBool WriteDevice(TUint32 device, TUint8 b);
  static TBool ExpectAck();

protected:
  TBool mKeyboard, mMouse;
};

#endif

#endif
