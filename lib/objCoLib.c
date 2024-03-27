#include "objCoLib.h"

// log 
void log_callback(void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args) {
    char *str = malloc(1024);
    vsnprintf(str, 1024, fmt, args);
    printf("libVLC: %s\n", str);
    free(str);
}


void streamAudioServer(char *file_path) {
    libvlc_instance_t *inst;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;
  
    // Crée une nouvelle instance libVLC
    inst = libvlc_new(0, NULL);

    // Enregistre le callback de log
    //libvlc_log_set(inst, log_callback, NULL);    

    // Crée un nouvel objet média
    m = libvlc_media_new_path(inst, file_path);
    
    // Ajoute des options de streaming au média
    libvlc_media_add_option(m, ":sout=#transcode{acodec=mpga,ab=128,channels=2,samplerate=44100}:http{mux=ogg,dst=:8080/}");
    libvlc_media_add_option(m, ":no-sout-rtp-sap");
    libvlc_media_add_option(m, ":no-sout-standard-sap");
    libvlc_media_add_option(m, ":sout-keep");

    // Crée un lecteur média à partir du média
    mp = libvlc_media_player_new_from_media(m);

    // Ne plus avoir besoin du média
    libvlc_media_release(m);

    // Joue le média
    libvlc_media_player_play(mp);

    sleep(1);
    // Attendre la fin de la lecture
    while(libvlc_media_player_is_playing(mp));

    // Nettoyage
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(inst);
}

void streamAudioClient(char *ip) {
    libvlc_instance_t *vlcInstance;
    libvlc_media_player_t *mp;
    libvlc_media_t *media;
    
    char url[256];
    snprintf(url, sizeof(url), "http://%s:8080/", ip);
    
    // Initialisation de libVLC
    vlcInstance = libvlc_new(0, NULL);
    if (vlcInstance == NULL) {
        printf("Could not initialize libVLC\n");
        return;
    }
    
    // Création d'un nouveau media pour le flux réseau
    media = libvlc_media_new_location(vlcInstance, url);
    if (media == NULL) {
        printf("Could not create media for URL: %s\n", url);
        libvlc_release(vlcInstance);
        return;
    }
    
    // Création d'un media player
    mp = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media); // Le player conserve une référence au media
    
    // Lecture du media
    if (libvlc_media_player_play(mp) == -1) {
        printf("Could not play\n");
        libvlc_media_player_release(mp);
        libvlc_release(vlcInstance);
        return;
    }

    sleep(1);
    printf("Playing\n");
    // Attendre la fin de la lecture
    while(libvlc_media_player_is_playing(mp));
    
    // Nettoyage
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(vlcInstance);
}


#ifdef JOYPI

int init_7segment(){
    int fd = wiringPiI2CSetup(I2C_ADDRESS); // Initialisation de l'afficheur 7 segments
    // Configuration de l'afficheur 7 segments
    wiringPiI2CWriteReg8(fd, 0x21, 0x01);
    wiringPiI2CWriteReg8(fd, 0x81, 0x00); 
    wiringPiI2CWriteReg8(fd, 0xef, 0x00);
    return fd;
}


void afficher_7segment(int fd,int sec,int min){
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

// on recoit le temps en secondes et on affiche les secondes et les minutes
void afficher_7segment_sec(int fd,int sec){
    int min=0;
    if (sec>=60){
        // on calcule les minutes
        min= sec/60;
        sec= sec%60;


    }
    afficher_7segment(fd,sec,min);
}




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

    if (lcdHandle = lcdInit(2, 16, 4, PIN_BASE+RS,PIN_BASE+E ,PIN_BASE+DB4,PIN_BASE+DB5,PIN_BASE+DB6,PIN_BASE+DB7,0,0,0,0) < 0){
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

#endif

#ifndef JOYPI

int init_7segment(){
    return 0;
}

void afficher_7segment(int fd,int sec,int min){
    return;
}

void afficher_7segment_sec(int fd,int sec){
    return;
}

int initLCD(){
    return 0;
}

void writeLCD(int lcdHandle, int line, int column, char* message){
    return;
}

#endif