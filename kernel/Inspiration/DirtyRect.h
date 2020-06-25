#ifndef INSPIRATION_DIRTYRECT_H
#define INSPIRATION_DIRTYRECT_H

#include <Exec/Types/BList.h>
#include <Graphics/TRect.h>

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
};

#endif
