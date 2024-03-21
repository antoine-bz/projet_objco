/**
 * \file    data.h
 * \brief   fichier d'entête de data.c
 * \author  Boez Antoine
*/

/***********************/
/*      INCLUDES       */
/***********************/
#include "session.h"

#define MAX_BUFF 1024
#define SOCK_CLIENT 0
#define SOCK_SERVEUR 1

/**
 * \def     CHECK(sts,msg)
 * \brief   macro de vérification de l'état d'une fonction
 * \param   sts : fournit le résultat de la fonction
 * \param   msg : fournit le message d'erreur à afficher
*/
#define CHECK(sts,msg) if ((sts) == -1) {perror(msg);exit(-1);}

/**
 * \def     CHECK_FILE(sts,msg)
 * \brief   macro de vérification de l'état d'un fichier
 * \param   sts : fournit le résultat de la fonction
 * \param   msg : fournit le message d'erreur à afficher
*/
#define CHECK_FILE(sts,msg) if ((sts) == NULL) {perror(msg);exit(-1);}

/**
 * \def     CHECK_SHM(sts,msg)
 * \brief   macro de vérification de l'état d'une mémoire partagée
 * \param   sts : fournit le résultat de la fonction
 * \param   msg : fournit le message d'erreur à afficher
*/
#define CHECK_SHM(sts,msg) if ((sts) == (void *) -1) {perror(msg);exit(-1);}

/**
 * \def     CHECK_MAP(sts,msg)
 * \brief   macro de vérification de l'état d'un mapping
 * \param   sts : fournit le résultat de la fonction
 * \param   msg : fournit le message d'erreur à afficher
*/
#define CHECK_MAP(sts,msg) if ((sts) == MAP_FAILED) {perror(msg);exit(-1);}

/**
 * \typedef char buffer_t[MAX_BUFF];
 * \brief   buffer de taille MAX_BUFF
*/
typedef char buffer_t[MAX_BUFF];


/**
 * \typedef void * generic;
 * \brief   pointeur générique
*/
typedef void * generic;


/**
 * \typedef void (*pFct)(generic, generic);
 * \brief   fonction de pointeur sur fonction
*/
typedef void (*pFct)(generic, generic);


/**
 * \struct  socket_t *
 * \brief   structure de socket
 * \param   sock : fournit le numéro de fd de la socket
 * \param   mode : fournit le mode de la socket (SOCK_DGRAM/ SOCK_STREAM)
 * \param   type : fournit le type de la socket (SOCK_CLIENT/ SOCK_SERVEUR)
*/
typedef struct socket_t{
    int sock;
    short mode; //SOCK_DGRAM/ SOCK_STREAM
    short type; //SOCK_CLIENT/ SOCK_SERVEUR
} socket_t;


/**
 * \fn      void envoyer(socket_t * sock, generic quoi, pFct serial);
 * \brief   fonction d'envoi d'un message sur une socket
 * \param   sock : fournit la socket sur laquelle envoyer le message
 * \param   quoi : fournit le message à envoyer
 * \param   serial : fournit la fonction de sérialisation du message
 * \result  void
*/
void envoyer(socket_t * sock, generic quoi, pFct serial);

/**
 * \fn      void recevoir(socket_t * sock, generic quoi, pFct deSerial);
 * \brief   fonction de réception d'un message sur une socket
 * \param   sock : fournit la socket sur laquelle recevoir le message
 * \param   quoi : fournit le message à recevoir
 * \param   deSerial : fournit la fonction de désérialisation du message
 * \result  int : fournit la taille du message reçu
*/
int recevoir(socket_t * sock, generic quoi, pFct deSerial);


/**
 * \fn      void fermerSocket(socket_t * sock);
 * \brief   fonction de fermeture d'une socket
 * \param   sock : fournit la socket à fermer
 * \result  void
*/
void fermerSocket(socket_t * sock);

/**
 * \fn     void ouvrirSocket(socket_t * sock, short mode, char * addrIP, short port);
 * \brief  fonction d'ouverture d'une socket
 * \param  sock : fournit la socket à ouvrir
 * \param  mode : fournit le mode de la socket (SOCK_DGRAM/ SOCK_STREAM)
 * \param  addrIP : fournit l'adresse IP de la socket
 * \param  port : fournit le port de la socket
*/
void ouvrirSocket(socket_t * sock, short mode, char * addrIP, short port);

/**
 * \fn      socket_t accepterConnection(socket_t * sock);
 * \brief   fonction d'acceptation d'une connexion sur une socket
 * \param   sock : fournit la socket sur laquelle accepter la connexion
 * \result  socket_t : fournit la socket de la connexion acceptée
*/
socket_t accepterConnection(socket_t * sock);
