#include <stdio.h>
#include <wiringPi.h>

// hypothèse pullup 
#define APPUYE LOW
#define RELACHE HIGH

#define PIN_IN_GAUCHE 6
#define PIN_IN_DROITE 24
#define PIN_IN_HAUT 25
#define PIN_IN_BAS 23

#define HAUT    1
#define BAS     2
#define GAUCHE  3
#define DROITE  4


int button ()
{
	
	wiringPiSetup () ;
	pinMode (PIN_IN_GAUCHE, INPUT) ;
    pinMode (PIN_IN_DROITE, INPUT) ;
    pinMode (PIN_IN_HAUT, INPUT) ;
    pinMode (PIN_IN_BAS, INPUT) ;

	while (1) {

        if (digitalRead(PIN_IN_GAUCHE) == APPUYE)
        {
            return GAUCHE;
        }
        if (digitalRead(PIN_IN_DROITE) == APPUYE)
        {
            return DROITE;
        }
        if(digitalRead(PIN_IN_HAUT) == APPUYE)
        {
            return HAUT;
        }
        if(digitalRead(PIN_IN_BAS) == APPUYE)
        {
            return BAS;
        }
	}

	return 0 ;
}








int main(){
    
    int a;
    a = button();
    printf("%d\n", a);
    return 0;

}