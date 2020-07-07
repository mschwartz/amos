#ifndef KERNEL_INSPIRATION_IDCMPSUBSCRIBERS_H
#define KERNEL_INSPIRATION_IDCMPSUBSCRIBERS_H

#include <Types/BList.h>
#include <Inspiration/idcmp/IdcmpTask.h>

class MessagePort;
class BWindow;

struct IdcmpSubscriber : public BNode {
  IdcmpSubscriber(const char *aName = "no name");

  MessagePort *mPort;
  BWindow *mWindow;
  TUint64 mIdcmpFlags;
};

class IdcmpSubscribers : public BList {
public:
  IdcmpSubscribers();

public:
  IdcmpSubscriber *Find(IdcmpSubscribeMessage *aMatch);
  IdcmpSubscriber *First() { return (IdcmpSubscriber *)First(); }
  IdcmpSubscriber *Next(IdcmpSubscriber *s) { return (IdcmpSubscriber *)Next(s); }
};

#endif
