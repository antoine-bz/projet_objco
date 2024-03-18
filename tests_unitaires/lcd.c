#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <lcd.h>
#include <mcp23008.h>

#define PIN_BASE 100
#define ADDRESS_MCP23008 0x20
#define I2C_ADDRESS_LCD 0x21 

#define RS 1
#define E 2
#define DB4 3
#define DB5 4
#define DB6 5
#define DB7 6



int initLCD(){
    
    int i;
    int lcdHandle;
    int fd = wiringPiI2CSetup(I2C_ADDRESS_LCD);
    for (i=0; i<9; i++){
        wiringPiI2CWriteReg8(fd, i, 0xFF);
    }
    wiringPiSetupSys();
    mcp23008Setup(PIN_BASE, I2C_ADDRESS_LCD); // 100 is the base pin number, 0x20 is the I2C address of the MCP23008
    pinMode(PIN_BASE, OUTPUT); // Set the MCP23008 pin 0 as an output

    lcdHandle = lcdInit(2, 16, 4, PIN_BASE+RS,PIN_BASE+E ,PIN_BASE+DB4,PIN_BASE+DB5,PIN_BASE+DB6,PIN_BASE+DB7,0,0,0,0); 
        printf("lcdInit failed!\n");
        return -1;
    }
    lcdDisplay(lcdHandle, 1);
    return lcdHandle;

}

void writeLCD(int lcdHandle, int line, int column, char* message){
    lcdPosition(lcdHandle, line, column);
    lcdPuts(lcdHandle, message);
    
}


int main(){
    int lcdHandle = initLCD();
    writeLCD(lcdHandle, 0, 0, "TEST");
    printf("ON ECRIT\n");
    return 0;
}