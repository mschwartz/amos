#!/bin/sh

. ./shlib/platform.sh

echo $platform
./build.sh
case $platform in
    "wsl" )
	echo bochsrc.wsl
    	cp bochsrc.wsl bochsrc
	;;
    "macos" )
	echo bochsrc.macos
    	cp bochsrc.macos bochsrc
	;;
    "arch" )
	echo bochsrc.linux
    	cp bochsrc.linux bochsrc
	;;
    "linux" )
	echo bochsrc.linux
    	cp bochsrc.linux bochsrc
	;;
    * )
	echo "unknown platform " $platform
	;;
esac
echo $platform
sudo chmod 777 c.img
rm *.lock
bochs -q

# if [ "$WSL" != "" ]; then
# 	if [ -e "/mnt/c/dev/bare.log" ]; then
# 	    mv /mnt/c/dev/bare.log /mnt/c/dev/bare.log.1
# 	fi
# 	cp bochsrc /mnt/c/dev/
# 	cp bare.img /mnt/c/dev
# 	ping -c 4 192.168.1.1 >/dev/zero
# 	"$VBOX/VirtualBoxVM.exe" \
    # 	    --startvm  \
    # 	    bare \
    # 	    --debug 
# 	ping -c 4 192.168.1.1 >/dev/zero
# 	tail -f /mnt/c/dev/bare.log
# else
# 	if [ $platform = "macos" ]; then
# 	    cp bochsrc.macos bochsrc
# 	else
# 	    cp bochsrc.linux bochsrc
# 	fi
# 	bochs -q
# fi

# #qemu-system-x86_64 bare.img -boot a -s -S
