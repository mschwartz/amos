#!/bin/bash

set -e

. ./shlib/platform.sh

./build.sh

#./build.sh && qemu-system-x86_64 -d int -overcommit cpu-pm=on -m 8192  -enable-kvm c.img
if [ "$WSL" != "" ]; then
    if [ -e "/mnt/c/dev/bare.log" ]; then
	mv /mnt/c/dev/bare.log /mnt/c/dev/bare.log.1
    fi
    "$VBOX/VBoxManage.exe" \
	startvm  \
	bare
    #  tail -f /mnt/c/dev/bare.log
else
    if [ $platform = "macos" ]; then
	qemu-system-x86_64 -serial stdio  -m 8192  -accel hvf -m 8192 -smp 4 -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time c.img
    else
	qemu-system-x86_64 -serial stdio  -m 8192  -enable-kvm -m 8192 -smp 4 -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time c.img
    fi
fi
