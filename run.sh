#!/bin/bash

set -e

#./build.sh && qemu-system-x86_64 -d int -overcommit cpu-pm=on -m 8192  -enable-kvm c.img
./build.sh && qemu-system-x86_64 -m 8192  -enable-kvm -cpu host c.img
