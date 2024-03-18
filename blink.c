
#include <stdio.h>
#include <wiringPi.h>

// clignotement de la patte ARG1
// période ARG2 en ms 

// Compiler en croisé
// faire une édition de liens avec la lib. wiringPi
// penser à une cible "install" utilisant la commande sshpasswd

// V2 : utiliser la fonction softTone 

#define	DIODE	0
#define	PERIODE	500
#define BUZZER 1
#define BOUTON 24

// pullup 
#define APPUYE LOW
#define RELACHE HIGH

void jouer(){
	if (digitalRead(BOUTON) == APPUYE) 
		softToneWrite (BUZZER, 440) ; // son actif
	else 
		softToneWrite (BUZZER, 0) ;
}

int main (int argc, char ** argv) {
	int diode, periode; 
	diode = DIODE; 
	periode = PERIODE; 
	
	if (argc >1) {
		diode = atoi(argv[1]); 
	}
	if (argc >2) {
		periode = atoi(argv[2]); 
	}
	
  printf ("Raspberry Pi blink patte %d, periode %d\n", diode, periode) ;

  wiringPiSetup () ;
	softToneCreate (BUZZER) ;
  pinMode (diode, OUTPUT) ;
  pinMode (BOUTON, INPUT) ;
  pullUpDnControl(BOUTON, PUD_UP);
  
  /*
  void pullUpDnControl (int pin, int pud) ;
This sets the pull-up or pull-down resistor mode on the given pin, which should be set as an input. Unlike the Arduino, the BCM2835 has both pull-up an down internal resistors. The parameter pud should be; PUD_OFF, (no pull up/down), PUD_DOWN (pull to ground) or PUD_UP (pull to 3.3v) The internal pull up/down resistors have a value of approximately 50KΩ on the Raspberry Pi.
  */
  

  for (;;)
  {
    digitalWrite (diode, HIGH) ;	// On
    jouer();
    delay (periode) ;		// mS
    digitalWrite (diode, LOW) ;	// Off
    softToneWrite (BUZZER, 0) ;
    delay (periode) ;
  }
  return 0 ;
}
