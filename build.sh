#!/bin/sh

set -e

. ./shlib/common.sh

if [ "$WSL" != "" ]; then
    ./cbuild.sh
else
    # build docker image if it doesn't exist
    docker image inspect cross >/dev/null 2>&1 || docker build --tag cross:latest .
    docker run -it --rm --name build -v "$PWD":/usr/src/myapp -w /usr/src/myapp cross ./cbuild.sh
fi
