#!/bin/sh

./build.sh

qemu-system-x86_64 -s -S -serial stdio  -m 8192  -enable-kvm -m 8192 -smp 4 -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time c.img
#qemu-system-x86_64 bare.img -boot a -s -S

