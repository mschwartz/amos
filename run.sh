#!/bin/bash

set -e

. ./shlib/platform.sh

./build.sh

# sudo chmod 644 c.img

#./build.sh && qemu-system-x86_64 -d int -overcommit cpu-pm=on -m 8192  -enable-kvm c.img
if [ "$WSL" != "" ]; then
	# sudo qemu-system-x86_64 \
	#      -serial stdio  \
	#      -m 8192  \
	#      -smp 4 \
	#      -drive  format=raw,media=disk,file=c.img

 sudo qemu-system-x86_64 \
      -serial stdio  \
      -m 8192  \
      -enable-kvm \
      -smp 4 \
      -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time \
      -drive  format=raw,media=disk,file=c.img
#    if [ -e "/mnt/c/dev/bare.log" ]; then
#  mv /mnt/c/dev/bare.log /mnt/c/dev/bare.log.1
#    fi
#    "$VBOX/VBoxManage.exe" \
#  startvm  \
#  bare
    #  tail -f /mnt/c/dev/bare.log
else
    if [ $platform = "macos" ]; then
	qemu-system-x86_64 -serial stdio  -m 8192  -accel hvf -m 8192 -smp 4 -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time c.img
    else
	sudo qemu-system-x86_64 \
	     -serial stdio  \
	     -m 8192  \
	     -enable-kvm \
	     -smp 4 \
	     -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time \
	     -drive  format=raw,media=disk,file=c.img
    fi
fi
