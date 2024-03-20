# sudo apt-get install portaudio19-dev

all: blink softTone

PATH_CC=$(CURDIR)/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin

CCC=$(PATH_CC)/arm-linux-gnueabihf-gcc

TARGET_WPI=/media/khouloud/Antoine-USB/objco/CCR3/wiring/target-wpi

CFLAGS=-I$(TARGET_WPI)/include
LDFLAGS=-L$(TARGET_WPI)/lib 

blink: blink.c 
	$(CCC) $^ -o $@ -lwiringPi
	
softTone: softTone.c 
	$(CCC) $^ -o $@  -lwiringPi	

example: example.c 
	$(CCC) $^ -o $@ -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt 
	
server:server.c
	$(CCC) $^ -o $@ -lportaudio -lm -pthread -lrt
	gcc $^ -o $@.exe -lportaudio -lm -pthread -lrt
	
install : blink softTone
	scp example softTone pi@192.168.137.40:/home/pi/Desktop/

clean: 
	rm -rf blink softTone example
