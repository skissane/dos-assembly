#!/bin/bash
[ $# = 1 ] || {
	echo Floppy Creator - Creates a blank 1440KB MS-DOS formatted floppy image
	echo
	echo usage: $0 IMAGE
	echo -e "\twhere IMAGE is name of disk image file to create"
	exit 1
}
[ -e $1 ] && {
	echo ERROR: already exists: $1 - refusing to overwrite
	exit 1
}
dd if=/dev/zero of=$1 count=1440 bs=1024
mformat -f 1440 -i $1 ::
