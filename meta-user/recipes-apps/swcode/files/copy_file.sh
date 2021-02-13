#!/bin/sh
make clean;
make;
rm -rf /mnt/hgfs/Images/swcode;
cp ./swcode /mnt/hgfs/Images;
