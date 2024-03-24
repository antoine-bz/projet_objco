/**
 * @file reqRep.h
 * @brief Fichier d'entête de reqRep.c
 * @version 0.1
*/
#include "data.h"

#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

/**
 * \def CHECK(sts,msg)
 * \brief Vérifier si une fonction a réussi
 * \param sts Statut
 * \param msg Message
 * \def CHECK_FILE(sts,msg)
 * \brief Vérifier si un fichier a été ouvert
 * \param sts Statut
 * \param msg Message
 * \def CHECK_SHM(sts,msg)
 * \brief Vérifier si la mémoire partagée a été créée
 * \param sts Statut
 * \param msg Message
*/
#define CHECK(sts,msg) if ((sts) == -1) {perror(msg);exit(-1);}
#define CHECK_FILE(sts,msg) if ((sts) == NULL) {perror(msg);exit(-1);}
#define CHECK_SHM(sts,msg) if ((sts) == (void *) -1) {perror(msg);exit(-1);}


/**
 * \def MAX_BUFF
 * \brief Taille maximale du buffer
 * \def EXIT
 * \brief Message pour quitter
 * \def TRUE
 * \brief Vrai
 * \def FALSE
 * \brief Faux
*/
#define MAX_BUFF 1024
#define EXIT "exit"
#define TRUE 1
#define FALSE 0

typedef char buffer_t[MAX_BUFF];

// Définir des constantes pour les types de messages
/**
 * \def PLAYLIST_RETURN
 * \brief Type de message pour retourner la playlist
 * \def SEND_MUSIC_CHOICE
 * \brief Type de message pour envoyer le choix de musique
 * \def SEND_MUSIC_REQUEST
 * \brief Type de message pour envoyer la demande de musique
 * \def MUSIC_RETURN
 * \brief Type de message pour retourner la musique
 * \def SEND_CURRENT_TIME_REQ
 * \brief Type de message pour envoyer la demande de temps courant
 * \def CURRENT_TIME_RETURN
 * \brief Type de message pour retourner le temps courant
 * \def QUIT
 * \brief Type de message pour quitter
 * \details Ces constantes sont utilisées pour les types de messages
*/
#define PLAYLIST_RETURN       1
#define SEND_MUSIC_CHOICE     2
#define SEND_MUSIC_REQUEST    3
#define MUSIC_RETURN          4
#define REQUEST_PLAYLIST      7
#define QUIT                  8


/**
 * \struct MusicMessage
 * \brief Structure pour les messages de musique
 * \param type Type de message
 * \param current_music Nom de la chanson courante
 * \param playlist_size Taille de la playlist
 * \param playlist Nom de la chanson courante
 * \details Cette structure est utilisée pour les messages de musique entre le client et le serveur
*/
typedef struct MusicMessage {
    int type; // type de message
    char current_music[MAX_BUFF]; // nom de la chanson courante
    int playlist_size; // taille de la playlist
    char playlist[MAX_BUFF][MAX_BUFF]; // nom de la chanson courante
} MusicMessage;


/**
 * \fn void serializeMusicMessage(generic quoi, buffer_t buff)
 * \brief Fonction qui sérialise un message de musique
 * \param quoi Message de musique
 * \param buff Buffer
 * \return void
 * \details Cette fonction sérialise un message de musique
*/
void serializeMusicMessage(generic quoi, buffer_t buff);

/**
 * \fn void deserializeMusicMessage(buffer_t buff, generic quoi)
 * \brief Fonction qui désérialise un message de musique
 * \param buff Buffer
 * \param quoi Message de musique
 * \return void
 * \details Cette fonction désérialise un message de musique
*/
void deserializeMusicMessage(buffer_t buff, generic quoi);
