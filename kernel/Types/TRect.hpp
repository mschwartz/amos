#ifndef GRAPHICS_TRECT_H
#define GRAPHICS_TRECT_H

#include <Types.hpp>

typedef TInt32 TCoordinate;

struct TPoint {
  TCoordinate x, y;

  TPoint() {
    x = 0.0;
    y = 0.0;
  }

  TPoint(TCoordinate aX, TCoordinate aY) {
    x = aX;
    y = aY;
  }

  TPoint(const TPoint &aOther) {
    x = aOther.x;
    y = aOther.y;
  }

  void Set(TCoordinate aX, TCoordinate aY) {
    x = aX;
    y = aY;
  }

  void Set(const TPoint &aOther) {
    x = aOther.x;
    y = aOther.y;
  }

  void Offset(TCoordinate aX, TCoordinate aY) {
    x += aX;
    y += aY;
  }

  void Offset(const TPoint &aOther) {
    x += aOther.x;
    y += aOther.y;
  }
} PACKED;

struct TRect {
  TCoordinate x1, y1, x2, y2;

  // Constructors
public:
  TRect() {
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
  }

  TRect(TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2) {
    this->x1 = aX1;
    this->y1 = aY1;
    this->x2 = aX2;
    this->y2 = aY2;
  }

  TRect(const TPoint &aUpperLeft, const TPoint &aLowerRight) {
    this->x1 = aUpperLeft.x;
    this->y1 = aUpperLeft.y;
    this->x2 = aLowerRight.x;
    this->y2 = aLowerRight.y;
  }

  TRect(const TRect &aOther) {
    this->x1 = aOther.x1;
    this->y1 = aOther.y1;
    this->x2 = aOther.x2;
    this->y2 = aOther.y2;
  }

  void Set(TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2) {
    this->x1 = aX1;
    this->y1 = aY1;
    this->x2 = aX2;
    this->y2 = aY2;
  }

  void Set(const TPoint &aUpperLeft, const TPoint &aLowerRight) {
    this->x1 = aUpperLeft.x;
    this->y1 = aUpperLeft.y;
    this->x2 = aLowerRight.x;
    this->y2 = aLowerRight.y;
  }

  void Set(const TRect &aOther) {
    this->x1 = aOther.x1;
    this->y1 = aOther.y1;
    this->x2 = aOther.x2;
    this->y2 = aOther.y2;
  }

public:
  TCoordinate Width() { return (x2 - x1) + 1; }

  TCoordinate Height() { return (y2 - y1) + 1; }

  void Width(TCoordinate aWidth) { x2 = x1 + aWidth - 1; }

  void Height(TCoordinate aHeight) { y2 = y1 + aHeight - 1; }

  TCoordinate Area() { return Width() * Height(); }

  void Dump(const char *aArgs = ENull) {
#ifdef KERNEL
    if (aArgs) {
      dlog("%s TRect x1:%d,y1:%d x2:%d,y2:%d\n", aArgs, x1, y1, x2, y2);
    } else {
      dlog("TRect x1:%d,y1:%d x2:%d,y2:%d\n", x1, y1, x2, y2);
    }
#endif
  }

public:
  // offset (move) by detla in resources and y
  void Offset(TInt aDX, TInt aDY) {
    x1 += aDX;
    y1 += aDY;
    x2 += aDX;
    y2 += aDY;
  }

public:
//  TBool Overlaps(TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2);

//  TBool Overlaps(const TPoint &aUpperLeft, const TPoint);

  TBool Overlaps(TRect &aOther);

public:
  void Normalize();

  void Union(const TRect &aRect);

  TBool Intersection(const TRect &aSrcRect);

  TBool Intersection(const TRect &aSrcRect1, const TRect &aSrcRect2);

public:
  TBool PointInRect(TInt x, TInt y) {
    return x >= x1 && x <= x2 && y >= y1 && y <= y2;
  }

  TBool PointInRect(TPoint p) { return PointInRect(p.x, p.y); }

public:
  void Center(TCoordinate aX, TCoordinate aY);

  void Center(TPoint &aPoint) { Center(aPoint.x, aPoint.y); }

  void Center(TRect &aRect) {
    Center(MID(aRect.x1, aRect.x2), MID(aRect.y1, aRect.y2));
  }

  void CenterX(TCoordinate aX);

  void CenterX(TRect &aRect) { CenterX(MID(aRect.x1, aRect.x2)); }

  void CenterY(TCoordinate aY);

  void CenterY(TRect &aRect) { CenterY(MID(aRect.y1, aRect.y2)); }

  void CenterInX(TRect &aRect); // centers within rect (typically ViewPort rect)
  void CenterInY(TRect &aRect); // centers within rect (typically ViewPort rect)
  void CenterIn(TRect &aRect);  // centers within rect (typically ViewPort rect)
} PACKED;

#endif //GRAPHICS_TRECT_H
