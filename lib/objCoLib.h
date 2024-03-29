/**
 * @file audioStream.h
 * @brief Audio streaming header file library
 * @version 1.0
*/
#include <vlc/vlc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#ifdef JOYPI

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <lcd.h>
#include <mcp23008.h>



#define PIN_BASE 100
#define ADDRESS_MCP23008 0x20
#define I2C_ADDRESS_LCD 0x21 
#define I2C_ADDRESS 0x70

#define RS 1
#define E 2
#define DB4 3
#define DB5 4
#define DB6 5
#define DB7 6

int hexValues[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

#endif



/**
 * @brief Stream audio server
 * @param path Path to the audio file
 * @return void
*/
void streamAudioServer(char *path);

/**
 * @brief Stream audio client
 * @param ip IP address of the server
 * @return void
*/
void streamAudioClient(char *ip);

/**
 * @brief Log callback
 * @param data Data
 * @param level Level
 * @param ctx Context
 * @param fmt Format
 * @param args Arguments
 * @return void
 * @note This function is used to log the VLC messages
*/
void log_callback(void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args);


/**
 * @brief Initialize the 7-segment display
 * @return File descriptor
 * @note This function initializes the 7-segment display
 * @note This function is only available on the Raspberry Pi
*/
int init_7segment();

/**
 * @brief Display the time on the 7-segment display
 * @param fd File descriptor
 * @param sec Seconds
 * @param min Minutes
 * @return void
 * @note This function displays the time on the 7-segment display
 * @note This function is only available on the Raspberry Pi
*/
void afficher_7segment(int fd,int sec,int min);

/**
 * @brief Display the seconds on the 7-segment display
 * @param fd File descriptor
 * @param sec Seconds
 * @return void
 * @note This function displays the seconds on the 7-segment display
 * @note This function is only available on the Raspberry Pi
*/
void afficher_7segment_sec(int fd,int sec);

/**
 * @brief Initialize the LCD display
 * @return File descriptor
 * @note This function initializes the LCD display
 * @note This function is only available on the Raspberry Pi
*/
int initLCD();

/**
 * @brief Write to the LCD display
 * @param lcdHandle File descriptor
 * @param line Line
 * @param column Column
 * @param message Message
 * @return void
 * @note This function writes to the LCD display
 * @note This function is only available on the Raspberry Pi
*/
void writeLCD(int lcdHandle, int line, int column, char* message);

