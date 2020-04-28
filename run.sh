#!/bin/bash

set -e

#./build.sh && qemu-system-x86_64 -d int -overcommit cpu-pm=on -m 8192  -enable-kvm c.img
./build.sh && qemu-system-x86_64 -m 8192  -enable-kvm -m 8192 -smp 4 -cpu host,hv_relaxed,hv_spinlocks=0x1fff,hv_vapic,hv_time c.img
