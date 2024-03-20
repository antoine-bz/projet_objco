#! /bin/bash
export PATH_CC=$(pwd)/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
export CCC=$PATH_CC/arm-linux-gnueabihf-gcc
export CC=$CCC
export CXX=$PATH_CC/arm-linux-gnueabihf-g++

cd portaudio
./configure --prefix=/home/antoine/Documents/IG2I-L3/objco/CCR3/ncurses/target_portaudio --host=x86_64-build_unknown-linux-gnu --target=arm-linux-gnueabihf 

make -j9
make install

cd ..
    
ln -s $($CCC -print-sysroot)/lib/arm-linux-gnueabihf lib-ccc
ln -s $($CCC -print-sysroot)/usr/include include-ccc
