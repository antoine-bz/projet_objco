#include "data.h"


void envoyer(socket_t *sock, generic quoi, pFct serial)
{
    buffer_t buff;
    if (sock->mode == SOCK_STREAM)
    {
        if (serial == NULL){
            ecrire(sock->sock, (char *) quoi, MAX_BUFF);
        }
        else
        {
            serial(quoi, buff);
            ecrire(sock->sock, buff, MAX_BUFF);
        }
    }
    else if (sock->mode == SOCK_DGRAM){
        //blasvallavlaba   
    }
    
}

int recevoir(socket_t *sock, generic quoi, pFct deSerial){
    buffer_t buff;
    if (sock->mode == SOCK_STREAM)
    {
        if (deSerial == NULL){
            lire(sock->sock, (char *) quoi, MAX_BUFF);
        }
        else
        {
            lire(sock->sock, buff, MAX_BUFF);
            deSerial(buff, quoi);
        }
    }
    else if (sock->mode == SOCK_DGRAM){
        //blasvallavlaba   
    }
    return strlen(buff);
}



void fermerSocket(socket_t * sock){
    fermer(sock->sock);
}


void ouvrirSocket(socket_t * sock, short mode, char * addrIP, short port){
    sock->mode = mode;
    if(mode == SOCK_STREAM){
        if (sock->type == SOCK_CLIENT){
            sock->sock = connecter(addrIP, port);
        }
        else{
            sock->sock = creerSocketEcoute(addrIP, port, 5);
        }
    }
    else{
        if (sock->type == SOCK_CLIENT){
            
        
        }
        else{
        
        }
    }
}

socket_t accepterConnection(socket_t * sock){
    socket_t sockDial;
    if (sock->mode == SOCK_STREAM){
        sockDial.sock = accepter(sock->sock);
        sockDial.mode = SOCK_STREAM;
        sockDial.type = SOCK_CLIENT;
    }
    return sockDial;
}