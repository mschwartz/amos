#!/bin/sh

set -e

find . -name '*.o' -delete
find . -name '*.a' -delete
find . -name '*.img' -delete
find . -name '*.raw' -delete
#rm -rf *.o */*.o */*/*.o cmake-build-release cmake-build-debug *.img */*.img
