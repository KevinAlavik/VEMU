#!/bin/bash

echo "Hello, VISC!"> boot.fs
dd if=/dev/zero of=boot.img bs=1440k count=1
dd conv=notrunc if=boot.fs of=boot.img

rm boot.fs