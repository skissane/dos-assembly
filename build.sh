#!/bin/bash
rm -rf Build
mkdir Build
./makeFloppy.sh Build/zutl.img
./toFloppy.sh Build/zutl.img
