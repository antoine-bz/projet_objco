#! /bin/bash
export PATH_CC=$(pwd)/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
export CCC=$PATH_CC/arm-linux-gnueabihf-gcc


ln -s $($CCC -print-sysroot)/lib/arm-linux-gnueabihf lib-ccc
ln -s $($CCC -print-sysroot)/usr/include include-ccc