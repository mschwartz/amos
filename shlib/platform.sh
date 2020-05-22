#!/bin/bash

platform_type=`uname -m`
unamestr=`uname`
platform='unknown'
if [[ "$unamestr" == 'Linux' ]]; then
    platform='linux'
    if [[ -e "/etc/arch-release" ]]; then
	platform='arch'
    fi
elif [[ "$unamestr" == 'armv7' ]]; then
    platform='linux'
elif [[ "$unamestr" == 'Darwin' ]]; then
    platform='macos'
    install() {
      if [[ -e "/usr/local/bin/$1" ]]; then
        brew upgrade $1
      else
        brew install $1
      fi
    }
fi
export platform_type
export platform
export hostname=`hostname -s`
export numcpus=`getconf _NPROCESSORS_ONLN`
