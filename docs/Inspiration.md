# Inspiration

Inspiration is the display/screen/window manager for AMOS.

## Display

There is a Display class for each attached monitor.

## BScreen

Each Display manaages a list of BScreens.  The first Screen in the list is the active one, or topmost one.

Screens maintain a list of BWindows that are rendered on them.

On the original Amiga, you could click on the Screen's title bar and drag down, revealing a portion of the screen
beneath.

An application might create a screen and render all of its windows on that screen.  There is a default screen known as
Desktop (AmigaOS was Workbench), which is the familiar desktop application with icons and files and folders and so on.  BWindows
can be opened on top of the Desktop.

## BWindow

Applications inherit from BWindow to create their application windows.  The applicaton can control the window's contents and decorations and can receive and respond to event messages (IDCMPMessage).

## BConsoleWindow

This is a special class of BWindow that works like the VGA text mode hardware.  There is a 2-byte-per-character 2 dimensional array (rows and columns).  The upper byte per character position is the display attributes (foreground and background attribute of the character).  The lower byte is the character itself.

BConsoleWindow handles printing to the window, as you would to a terminal window on Linux or MacOS or Windows.  The cursor starts at row 0, column 0 and as you print characters it is moved to the right.  When a newline is printed, the cursor is moved to column 0 of the next row.  When the cursor is at the bottom and newline is printed, the text is scrolled up.  There is no history, per se.

## Cursor

The mouse pointer is repreented by a Cursor.  Applications can change the Cursor depending on context (e.g. I beam over text, pointer over the rest).


