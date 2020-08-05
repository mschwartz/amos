# AMOS - 64 bit OS written in C++ with a definite AmigaOS flavor.

AMOS is MIT license.  Use it as you wish, steal the code for your own hobby OS.

## Introduction

It seems that many/most hobby OS efforts are attempting to reinvent Linux.  AmigaOS was/is a multitasking OS with a very distinct feel of its own, unlike Linux.

I (Mike Schwartz) work for Modus Create and designed a classic game style video game application framework for Modus that we call Creative Engine: https://github.com/ModusCreateOrg/creative-engine/.

AMOS borrows many of the concepts, C++ classes, algorithms, and spirit from Createive Engine.

See the LICENSE.md file for licensing information.

## AmigaOS

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

## Requirements

The build system uses Docker so you don't need to install a cross compiler or anything else in your enviromnent to build AMOS.

The build.sh script uses a customized Docker gcc container (we add nasm) to compile and assembly everything.

You do need to have qemu/kvm (to run in emulator on linux/mac) and bochs installed for development.

For Windows, the build environment uses Linux under WSL2.  You need an X server installed (there are free options), and Docker Desktop.  You also need VirtualBox with a machine named bare created and its boot drive as our c.img.  Yes, we run in VirtualBox!


## Useful links
* OSDEV.org wiki - https://wiki.osdev.org/
* AmigaOS Documentation Wiki - https://wiki.amigaos.net/wiki/Main_Page
* http://www.independent-software.com/operating-system-development.html  

## useful pragmas
#pragma GCC target("no-sse2")

## Small gunzip library (5 small files)
https://github.com/jibsen/tinf

## Other Hobby OSes

* toaruos - https://github.com/klange/toaruos
