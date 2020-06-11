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
    return EFalse;
  }

  // dlog("DoIO\n");
  // aFileDescriptor->Dump();

  aFileDescriptor->mMessage.mReplyPort = mFsReplyPort;
  aFileDescriptor->mMessage.SendMessage(port);
  WaitPort(mFsReplyPort);
  FileSystemMessage *m;
  while (m = (FileSystemMessage *)mFsReplyPort->GetMessage()) {
    if (m != &aFileDescriptor->mMessage) {
      delete m;
    }
  }
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/
FileDescriptor::FileDescriptor(const char *aName) : BNode(aName) {
  dlog("Construct FileDescriptor(%s)\n", mNodeName);
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
  return fd;
}

TBool BProcess::ReadDirectory(FileDescriptor *aFileDescriptor) {
  aFileDescriptor->mMessage.Reuse(EFileSystemReadDirectory);
  aFileDescriptor->mMessage.mBuffer = DuplicateString(mNodeName);
  DoIO(aFileDescriptor);
  return ETrue;
}

TBool BProcess::CloseDirectory(FileDescriptor *aFileDescriptor) {
  aFileDescriptor->mMessage.Reuse(EFileSystemCloseDirectory);
  aFileDescriptor->mMessage.mBuffer = DuplicateString(mNodeName);
  DoIO(aFileDescriptor);
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

FileDescriptor *BProcess::OpenFile(const char *aFilename) {
  return ENull;
}

TBool BProcess::ReadFile(FileDescriptor *aFileDescriptor, TAny *aBuffer, TUint64 aSize) {
  return EFalse;
}

TBool BProcess::WriteFile(FileDescriptor *aFileDescriptor, TAny *aBuffer, TUint64 aSize) {
  return EFalse;
}

TBool BProcess::CloseFile(FileDescriptor *aFileDescriptor) {
  return EFalse;
}

TBool BProcess::RemoveFile(FileDescriptor *aFileDescriptor) {
  return EFalse;
}
