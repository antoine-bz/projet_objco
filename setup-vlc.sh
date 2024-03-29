#! /bin/bash


export PATH2=$(pwd)
export PATH_CC=$PATH2/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
export LD_LIBRARY_PATH=$PATH2/target_vlc/lib:$LD_LIBRARY_PATH
export CCC=$PATH_CC/arm-linux-gnueabihf-gcc
#export CC=$CCC
#export CXX=$PATH_CC/arm-linux-gnueabihf-g++

TRIPLET=$PATH_CC/arm-linux-gnueabihf


export CC=$TRIPLET-gcc
export CXX=$TRIPLET-g++
export CPP=$TRIPLET-cpp
export AR=$TRIPLET-ar
export RANLIB=$TRIPLET-ranlib
export ADD2LINE=$TRIPLET-addr2line
export AS=$TRIPLET-as
export LD=$TRIPLET-ld
export NM=$TRIPLET-nm
export STRIP=$TRIPLET-strip

cd vlc-master

make clean

./configure --prefix=$PATH2/target_portaudio --host=x86_64-build_unknown-linux-gnu --target=arm-linux-gnueabihf CXXFLAGS="-std=c++11"


make -j9
sudo make install

make clean

#./configure && make

#sudo make install

#cp lib/.libs/libportaudio.a $PATH2/target_portaudio-pc/
#cp /usr/local/lib/libportaudio.a $PATH2/target_portaudio-pc/

cd ..
    
ln -s $($CCC -print-sysroot)/lib/arm-linux-gnueabihf lib-ccc
ln -s $($CCC -print-sysroot)/usr/include include-ccc
