# Developing

AMOS uses C++, but is not purely object oriented.  Rather it is "Object Based," so we use Objects in C style
methods, and we use global variables.

## Naming Conventions

* Classes are named BClassName if it is a base type class, intendted to be inherited from.
* Class and structure member variables are named mVariableName
* Global variables are named gVariableName
* File local static variables are named sVariableName
* Arguments to methods are named aArgumentName
* Method local variables, including statics, may be any name at all
* File local static functions are named function_name();

## BBase

BBase is the base class for EVERYTHING!  If you inherit from a BClass, you will ultimately have intherited from a
descendant of BBase.

## BList

AMOS relies heavily on doubly linked lists.  BList.h has two kinds of linked lists: BNode and BNodePri.  

If you inherit list items (nodes) from BNode and use BList, you end up with a simple unsorted linked list.  You can 
add and remove nodes from the head and tail of the BList.

If you inherit list items (nodes) from BNodePri and use BListPri, you end up with a sorted linked list.  You can add 
BNodePri nodes to unsorted lists using AddHead/AddTail, but the list will remain unsorted.

Both types of nodes have a string mNodeName member.  This is useful for debugging and for informational purposes.  The
mName is used, for example, as a MessagePort's name.

NOTE: list manipulation with interrupts and tasking enabled can cause incoherent lists.  Consider that inserting a node
requires storing to the next and prev members and to the next/prev members of other nodes.  If an interrupt occurs
between these stores, the list is corrupt.

## ExecBase

ExecBase is the mother of everything.  There is a gExecBase global variable that allows you access to ExecBase
from anywhere.  On the original AmigaOS, ExecBase was pointed to by location 4 in low memory.  Since we're using
C++, we can just access the global.

ExecBase maintains BInterrupts, MessagePorts, Devices, (eventually Libraries), etc.

TODO: deal with gExecBase for user space.

## BTask

BTask is the abstract base class for AMOS tasking.  You implement a Run() method for your child class and you 
add your task to the global task list(s) via ExecBase.

Tasking is priority based.  The mPri field of BNodePri (base class for BTask) determines the sort order for the
active task list.  The first task in that list is run when a task switch is scheduled.  The current task is removed
from the list and re-added, which causes it to be inserted at the end of all tasks with the same priority.  This 
implements round-robin task scheduling for tasks of the same priority.

Tasks that are blocking on some event will be removed from the active list and added to the waiting task list.

## MessagePorts and Signals

BTasks have 64 bits of signals that can be sent from outside the task.  When a task is signaled, it is removed from 
the waiting task list (if it is blocking) and added to the active task list.

A task will call Wait(aSignalMask) to wait for any of the bits in the mask.  When the task is signaled and wakes up,
the actual mask of received bits are returned.

AMOS relies heavily on message passing.  You can send messages to a MessagePort owned by a task.  The task 
should be programmed to wait on that MessagePort and then retrieve all messages in the MessagePort before
waiting for more messages.

The sender typically allocates a message and sends it to a task.  The receiver retrieves the message, acts upon it,
and replies to the message.  The sender will get the reply in it's reply port (MessagePort) and can then free
the message.  This is the Amiga OS style convention.

MessagePorts may be private or public.  To make a port public, you call gExecBase.AddMessagePort().  You should
be careful to not have multiple ports with the same name, including base OS ports.  Public MessagePorts can be found
using gExecBase.FindMessagePort(aName).

## BDevice

BDevice is the base class for all devices (keyboard, mouse, timers, disk, etc.).  Devices typically manipulate hardware
configuration, install interrupt handlers, and process I/O requests via a MessagePort.

Devices in AMOS create MessagePorts with standard names like "timer.device" and "keyboard.device".


