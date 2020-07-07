#include <Exec/BProcess.h>
#include <Exec/ExecBase.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// constructor
BProcess::BProcess(const char *aName, TInt64 aPri, TUint64 aStackSize) : BTask(aName, aPri, aStackSize) {
  //
  mFsReplyPort = CreateMessagePort();
}

// destructor
BProcess::~BProcess() {
  //
  // FileSystemMessage *m;
  // while (m = (FileSystemMessage *)mFsReplyPort->GetMessage()) {
  //   delete m;
  // }
  delete mFsReplyPort;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

TBool BProcess::DoIO(FileDescriptor *aFileDescriptor) {
  // need to forbid/permit to avoid other task modifying message port list while we find()
  Forbid();
  MessagePort *port = gExecBase.FindMessagePort("simple.filesystem");
  Permit();

  if (!port) {
    dlog("no simple.filesystem port\n");
    bochs;
    return EFalse;
  }

  // dlog("DoIO\n");
  // aFileDescriptor->Dump();

  aFileDescriptor->mMessage.mReplyPort = mFsReplyPort;
  aFileDescriptor->mMessage.Send(port);
  WaitPort(mFsReplyPort);
  FileSystemMessage *m;
  while ((m = (FileSystemMessage *)mFsReplyPort->GetMessage())) {
    if (m != &aFileDescriptor->mMessage) {
      delete m;
    }
  }
  aFileDescriptor->mError = aFileDescriptor->mMessage.mError;
  // aFileDescriptor->Dump();
  // dlog("DoIO success\n");
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/
FileDescriptor::FileDescriptor(const char *aName) : BNode(aName) {
  // dlog("Construct FileDescriptor(%s)\n", mNodeName);
}

FileDescriptor::~FileDescriptor() {
  //
}

FileDescriptor *BProcess::OpenDirectory(const char *aFilename) {
  FileDescriptor *fd = new FileDescriptor(aFilename);

  fd->mMessage.Reuse(EFileSystemOpenDirectory);
  fd->mMessage.mBuffer = DuplicateString(fd->mNodeName);

  // dlog("new fd %x (%s)\n", fd, fd->mMessage.mBuffer);
  DoIO(fd);

  delete[](TUint8 *) fd->mMessage.mBuffer;
  fd->mMessage.mBuffer = ENull;

  if (fd->mError != EFileSystemErrorNone) {
    delete fd;
    return ENull;
  }

  return fd;
}

TBool BProcess::ReadDirectory(FileDescriptor *aFileDescriptor) {
  aFileDescriptor->mMessage.Reuse(EFileSystemReadDirectory);
  DoIO(aFileDescriptor);

  return aFileDescriptor->mError == EFileSystemErrorNone;
}

TBool BProcess::CloseDirectory(FileDescriptor *aFileDescriptor) {
  aFileDescriptor->mMessage.Reuse(EFileSystemCloseDirectory);
  aFileDescriptor->mMessage.mBuffer = DuplicateString(mNodeName);
  DoIO(aFileDescriptor);
  delete [] (char *)aFileDescriptor->mMessage.mBuffer;
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

FileDescriptor *BProcess::OpenFile(const char *aFilename) {
  FileDescriptor *fd = new FileDescriptor(aFilename);

  fd->mMessage.Reuse(EFileSystemOpen);
  fd->mMessage.mBuffer = DuplicateString(fd->mNodeName);

  // dlog("new fd %x (%s)\n", fd, fd->mMessage.mBuffer);
  DoIO(fd);

  delete[](TUint8 *) fd->mMessage.mBuffer;
  fd->mMessage.mBuffer = ENull;

  if (fd->mError != EFileSystemErrorNone) {
    delete fd;
    return ENull;
  }

  return fd;
}

TUint64 BProcess::ReadFile(FileDescriptor *aFileDescriptor, TAny *aBuffer, TUint64 aSize) {
  aFileDescriptor->mMessage.Reuse(EFileSystemRead);
  aFileDescriptor->mMessage.mBuffer = aBuffer;
  aFileDescriptor->mMessage.mCount = aSize;

  DoIO(aFileDescriptor);
  if (aFileDescriptor->mError != EFileSystemErrorNone) {
    return 0;
  }
  return aFileDescriptor->mMessage.mCount;
}

TBool BProcess::WriteFile(FileDescriptor *aFileDescriptor, TAny *aBuffer, TUint64 aSize) {
  return EFalse;
}

TBool BProcess::CloseFile(FileDescriptor *aFileDescriptor) {
  aFileDescriptor->mMessage.Reuse(EFileSystemClose);
  aFileDescriptor->mMessage.mBuffer = DuplicateString(mNodeName);
  DoIO(aFileDescriptor);
  delete [] (char *)aFileDescriptor->mMessage.mBuffer;
  return ETrue;
}

TBool BProcess::RemoveFile(FileDescriptor *aFileDescriptor) {
  return EFalse;
}
