#include <Inspiration/WindowManager/KeyboardTask.hpp>
#include <Inspiration/InspirationBase.hpp>
#include <Exec/ExecBase.hpp>
#include <Devices/KeyboardDevice.hpp>

KeyboardTask::KeyboardTask() : BTask("inspiration-keyboard.task", TASK_PRI_DEFAULT) {
  //
}

TInt64 KeyboardTask::Run() {
  MessagePort *keyboardPort;

  dprint("\n");
  dlog("KeyboardTask Run\n");
  // Forbid();
  while ((keyboardPort = gExecBase.FindMessagePort("keyboard.device")) == ENull) {
    Sleep(1);
  }
  // Permit();

  MessagePort *replyPort = CreatePort("replyPort");
  KeyboardMessage *message = new KeyboardMessage(replyPort, EKeyRead);

  message->Send(keyboardPort);

  for (;;) {
    WaitPort(replyPort);
    while (KeyboardMessage *m = (KeyboardMessage *)replyPort->GetMessage()) {
      if (m == message) {
        if (m->mError != EKeyboardTryAgain && (m->mResult & 0x80) == 0) { // ignore AGAIN and key up
                                                                          // dlog("key %02x %c\n", m->mResult, m->mResult);
          IdcmpMessage im;
          im.mClass = IDCMP_VANILLAKEY;
          im.mCode = m->mResult;
          im.mQualifier = 0;
          im.mAddress = ENull;
          mInspirationBase.SendIdcmpMessage(&im);
        }
      }
      else {
        dlog(" *** SPURIOUS\n");
      }
      message->mReplyPort = replyPort;
      message->Send(keyboardPort);
    }
  }
}
