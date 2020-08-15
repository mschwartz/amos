# AMOS - 64 bit OS written in C++ with a definite AmigaOS flavor.

## Preliminaries

AMOS is MIT license.  Use it as you wish, steal the code for your own hobby OS.

I (Mike Schwartz) work for Modus Create and designed a classic game style video game application framework for Modus that we call Creative Engine: https://github.com/ModusCreateOrg/creative-engine/.

AMOS borrows many of the concepts, C++ classes, algorithms, and spirit from Creative Engine.  Of particular interest are the basic C++ types it contains and graphics primitives.  The BList.hpp/.cpp files contain very handy base classes for ordered and unordered doubly linked lists.  AMOS heavily uses these lists for all sorts of things.

See the LICENSE.md file for licensing information.

Some of the code is borrowed from other OS projects.  Will try to address which sections with comments in the code.  Caveat programmer - the license for these other projects may not be MIT.

## Introduction

It seems that many/most hobby OS efforts are attempting to reinvent Linux.  AmigaOS was/is a multitasking OS with a very distinct feel of its own, unlike Linux.

One of the big differences between the AMOS design and a Linux or BSD (MacOS) design is that AMOS is basically a microkernel while the otherse are monolitic ones.

As a microkernel, the core (Exec) is fairly small in terms of the amount of code.  Device drivers and hardware interface can be done almost entirely in User space.  Exec would be called from a Device to allow direct access to hardware (enable in/out instructions, memory map in device MMIO address space) and to install and enable interrupt handlers.

## AMOS inspired by AmigaOS

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

Specific features of AmigaOS will be found in AMOS, though written from scratch!  For example, AMOS features (but is not limited to):
* Exec
* Devices
* Graphics
* Inspiration (something like Amiga's Intuition)

### Exec

The heart of Exec is ExecBase, which is accessed globally via gExecBase.  AmigaOS stores it's ExecBase pointer at location 4 in memory.  When AMOS gets to implementing User and Kernel separated space, something like this will need to be done as well.

ExecBase is the hub that controls:
* Multitasking
* Multiprocessing (multiple core support)
* Memory (MMU, AllocMem/FreeMem/etc.)
* Mutexes, Semaphores and the like
* Base classes for things like Devices, FileSystems, Tasks, Processes.
* Interrupts and Interrupt Handlers
* Messages and MessagePorts

ExecBase constructor is called at the start of the kernel code.  It sets up the machine's resources (IDT, GDT, TSS, APICs, etc.) and then starts the APs (Application Processors).

Round-robin scheduling is done within each CPU.  CPU affinity is determined by Exec as tasks move from waiting state to running state.

### Devices

Devices are typically implemented as at least one task and one interrupt handler.  The task initializes the hardware and installs and enables the Interrupt handler.  The task then loops reading BMessages from its MessagePort, performs requested operations, and replys to the BMessages with the result.

The MessagePort for a device is consistent: keybaord.device, timer.device, rtc.device, and so on.  The BMessage formats are custom per device, but very similar with one another.

All IO is basically done via request/response.  Code does not have to wait for the response - it's asynchronous, though code can Wait for the response if it has no work to do.

### Graphics

Graphics are done using Extended VGA Graphics mode, which is hard coded.  See the boot log.

AMOS borrows the BBitmap, and BViewPort classes from Creative Engine.  These two classes provide the basis for a window system with desktop(s).  

BViewPort controls rendering to windows so the pixels are clipped to the viewport's (Window) boundary.

BBitmap provides graphics primitives to operate on bitmaps: lines, plot pixels, rectangles, filled rectangles, circles, font rendering, etc.

### Inspiration

Inspiration is the default window manager.

It features a default screen that is called the Desktop (AmigaOS' Workbench concept).  The Desktop is where the application icons are rendered, as well as the system menu and icons, the file manager windows, shell/console windows, etc.

Programs can create custom screens and render windows on them.

The screens are depth arranged and you can drag the topmost one down and partially reveal the screen below. 

TODO: windows

## Documentation

The docs/ directory contains documentation about AMOS as well as Intel and AMD pdf files. These PDFs include things like the Intell Programmers Reference Manual and the IOAPIC specification.  There will be more documents added as AMOS design needs to be explained and as more standards type documents need to be referenced in a handy fashion.

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
