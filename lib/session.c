#include "session.h"



int creerSocket(int mode){
    int sock;                 /* variable qui va contenir le descripteur de socket */
    struct sockaddr_in addr;
    // Création de la socket 
    CHECK(sock=socket(PF_INET, mode, 0), "__SOCKET__");
    return sock;    
}


struct sockaddr_in creerAddr(char *addrIP,short port){

    struct sockaddr_in addr;

    // Préparation de l’adressage du service à contacter
    addr.sin_family = PF_INET;
    addr.sin_port = htons (port);
    addr.sin_addr.s_addr = inet_addr(addrIP);
    memset(&addr.sin_zero, 0, 8);

    return addr;
}


int creerSocketAddr(int mode, char * addrIP,short port){
    int sock = creerSocket(mode);    
    struct sockaddr_in addr = creerAddr(addrIP, port);

    // Association de la socket avec l’adressage preparé avec la socket créée
    CHECK(bind(sock, (struct sockaddr *) &addr, sizeof addr) , "__BIND__");
    return sock;
}

int creerSocketEcoute(char * addrIP,short port, short maxClts){
    int sock = creerSocketAddr(SOCK_STREAM, addrIP, port);
    // Mise en mode écoute de la socket
    CHECK(listen(sock, maxClts), "__LISTEN__");
    return sock;
}

int connecter(char *addrIPsrv, short port){
    int sock = creerSocket(SOCK_STREAM);
    struct sockaddr_in addrSrv = creerAddr(addrIPsrv, port);
    struct sockaddr_in monAddr;
    int lenAddr = sizeof monAddr;
    
    // Demande d’une connexion au service
    CHECK(connect(sock, (struct sockaddr *)&addrSrv, sizeof addrSrv) , "__CONNECT__");

    CHECK(getsockname(sock, (struct sockaddr *)&monAddr, &lenAddr) , "__GETSOCKNAME__");
    fprintf(stderr, "L'adresse IP clent=[%s], Port client=[%d]\n", inet_ntoa(monAddr.sin_addr), ntohs(monAddr.sin_port));
    return sock;
}

int accepter(int sockEcoute){
    struct sockaddr_in clt;
    socklen_t cltLen = sizeof(clt);
    int sock = accept(sockEcoute, (struct sockaddr *)&clt, &cltLen);
    CHECK(sock, "__ACCEPT__");
    fprintf(stderr, "L'adresse IP client=[%s], Port client=[%d]\n", inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));
    return sock;
}

int ecrire(int sock, char *msg, int taille){
    int nbOctets = write(sock, msg, taille);
    CHECK(nbOctets, "__WRITE__");
    return nbOctets;
}

int lire(int sock, char *msg, int taille){
    int nbOctets = read(sock, msg, taille);
    CHECK(nbOctets, "__READ__");
    return nbOctets;
}

void fermer(int sock){
    CHECK(close(sock), "__CLOSE__");
}