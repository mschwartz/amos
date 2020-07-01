# AMOS - 64 bit OS written in C++ with a definite AmigaOS flavor.

It seems that many/most hobby OS efforts are attempting to reinvent Linux.  AmigaOS was/is a multitasking OS with a very distinct feel of its own, unlike Linux.

The original AmigaOS came out in 1985, at a time when 256 color graphics cards were very expensive and PCs were running DOS with a 640K memory limit.  

There was no Linux at the time.  Unix was an AT&T thing, but commercial and expensive.  BSD was Berkeey's open source clone of Unix that was available for free.  

Until OS/2 became stable enough to use, only Unix and AmigaOS were popular multitasking operating systems.

AmigaOS was written in C (still is).  So things like linked list node structure is implemented like:

```
struct MessagePort {
  struct node node;
  ... rest of struct
};
```

AMOS is written in C++, and things like the node example above are done via inheritance:
```
struct MessagePort : public Node {
  ...
};
```

While AMOS is written in C++, it is written using an Object Based style vs. OOP.  The C++ code in AMOS does not use namespaces or templates or other advanced C++ features.  The concept of Object Based C++ is that instead of defining a struct and then several functions that perform operations on that struct (funcitons with awkward names!), we can simply use a Class and bind the methods that way.  Outside of classes, the code is very C-like.

## Coding conventions.

* Base classes begin with B.  BNode, BBase, BMessage, and so on.  These are meant to be inherited from!
* Public class methods are UpperCase style.  Like SendMessage() and so on.
* class variables are mUpperCase style.  The m prefix means "member".  The reason we do this is to avoid name overrides caused by member and function argument being the same text.
* Arguments to functions are aUpperCase style.  With m and a prefixes, variables within functions are obvious.
* Variables within functions are up to the programmer.  Using i for loops, s for strings, and so on - whatever the programmer feels comfortable using.
* Global variables begin with a g prefix.  Like gExecBase.
* Types begin with a T prefix.  Like TInt, or TRGB.
* AMOS uses a different kind of variable typing than AmigaOS.  AmigaOS uses INT whlie AMOS uses TInt.

## Basic Types
* TInt - int
* TInt8 - 8-bit int
* TInt16 - 16-bit int
* TInt32 - 32-bit int
* TInt64 - 64 bit int

* TUint - unsigned int
* TUint8 - unsigned 8-bit int
* TUint16 - unsigned 16-bit int
* TUint32 - unsigned 32-bit int
* TUint64 - unsigned 64-bit int

* char - same as TInt8, but used for strings

* TAny - same as void.  For example, TAny *foo (void *foo)

## TODO
[ ] Multicore
[ ] Better ACPI
[ ] Better PCI
[ ] Sound
[ ] Overlapping windows
[ ] AmigaOS style .library methods
[ ] Higher half kernel
[ ] VMM and protected Task space (and threads)
[ ] Mutexes


## Useful links
* http://www.independent-software.com/operating-system-development.html  
* https://wiki.osdev.org/PCI_IDE_Controller

## useful pragmas
#pragma GCC target("no-sse2")

## Small gunzip library (5 small files)
https://github.com/jibsen/tinf

## Other Hobby OSes

* toaruos - https://github.com/klange/toaruos
