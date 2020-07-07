#include <Inspiration/idcmp/IdcmpSubscribers.h>
#include <Inspiration/idcmp/IdcmpTask.h>

IdcmpSubscribers::IdcmpSubscribers() : BList("IdcmpSubscribers") {
}

IdcmpSubscriber::IdcmpSubscriber(const char *aName) : BNode(aName) {
}

IdcmpSubscriber *IdcmpSubscribers::Find(IdcmpSubscribeMessage *aMatch) {
  for (IdcmpSubscriber *s = (IdcmpSubscriber *)First(); !End(s); s = (IdcmpSubscriber *)Next(s)) {
    if (s->mWindow == aMatch->mWindow) {
      return s;
    }
  }
  return ENull;
}
