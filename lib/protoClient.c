#include "protoClient.h"

socket_t sockDial;
int mute;  // Local mute variable

void client(char *addrIPsrv, short port) {
    char reponse[MAX_BUFF];
    char musicName[MAX_BUFF];
    MusicMessage buffer = {0};  // Initialize buffer to zero
    int choix, tailleTableau;
    int client_connected = 1;  // Variable to control the loop, based on client connection status.
    char current_music_str[10];

    // Demande d’une connexion au service
    sockDial.mode = SOCK_STREAM;
    sockDial.sock = connecter(addrIPsrv, port);
    sockDial.type = SOCK_CLIENT;
    ouvrirSocket(&sockDial, SOCK_STREAM, addrIPsrv, port);

    while (client_connected) {
        buffer.type = SEND_MUSIC_REQUEST;
        envoyer(&sockDial, &buffer, (pFct) serializeMusicMessage);
        recevoir(&sockDial, &buffer, (pFct) deserializeMusicMessage);

        if (buffer.type == PLAYLIST_RETURN) {
            printf("\nPlaylist:\n");
            tailleTableau = sizeof(buffer.playlist) / sizeof(buffer.playlist[0]);
            for (int i = 0; i < tailleTableau; i++) {
                if (strlen(buffer.playlist[i]) > 0) {
                    printf("%d - %s\n", i, buffer.playlist[i]);
                }
            }
            printf("\nChoisir une musique (%d pour quitter):\n", DISCONNECT_CHOICE);
            scanf("%d", &choix);

            if (choix == DISCONNECT_CHOICE) {
                client_connected = 0;
                break;
            }

            buffer.current_music = choix;
            buffer.type = SEND_MUSIC_CHOICE;

            envoyer(&sockDial, &buffer, (pFct) serializeMusicMessage);
            recevoir(&sockDial, &reponse, NULL);


            sprintf(current_music_str, "%d", buffer.current_music);
            strcpy(musicName, "current_");
            strcat(musicName, current_music_str);
            streamAudioClient(addrIPsrv);  // Use the IP to stream audio
            if (strcmp(reponse, "OK") != 0) {
                printf("Erreur de reception du choiiiiiiix\n");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Erreur de reception de la playlist\n");
            exit(EXIT_FAILURE);
        }

        printf("Voulez-vous choisir une autre musique (1 pour oui, %d pour non) ?\n", DISCONNECT_CHOICE);
        scanf("%d", &choix);
        if (choix == DISCONNECT_CHOICE) {
            client_connected = 0;  // User chose to not continue
        }
    }

    closeSocket(&sockDial);  // Implement this function as needed
}

void closeSocket(socket_t* sockDial) {
    close(sockDial->sock);
}

// The following functions need to be defined according to your application:
// - connecter
// - ouvrirSocket
// - envoyer
// - recevoir
// - serializeMusicMessage
// - deserializeMusicMessage
// - streamAudioClient
// - waitForMusicToEnd

// If you have signal handlers or other functionality, make sure to include those too.
