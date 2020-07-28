#include <Exec/ExecBase.hpp>
#include <Exec/x86/cpu_utils.hpp>
#include <Exec/x86/ps2.hpp>
#include <Devices/KeyboardDevice.hpp>
#include <Exec/BTask.hpp>

const TUint16 KEYB_DR = 0x60; /* data register */
const TUint16 KEYB_SR = 0x64; /* status register */
const TUint8 PORT1 = 1;
const TUint8 PORT2 = 2;

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

const char kbdus[128] = {
  0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
  '9', '0', '-', '=', '\b',                         /* Backspace */
  '\t',                                             /* Tab */
  'q', 'w', 'e', 'r',                               /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     /* Enter key */
  0,                                                /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
  '\'', '`', 0,                                     /* Left shift */
  '\\', 'z', 'x', 'c', 'v', 'b', 'n',               /* 49 */
  'm', ',', '.', '/', 0,                            /* Right shift */
  '*',
  0,   /* Alt */
  ' ', /* Space bar */
  0,   /* Caps lock */
  0,   /* 59 - F1 key ... > */
  0, 0, 0, 0, 0, 0, 0, 0,
  0, /* < ... F10 */
  0, /* 69 - Num lock*/
  0, /* Scroll Lock */
  0, /* Home key */
  0, /* Up Arrow */
  0, /* Page Up */
  '-',
  0, /* Left Arrow */
  0,
  0, /* Right Arrow */
  '+',
  0, /* 79 - End key*/
  0, /* Down Arrow */
  0, /* Page Down */
  0, /* Insert Key */
  0, /* Delete Key */
  0, 0, 0,
  0, /* F11 Key */
  0, /* F12 Key */
  0, /* All other keys are undefined */
};

#define TOUPPER(x) (('a' <= (x) && (x) <= 'z') ? ((x - 'a') + 'A') : (x))

//Keyboard *gKeyboard;

static inline TUint8 read_config_byte(TUint8 offset) {
  outb(KEYB_SR, CMD_READ_BYTE_00 + offset);
  return inb(KEYB_DR);
}

static inline TUint16 read_status() {
  return inb(KEYB_SR);
}

static inline void write_status(TUint8 value) {
  outb(KEYB_SR, value);
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

  outb(KEYB_DR, value);
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

class KeyboardDeviceTask;

class KeyboardInterrupt : public BInterrupt {
public:
  KeyboardInterrupt(KeyboardDeviceTask *aTask) : BInterrupt("keyboard.device", LIST_PRI_MAX) {
    mTask = aTask;
  }

public:
  TBool Run(TAny *aData);
  //    TUint16 *ptr = (TUint16 *)0xb8000;

protected:
  KeyboardDeviceTask *mTask;
};

class KeyboardDeviceTask : public BTask {
  friend KeyboardInterrupt;

public:
  KeyboardDeviceTask() : BTask("keyboard.device", LIST_PRI_MAX) {
    mHead = mTail = 0;
    // install kernel handler
    gExecBase.SetIntVector(EKeyboardIRQ, new KeyboardInterrupt(this));
    // enable the keyboard interrupt
    gExecBase.EnableIRQ(IRQ_KEYBOARD);
  }

  TInt64 Run();

public:
  MessagePort *mMessagePort;

protected:
  char mBuffer[KEYBOARD_BUFFER_SIZE];
  int mHead, mTail;
};

TBool KeyboardInterrupt::Run(TAny *aData) {
  static TBool shift_key = EFalse;

  TInt timeout;
  for (timeout = 1000; timeout > 0; timeout--) {
    TUint8 t = inb(KEYB_DR);
    if (t == 0) {
      continue;
    }
    // dlog("   keycode: %02x\n", t);
    if (t & 0x80) {
      t &= 0x7f;
      if (t == 0x2a) {
        shift_key = EFalse;
      }
      else {
        TInt res = kbdus[t];
        if (res) {
          if (shift_key) {
            res = TOUPPER(res);
          }
          KeyboardMessage *m = new KeyboardMessage(ENull, EKeyUp);
          m->mResult = res;
          m->Send(mTask->mMessagePort);
        }
      }
    }
    else {
      // send message to task
      if (t == 0x2a) {
        shift_key = ETrue;
      }
      else {
        TInt res = kbdus[t];
        if (res) {
          if (shift_key) {
            res = TOUPPER(res);
          }
          KeyboardMessage *m = new KeyboardMessage(ENull, EKeyDown);
          m->mResult = res;
          m->Send(mTask->mMessagePort);
        }
      }
      break;
    }
    if (timeout > 0) {
      gExecBase.AckIRQ(IRQ_KEYBOARD);
    }
    return ETrue;
  }

  gExecBase.AckIRQ(IRQ_KEYBOARD);
  return ETrue;
}

TInt64 KeyboardDeviceTask::Run() {
  dprint("\n");
  dlog("keyboard task alive!\n");

  // initialize message port and wait for messages
  mMessagePort = CreatePort("keyboard.device");
  gExecBase.AddMessagePort(*mMessagePort);

  for (;;) {
    while (WaitPort(mMessagePort)) {
      while (KeyboardMessage *m = (KeyboardMessage *)mMessagePort->GetMessage()) {
        switch (m->mCommand) {
          case EKeyUp:
            // dlog("Queue key UP %d, %x, %c\n", m->mResult, m->mResult, m->mResult);
            if (((mTail + 1) % KEYBOARD_BUFFER_SIZE) != mHead) {
              mBuffer[mTail++] = m->mResult | 0x80;
              mTail %= KEYBOARD_BUFFER_SIZE;
            }
            else {
              dlog("*** keyboard.device keyboard queue full\n");
            }
            // We don't want to waste time deleting the message in the Interrupt handler, so we do it here.
            // This is not normal convention!
            delete m;
            break;

          case EKeyDown:
            // dlog("Queue key DOWN %d, %x, %c\n", m->mResult, m->mResult, m->mResult);
            if (((mTail + 1) % KEYBOARD_BUFFER_SIZE) != mHead) {
              mBuffer[mTail++] = m->mResult;
              mTail %= KEYBOARD_BUFFER_SIZE;
            }
            else {
              dlog("*** keyboard.device keyboard queue full\n");
            }
            // We don't want to waste time deleting the message in the Interrupt handler, so we do it here.
            // This is not normal convention!
            delete m;
            break;

          // respond to message if keyboard message FIFO is not empty
          case EKeyRead:
            // TODO: this looks expensive (lots of try again messages when no activity)
            if (mHead == mTail) {
              // queue is empty
              m->mError = EKeyboardTryAgain;
            }
            else {
              m->mError = EKeyboardErrorNone;
              m->mResult = mBuffer[mHead++];
              mHead %= KEYBOARD_BUFFER_SIZE;
            }
            m->Reply();
            break;

          default:
            dlog("keyboard.device: Unknown mCommand(%d/%x)\n", m->mCommand, m->mCommand);
            break;
        }
      }
    }
  }
}

KeyboardDevice::KeyboardDevice() : BDevice("keyboard.device") {
  dprint("\n");
  dlog("Construct KeyboardDevice\n");
  gExecBase.AddTask(new KeyboardDeviceTask);
}

KeyboardDevice::~KeyboardDevice() {
  //
}
