#!/bin/bash
[ $# = 1 ] || {
	echo Deploys files to disk image
	echo usage: $0 IMAGE
	exit 1
}
[ -e $1 ] || {
	echo ERROR: image not found - $1
	exit 1
}
mcopy -o -i $1 DOS/* ::
