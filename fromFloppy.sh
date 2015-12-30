#!/bin/bash
[ $# = 1 ] || {
	echo Copies back files from disk image
	echo usage: $0 IMAGE
	exit 1
}
[ -e $1 ] || {
	echo ERROR: image not found - $1
	exit 1
}
mcopy -n -i $1 ::*.* DOS
