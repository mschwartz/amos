#include <Inspiration/DirtyRect.hpp>

DirtyRect::DirtyRect(TRect &aRect) : BNode("dirty rect") {
  mRect.Set(aRect);
}

DirtyRectList::DirtyRectList() : BList("dirty Rect list") {
  //
}

void DirtyRectList::Add(TRect &aRect) {
  TInt32 area = aRect.Area();

  for (DirtyRect *d = (DirtyRect *)First(); !End(d); d = (DirtyRect *)Next(d)) {
    if (d->Overlaps(aRect)) {
      TRect newRect(d->mRect);
      newRect.Union(aRect);
      if (newRect.Area() <= d->Area() + area) {
        // combine
        d->mRect.Set(newRect);
        return;
      }
    }
  }
  // no overlap, add new DirtyRect
  DirtyRect *d = new DirtyRect(aRect);
  AddTail(*d);
}
