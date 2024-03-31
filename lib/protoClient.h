/**
 * \file protoClient.h
 * \brief Fichier d'entête de protoClient.c
 * \version 0.1
*/


#include "reqRep.h"
#include <sys/types.h>
#include <vlc/vlc.h>
#include "objCoLib.h"

/**
 * \def BITS
 * \brief Définition du nombre de bits pour le son
*/
#define BITS 8


#define DISCONNECT_CHOICE 0

/**
 * \fn void client (char *addrIPsrv, short port)
 * \brief Fonction qui crée un client pour envoyer des demandes au serveur
 * \param addrIPsrv Adresse IP du serveur
 * \param port Port du serveur
 * \return void
 * \details Cette fonction crée un client pour envoyer des demandes au serveur
*/
void client (char *addrIPsrv, short port);
/**
 * \fn void lancerMusique(char *file_name, int tempsEcoule)
 * \brief Fonction qui lance une musique
 * \param file_name Nom de la musique
 * \param tempsEcoule Temps écoulé
 * \return void
 * \details Cette fonction lance une musique
*/
void lancerMusique(char *file_name, int tempsEcoule) ;
/**
 * \fn void recevoirMusique(socket_t *client_socket,char * nomMusique)
 * \brief Fonction qui reçoit une musique
 * \param client_socket Socket du client
 * \param nomMusique Nom de la musique
 * \return void
 * \details Cette fonction reçoit une musique
*/
void recevoirMusique(socket_t *client_socket,char * nomMusique);

/**
 * \fn void client (char *addrIPsrv, short port)
 * \brief Fonction qui crée un client pour envoyer des demandes au serveur
 * \param addrIPsrv Adresse IP du serveur
 * \param port Port du serveur
 * \return void
 * \details Cette fonction crée un client pour envoyer des demandes au serveur
*/
void client (char *addrIPsrv, short port);

/**
 * \fn void signalHandler(int sig)
 * \brief Fonction qui gère les signaux
 * \param sig Signal
 * \return void
 * \details Cette fonction gère les signaux
*/
static void signalHandler(int sig);

/**
 * \var socket_t sockDial
 * \brief Socket pour la communication avec le serveur
*/
extern socket_t sockDial;

/**
 * \fn void waitForMusicToEnd()
 * \brief Fonction qui attend la fin de la musique
 * \return void
 * \details Cette fonction attend la fin de la musique
*/
void waitForMusicToEnd(libvlc_instance_t *vlcInstance, libvlc_media_player_t *mp);

/**
 * @brief 
 * 
 * @param sockDial 
 */
void closeSocket(socket_t* sockDial);