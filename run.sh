#!/bin/bash

set -e

./build.sh && qemu-system-x86_64 bare.img
