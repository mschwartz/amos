#!/bin/bash

set -e

./build.sh

#./build.sh && qemu-system-x86_64 -d int -overcommit cpu-pm=on -m 8192  -enable-kvm c.img
if [ ! -x "$WSL" ]; then
  if [ -e "/mnt/c/dev/bare.log" ]; then
    mv /mnt/c/dev/bare.log /mnt/c/dev/bare.log.1
  fi
  "$VBOX/VBoxManage.exe" \
    startvm  \
    bare
  tail -f /mnt/c/dev/bare.log
else
  qemu-system-x86_64 -serial stdio  -m 8192  -enable-kvm -m 8192 -smp 4 -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time c.img
fi
