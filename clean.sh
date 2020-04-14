#!/bin/sh

set -e

find . -name '*.o' -print
find . -name '*.o' -delete
find . -name '*.a' -print
find . -name '*.a' -delete

