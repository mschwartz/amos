#ifndef KERNEL_INSPIRATION_IDCMPTASK_H
#define KERNEL_INSPIRATION_IDCMPTASK_H

/**
 * IdcmpTask
 *
 * This task receives system I/O events and sends IDCMP messages to MessagePorts for BWindows that have 
 * subscribed to these messages.
 *
 * In the TNewWindow struct, the mIdcmpFlags configuration value is sent to IdcmpTask.  IdcmpTask keeps track
 * of which (Reply) ports to send the subscribed message(s) to.
 */

#include <Exec/BTask.h>
#include <Exec/MessagePort.h>
#include <Inspiration/BWindow.h>

class IdcmpSubscribers;
class BWindow;
class MouseMessage;
class KeyboardMessage;

struct IdcmpSubscribeMessage : public BMessage {
  EIdcmpCommand mCommand; // if ETrue, subscribe, otherwise unsubscribe
  TUint64 mIdcmpFlags;
  BWindow *mWindow;
  MessagePort *mIdcmpPort;
};

class IdcmpTask : public BTask {
public:
  IdcmpTask();
  ~IdcmpTask();

protected:
  IdcmpMessage *AllocMessage();
  void SendIdcmp(IdcmpMessage *aMessage);

  void HandleSubscribe();
  void HandleKeyboard();
  void HandleMouse();
  void HandleReply();

public:
  void Run();

protected:
  IdcmpSubscribers *mSubscribers; // windows/ports expecting to receive IdcmpMessage stream
  MessagePort *mCommandPort,      // subscribe/unsubscribe/etc.
    *mReplyPort,                  // replies to IdcmpMessages we send
    *mMousePort,                  // mouse.device
    *mMouseReplyPort,             // mouse.device replies
    *mKeyboardPort,               // keyboard.device
    *mKeyboardReplyPort;          // keyboard.device replies

  MouseMessage *mMouseMessage;
  KeyboardMessage *mKeyboardMessage;

  BList mFreeMessages;
};

#endif
