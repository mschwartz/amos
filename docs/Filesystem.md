# Simple File System

First cut:

Root sector at fixed location on the disk.  Just after the kernel sectors.

## How sectors are used

For simplicity's sake, the first TUint64 of a sector is the LBA of the next sector in the file.  A null link means end of file.

## Directory sectors

The root sector is a directory sector. 

The directory contents (e.g. filenames, file info) are stored one sector per entry in the file system (e.g. file or directory or...).

The directory entry consists of the TUint64 link to next sector, filename, attriubutes, and TUint64 link to first data sector.

## Data sectors

A data sector is TUint64 link to next sector (0 == end of file) followed by 512 - sizeof(TUint64) bytes of data.

The last sector of a file may contain as little as 1 byte of actual data.

## Performance

This file system is not likely to be very fast.  However, we can cache the directory sectors in RAM and update them in RAM 
before updating the values on disk.

We probably want to use something like an AVL tree so lookup of directory sectors by LBA in memory is fast(er).
