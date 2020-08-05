# Coding conventions.

If you plan to contribute, please obey the following coding conventions.


## Variable, class, and function names

We try to make our variable, class, and function names as clear as possible.  We avoid abbreviations like Msg instead of Message.

* Base classes begin with B.  BNode, BBase, BMessage, and so on.  These are meant to be inherited from!
* Public class methods are UpperCase style.  Like SendMessage() and so on.
* class variables are mUpperCase style.  The m prefix means "member".  The reason we do this is to avoid name overrides caused by member and function argument being the same text.
* Arguments to functions are aUpperCase style.  With m and a prefixes, variables within functions are obvious.
* Variables within functions are up to the programmer.  Using i for loops, s for strings, and so on - whatever the programmer feels comfortable using.
* Global variables begin with a g prefix.  Like gExecBase.
* Types begin with a T prefix.  Like TInt, or TRGB.
* AMOS uses a different kind of variable typing than AmigaOS.  AmigaOS uses INT whlie AMOS uses TInt.

Note: static (scope file) and local variables can be named anything you like.

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
