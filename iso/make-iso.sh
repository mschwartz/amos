#!/bin/sh

DISKTOOL="../tools/disk-tool/disk-tool"
echo "Building filesystem/ISO"

$DISKTOOL format "AMOS HD"
$DISKTOOL mkdir fonts
$DISKTOOL cp ../resources/fonts/console-fonts/* fonts
$DISKTOOL df
