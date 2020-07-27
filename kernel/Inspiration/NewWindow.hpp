#ifndef INSPIRATION_NEWWINDOW_H
#define INSPIRATION_NEWWINDOW_H

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// mouse buttons (IDCMP_MOUSEBUTTONS)
const TUint64 SELECTDOWN = 1;
const TUint64 SELECTUP = 2;
const TUint64 MIDDLEDOWN = 3;
const TUint64 MIDDLEUP = 4;
const TUint64 MENUDOWN = 5;
const TUint64 MENUUP = 6;

// these are actual AmigaOS defines.
// TODO: review and modify/remove/update these

const TUint64 IDCMP_SIZEVERIFY = 0x00000001;
const TUint64 IDCMP_NEWSIZE = 0x00000002;
const TUint64 IDCMP_REFRESHWINDOW = 0x00000004;
const TUint64 IDCMP_MOUSEBUTTONS = 0x00000008;
const TUint64 IDCMP_MOUSEMOVE = 0x00000010;
const TUint64 IDCMP_GADGETDOWN = 0x00000020;
const TUint64 IDCMP_GADGETUP = 0x00000040;
const TUint64 IDCMP_REQSET = 0x00000080;
const TUint64 IDCMP_MENUPICK = 0x00000100;
const TUint64 IDCMP_CLOSEWINDOW = 0x00000200;
const TUint64 IDCMP_RAWKEY = 0x00000400;
const TUint64 IDCMP_REQVERIFY = 0x00000800;
const TUint64 IDCMP_REQCLEAR = 0x00001000;
const TUint64 IDCMP_MENUVERIFY = 0x00002000;
const TUint64 IDCMP_NEWPREFS = 0x00004000;
const TUint64 IDCMP_DISKINSERTED = 0x00008000;
const TUint64 IDCMP_DISKREMOVED = 0x00010000;
const TUint64 IDCMP_WBENCHMESSAGE = 0x00020000; // System use only
const TUint64 IDCMP_ACTIVEWINDOW = 0x00040000;
const TUint64 IDCMP_INACTIVEWINDOW = 0x00080000;
const TUint64 IDCMP_DELTAMOVE = 0x00100000;
const TUint64 IDCMP_VANILLAKEY = 0x00200000;
const TUint64 IDCMP_INTUITICKS = 0x00400000;

//  for notifications from "boopsi" gadgets
const TUint64 IDCMP_IDCMPUPDATE = 0x00800000;

// for getting help key report during menu session
const TUint64 IDCMP_MENUHELP = 0x01000000;

// for notification of any move/size/zoom/change window
const TUint64 IDCMP_CHANGEWINDOW = 0x02000000;
const TUint64 IDCMP_GADGETHELP = 0x04000000;

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// --- Flags requested at OpenWindow() time by the application ---------
const TUint64 WFLAG_SIZEGADGET = 0x00000001;  // include sizing system-gadget?
const TUint64 WFLAG_DRAGBAR = 0x00000002;     // include dragging system-gadget?
const TUint64 WFLAG_DEPTHGADGET = 0x00000004; // include depth arrangement gadget?
const TUint64 WFLAG_CLOSEGADGET = 0x00000008; // include close-box system-gadget?

const TUint64 WFLAG_SIZEBRIGHT = 0x00000010;  // size gadget uses right border
const TUint64 WFLAG_SIZEBBOTTOM = 0x00000020; // size gadget uses bottom border

// --- refresh modes ------------------------------------------------------
// combinations of the WFLAG_REFRESHBITS select the refresh type

const TUint64 WFLAG_REFRESHBITS = 0x000000C0;
const TUint64 WFLAG_SMART_REFRESH = 0x00000000;
const TUint64 WFLAG_SIMPLE_REFRESH = 0x00000040;
const TUint64 WFLAG_SUPER_BITMAP = 0x00000080;
const TUint64 WFLAG_OTHER_REFRESH = 0x000000C0;
const TUint64 WFLAG_BACKDROP = 0x00000100;      // this is a backdrop window
const TUint64 WFLAG_REPORTMOUSE = 0x00000200;   // to hear about every mouse move
const TUint64 WFLAG_GIMMEZEROZERO = 0x00000400; // a GimmeZeroZero window
const TUint64 WFLAG_BORDERLESS = 0x00000800;    // to get a Window sans border
const TUint64 WFLAG_ACTIVATE = 0x00001000;      // when Window opens, it's Active

// --- Other User Flags ---------------------------------------------------
const TUint64 WFLAG_RMBTRAP = 0x00010000;       // Catch RMB events for your own
const TUint64 WFLAG_NOCAREREFRESH = 0x00020000; // not to be bothered with REFRESH

const TUint64 WFLAG_NW_EXTENDED = 0x00040000; // extension data provided
                                              // see struct ExtNewWindow

const TUint64 WFLAG_NEWLOOKMENUS = 0x00200000; // window has NewLook menus

// These flags are set only by Intuition.  YOU MAY NOT SET THEM YOURSELF!
const TUint64 WFLAG_WINDOWACTIVE = 0x00002000;  // this window is the active one
const TUint64 WFLAG_INREQUEST = 0x00004000;     // this window is in request mode
const TUint64 WFLAG_MENUSTATE = 0x00008000;     // Window is active with Menus on
const TUint64 WFLAG_WINDOWREFRESH = 0x01000000; // Window is currently refreshing
const TUint64 WFLAG_WBENCHWINDOW = 0x02000000;  // WorkBench tool ONLY Window
const TUint64 WFLAG_WINDOWTICKED = 0x04000000;  // only one timer tick at a time

const TUint64 WFLAG_VISITOR = 0x08000000; // visitor window
const TUint64 WFLAG_ZOOMED = 0x10000000;  // identifies "zoom state"
const TUint64 WFLAG_HASZOOM = 0x20000000; // window has a zoom gadget

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BScreen;
class BFont;

struct TNewWindow {
  TInt32 mLeft, mTop, mWidth, mHeight;
  TInt32 mMinWidth = 0, mMinHeight = 0;
  TInt32 mMaxWidth = 0, mMaxHeight = 0;
  const char *mTitle;
  TUint64 mIdcmpFlags = 0;
  TUint64 mWindowFlags = WFLAG_DRAGBAR;
  TUint32 mBackgroundColor = 0x000000,
          mForegroundColor = 0xffffff;
  BScreen *mScreen = ENull; // pointer to custom BScreen or ENull for Desktop
  BConsoleFont32 *mFont = ENull;
public:
  void Dump() const {
    dprint("\n\nTNewWindow at %x\n", this);
    dlog("left, top, width, height: %d,%d, %d,%d\n", mLeft, mTop, mWidth, mHeight);
    dlog(                 "mScreen: %x\n", mScreen);
  }
};

#endif
