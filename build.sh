#!/bin/sh

set -e

. ./shlib/common.sh

# build docker image if it doesn't exist
docker image inspect cross >/dev/null 2>&1 || docker build --tag cross:latest .
echo ""
echo ""
echo "BULIDING IN DOCKER CONTAINER"

# docker run -it --rm --name build -v "$PWD":/usr/src/myapp -w /usr/src/myapp cross make -j 1 c.img
docker run -it --rm --name build -v "$PWD":/usr/src/myapp -w /usr/src/myapp cross make -j 20 c.img
# docker run -it --rm --name build -v "$PWD":/usr/src/myapp -w /usr/src/myapp cross ./cbuild.sh

echo ""
echo ""
echo "-> BUILT IN DOCKER CONTAINER"

echo ""
ls -l iso/*.img
echo ""
ls -l c.img

nm iso/kernel.elf | grep " T " | awk '{ print $1" "$3 }' > amos.sym
echo ""
wc -l amos.sym
