#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define I2C_ADDRESS 0x70

int stop =0;

void chrono(int fd);
void affichage7segments(int fd,int sec,int min);

// Tableau des valeurs hexadécimales pour chaque chiffre de 0 à 9
int hexValues[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

int main()
{
    int fd = wiringPiI2CSetup(I2C_ADDRESS); // Initialisation de l'afficheur 7 segments
    
    // Configuration de l'afficheur 7 segments
    wiringPiI2CWriteReg8(fd, 0x21, 0x01);
    wiringPiI2CWriteReg8(fd, 0x81, 0x00); 
    wiringPiI2CWriteReg8(fd, 0xef, 0x00);
    chrono(fd);
}

void chrono(int fd){
    
    int sec=0;
    int min=0; 
    affichage7segments(fd,sec,min);
    while (stop!=1){
    
    printf("seconde=%d\n",sec);
    printf("minute=%d\n",min);
    
    delay(1000);
    if (sec>=60){
      sec=0;
      min++;  
    }
    sec++; 
    affichage7segments(fd,sec,min);
    }
}

void affichage7segments(int fd,int sec, int min){

    int unite_min= min % 10 ;
    int unite_sec= sec % 10;
    int dizaine_min= min / 10;
    int dizaine_sec=  sec / 10;

    wiringPiI2CWriteReg8(fd, 0x00, hexValues[dizaine_min]); // On affiche le chiffre 1 (tout à gauche)
    wiringPiI2CWriteReg8(fd, 0x02, hexValues[unite_min]); // On affiche le chiffre 2
    wiringPiI2CWriteReg8(fd, 0x04, 0x02);//On affiche ":"
    wiringPiI2CWriteReg8(fd, 0x06, hexValues[dizaine_sec]); // On affiche le chiffre 3
    wiringPiI2CWriteReg8(fd, 0x08, hexValues[unite_sec]); // On affiche le chiffre 4 (tout à droite)



}


