#ifndef AMOS_ATADEVICE_H
#define AMOS_ATADEVICE_H

#include <Exec/BDevice.h>

enum EAtaError {
  EAtaErrorNone,
  EAtaErrorFailed,
};
enum EAtaDeviceCommand {
  EAtaReady,
  EAtaReadBlocks,
};

class AtaMessage : public BMessage {
public:
  AtaMessage(MessagePort *aReplyPort, EAtaDeviceCommand aCommand)
      : BMessage(aReplyPort), mCommand(aCommand) {
    mError = EAtaErrorNone;
  }
  ~AtaMessage();

public:
  static AtaMessage *CreateReadMessaage(MessagePort *aReplyPort,
    TUint8 aUnit,
    TUint64 aLBA,
    TAny *aBuffer,
    TInt32 aCount) {
    AtaMessage *m = new AtaMessage(aReplyPort, EAtaReadBlocks);
    m->mUnit = aUnit;
    m->mLBA = aLBA;
    m->mBuffer = aBuffer;
    m->mCount = aCount;
    return m;
  }

public:
  EAtaDeviceCommand mCommand;
  EAtaError mError;
  TUint8 mUnit;
  TUint64 mLBA;
  TAny *mBuffer;
  TUint32 mCount;
};

class AtaDevice : public BDevice {
public:
  AtaDevice();
  ~AtaDevice();
};

#endif
