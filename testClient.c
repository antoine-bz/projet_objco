#include "./lib/reqRep.h"
#include "./lib/audioStream.h"

#define PORT_SVC 5000
#define INADDR_SVC "127.0.0.1"

int main(){/*
    char reponse[MAX_BUFF];
    char envoie[MAX_BUFF];
    char addrIPsrv[MAX_BUFF];
    short port;
    pid_t pid;

    socket_t sockDial;
    socket_t sockEcoute;

    strcpy(addrIPsrv, INADDR_SVC);
    port = PORT_SVC;

    // Demande d’une connexion au service
    sockDial.mode = SOCK_STREAM;
    sockDial.sock = connecter(addrIPsrv, port);
    sockDial.type = SOCK_CLIENT;
    ouvrirSocket(&sockDial, SOCK_STREAM, addrIPsrv, port);


   while(1){

        MusicMessage message;
        printf("Entrez le type de message: ");
        scanf("%d", &message.type);
        printf("Entrez le nom de la chanson courante: ");
        scanf("%d", &message.current_music);
        printf("Entrez la taille de la playlist: ");
        scanf("%d", &message.playlist_size);
        printf("Entrez les noms de chansons de la playlist\n");
        if (message.playlist_size > MAX_BUFF){
            printf("La taille de la playlist est trop grande\n");
            return 1;
        }
        if (message.playlist_size < 0){
            printf("La taille de la playlist est trop petite\n");
            return 1;
        }
        if (message.playlist_size != 0)
        for(int i = 0; i < message.playlist_size; i++){
            printf("Chanson %d: ", i);
            scanf("%s", message.playlist[i]);
        }

        envoyer(&sockDial, &message, (pFct) serializeMusicMessage);
        recevoir(&sockDial, &message, (pFct) deserializeMusicMessage);
    
   }*/

    // test de audiostream

    streamAudioClient("10.42.0.19");

}