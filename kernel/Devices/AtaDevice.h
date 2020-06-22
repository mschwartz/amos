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
  ~AtaMessage() {}

public:
  /**
   * Reuse the message, by setting the argument members.
   *
   * This saves us from having to new/delete over and over.
   */
  void Reuse(TUint64 aLba, TAny *aBuffer, TInt32 aCount) {
    mError = EAtaErrorNone;
    mLba = aLba;
    mBuffer = aBuffer;
    mCount = aCount;
  }

public:
  static AtaMessage *CreateReadMessaage(MessagePort *aReplyPort,
    TUint8 aUnit,
    TUint64 aLba,
    TAny *aBuffer,
    TInt32 aCount) {
    AtaMessage *m = new AtaMessage(aReplyPort, EAtaReadBlocks);
    m->mUnit = aUnit;
    m->mLba = aLba;
    m->mBuffer = aBuffer;
    m->mCount = aCount;
    return m;
  }

public:
  EAtaDeviceCommand mCommand;
  EAtaError mError;
  TUint8 mUnit;
  TUint64 mLba;
  TAny *mBuffer;
  TUint32 mCount;
};

class AtaDevice : public BDevice {
public:
  AtaDevice();
  ~AtaDevice();
};

#endif
