#! /bin/bash


export PATH2=$(pwd)
export PATH_CC=$PATH2/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
export LD_LIBRARY_PATH=/home/antoine/Documents/LE3/OBJCO/projet_objco/target_portaudio/lib:$LD_LIBRARY_PATH
export CCC=$PATH_CC/arm-linux-gnueabihf-gcc
export CC=$CCC
export CXX=$PATH_CC/arm-linux-gnueabihf-g++

cd portaudio

make clean

./configure --prefix=$PATH2/target_portaudio --host=x86_64-build_unknown-linux-gnu --target=arm-linux-gnueabihf 

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
