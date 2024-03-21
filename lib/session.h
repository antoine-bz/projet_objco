/**
 * \file    session.h
 * \brief   fichier d'entête de session.c
 * \author  Boez Antoine
*/

/***********************/
/*      INCLUDES       */
/***********************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * \def     CHECK(sts,msg)
 * \brief   macro de vérification de l'état d'une fonction
 * \param   sts : fournit le résultat de la fonction
 * \param   msg : fournit le message d'erreur à afficher
*/
#define CHECK(sts,msg) if ((sts) == -1) {perror(msg);exit(-1);}


/************************************************************/
/*                                                          */
/*              P  r  o  t  o  t  y  p  e  s                */
/*                                                          */
/************************************************************/


/**
 * \fn      int creerSocket(int mode);
 * \brief   fonction de création d'une socket dans un mode donné
 * \param   mode : fournit le mode de la socket à créer (SOCK_DGRAM/ SOCK_STREAM)
 * \result  numéro de fd de la socket créée 
 */
int creerSocket(int mode);


/**
 * \fn      struct sockaddr_in creerAddr(char *addrIP,short port);
 * \brief   fonction de création d'une structure sockaddr_in
 * \param   addrIP : fournit l'adresse IP de la socket à créer
 * \param   port : fournit le port de la socket à créer
 * \result  structure sockaddr_in créée
*/
struct sockaddr_in creerAddr(char *addrIP,short port);

/**
 * \fn      int creerSocketAddr(int mode, char * addrIP,short port);
 * \brief   fonction de création d'une socket dans un mode donné
 * \param   mode : fournit le mode de la socket à créer (SOCK_DGRAM/ SOCK_STREAM)
 * \param   addrIP : fournit l'adresse IP de la socket à créer
 * \param   port : fournit le port de la socket à créer
 * \result  numéro de fd de la socket créée
*/
int creerSocketAddr(int mode, char * addrIP,short port);

/**
 * \fn      int creerSocketEcoute(char * addrIP,short port, short maxClts);
 * \brief   fonction de création d'une socket en mode écoute
 * \param   addrIP : fournit l'adresse IP de la socket à créer
 * \param   port : fournit le port de la socket à créer
 * \param   maxClts : fournit le nombre maximum de clients à accepter
 * \result  numéro de fd de la socket créée
*/
int creerSocketEcoute(char * addrIP,short port, short maxClts);

/**
 * \fn      int connecter(char *addrIPsrv, short port);
 * \brief   fonction de connexion à un service
 * \param   addrIPsrv : fournit l'adresse IP du service à contacter
 * \param   port : fournit le port du service à contacter
 * \result  numéro de fd de la socket créée
*/
int connecter(char *addrIPsrv, short port);

/**
 * \fn      int accepter(int sockEcoute);
 * \brief   fonction d'acceptation d'une connexion
 * \param   sockEcoute : fournit le numéro de fd de la socket d'écoute
 * \result  numéro de fd de la socket créée
*/
int accepter(int sockEcoute);

/**
 * \fn      int ecrire(int sock, char *msg, int taille);
 * \brief   fonction d'écriture d'un message sur une socket
 * \param   sock : fournit le numéro de fd de la socket
 * \param   msg : fournit le message à écrire
 * \param   taille : fournit la taille du message à écrire
 * \result  nombre d'octets écrits
*/
int ecrire(int sock, char *msg, int taille);

/**
 * \fn      int lire(int sock, char *msg, int taille);
 * \brief   fonction de lecture d'un message sur une socket
 * \param   sock : fournit le numéro de fd de la socket
 * \param   msg : fournit le message à lire
 * \param   taille : fournit la taille du message à lire
 * \result  nombre d'octets lus
*/
int lire(int sock, char *msg, int taille);

/**
 * \fn      void fermer(int sock);
 * \brief   fonction de fermeture d'une socket
 * \param   sock : fournit le numéro de fd de la socket
*/
void fermer(int sock);