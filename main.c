#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#ifdef CLIENT
    
#include "./lib/protoClient.h"

#endif
#ifdef SERVEUR

#include "./lib/protoServer.h"

#endif

// Définition des constantes
#define PORT_SVC 5000
#define INADDR_SVC "0.0.0.0"


int main(int argc, char *argv[]) {
    // gcc useLiNet.c -DCLIENT -o client
    #ifdef CLIENT
        
        // Vérification des arguments
        if(argc != 3) {
                printf("Usage : %s <IP> <PORT>\n", argv[0]);
                exit(-1);
        }
        client(argv[1], atoi(argv[2]));
    #endif
    // gcc useLiNet.c -DSERVEUR -o serveur
    #ifdef SERVEUR
        // Vérification des arguments
        if(argc == 3) {
            server(argv[1], atoi(argv[2]));
        }
        else if (argc == 2)
        {
            server(INADDR_SVC, PORT_SVC+atoi(argv[1]));
        }
        else if (argc == 1)
        {
            server(INADDR_SVC, PORT_SVC);
        }
        else{
            printf("Usage : %s <IP> <PORT>\n", argv[0]);
            exit(-1);
        }
        
    #endif
}