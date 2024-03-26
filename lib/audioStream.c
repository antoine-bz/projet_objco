#include "audioStream.h"


void streamAudioServer(char *file_path){
    // Initialisation de VLC
    libvlc_instance_t *vlc_inst;
    libvlc_media_t *media;
    libvlc_media_player_t *media_player;

    // Initialisation de libvlc
    vlc_inst = libvlc_new(0, NULL);

    // Création de l'objet média
    media = libvlc_media_new_path(vlc_inst, file_path);

    // Création du lecteur média
    media_player = libvlc_media_player_new_from_media(media);

    // Libération de la mémoire du média puisqu'il n'est plus nécessaire
    libvlc_media_release(media);

    // Création du serveur de streaming
    libvlc_media_player_play(media_player);

    // Boucle infinie pour garder le serveur actif
    printf("Le serveur de streaming est actif. Appuyez sur Ctrl+C pour arrêter.\n");
    while (1) {
        sleep(1); // Mise en attente pour éviter que le programme se termine immédiatement
    }

    // Libération de la mémoire et arrêt de VLC
    libvlc_media_player_stop(media_player);
    libvlc_media_player_release(media_player);
    libvlc_release(vlc_inst);

}

void streamAudioClient(char *ip){
    // Initialisation de VLC
    libvlc_instance_t *vlc_inst;
    libvlc_media_player_t *media_player;
    char *stream_url = malloc(100 * sizeof(char));

    // Initialisation de libvlc
    vlc_inst = libvlc_new(0, NULL);

    // Création du lecteur média
    media_player = libvlc_media_player_new(vlc_inst);

    // Création de l'URL du flux
    snprintf(stream_url, 100, "http://%s:8080", ip);

    // Lecture du flux
    libvlc_media_player_set_media(media_player, libvlc_media_new_location(vlc_inst, stream_url));
    libvlc_media_player_play(media_player);

    // Boucle infinie pour garder le lecteur actif
    printf("En écoute du flux. Appuyez sur Ctrl+C pour arrêter.\n");
    while (1) {
        sleep(1); // Mise en attente pour éviter que le programme se termine immédiatement
    }

    // Libération de la mémoire et arrêt de VLC
    libvlc_media_player_stop(media_player);
    libvlc_media_player_release(media_player);
    libvlc_release(vlc_inst);
}


