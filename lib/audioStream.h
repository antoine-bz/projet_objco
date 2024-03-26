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




// Path: lib/audioStream.c

// on veut faire l'equivalent de la ligne de commande suivante mais en C
// cvlc chemin_vers_votre_musique --sout '#standard{access=http,mux=ogg,dst=:8080}' :sout-keep


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
void streamAudioClient(char *ip)
