#ifndef INSPIRATION_DIRTYRECT_H
#define INSPIRATION_DIRTYRECT_H

#include <Types/BList.hpp>
#include <Types/TRect.hpp>
#include <Exec/SpinLock.hpp>

class DirtyRect : public BNode {
public:
  DirtyRect(TRect &aRect);

public:
  TBool Overlaps(TRect &aRect) { return mRect.Overlaps(aRect); }
  TInt32 Area() { return mRect.Area(); }

public:
  TRect mRect;
};

class DirtyRectList : public BList {
public:
  DirtyRectList();

public:
  void Add(TRect &aRect);

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;

public:
  DirtyRect *First() { return (DirtyRect *)mNext; }
  DirtyRect *Next(DirtyRect *r) { return (DirtyRect *)r->mNext; }
  TBool End(DirtyRect *r) { return BList::End(r); }
};

#endif
