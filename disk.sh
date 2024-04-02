#!/bin/bash

echo "Hello, World!" > floppy.txt
dd if=/dev/zero of=boot.img bs=1440k count=1
dd conv=notrunc if=floppy.txt of=boot.img

rm floppy.txt