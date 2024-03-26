#include "audioStream.h"

void streamAudioServer(char *file_path) {
    libvlc_instance_t *inst;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;

    // Crée une nouvelle instance libVLC
    inst = libvlc_new(0, NULL);

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

    // Garde le programme en cours d'exécution
    printf("Streaming. Press enter to exit...\n");
    getchar();

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
    
    printf("Streaming from %s. Press Enter to stop.\n", url);
    getchar(); // Attend que l'utilisateur appuie sur Entrée
    
    // Nettoyage
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(vlcInstance);
}
