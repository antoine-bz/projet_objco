#include "protoClient.h"

socket_t sockDial;  
int *mute;
int shm_id, shm_size;
libvlc_instance_t *vlcInstance;
libvlc_media_player_t *mp;



void client(char *addrIPsrv, short port) {
    char reponse[MAX_BUFF];
    char musicName[MAX_BUFF];
    MusicMessage buffer = {0}; // Initialize buffer to zero
    int choix, tailleTableau;
    int client_connected = 1; // Variable to control the loop, based on client connection status.
    char current_music_str[10];
    sprintf(current_music_str, "%d", buffer.current_music);
    CHECK(shm_id = shm_open("maZoneClient", O_CREAT | O_RDWR, S_IRWXU), "shm_open error");
    // Calculate file size
    shm_size = sizeof(int);

    // Set the size of the shared memory object
    CHECK(ftruncate(shm_id, shm_size) == 0, "ftruncate error");

    // Map the shared memory
    mute = (int *) mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    CHECK_MAP(mute, "mmap error");

    // Demande d’une connexion au service
    sockDial.mode = SOCK_STREAM;
    sockDial.sock = connecter(addrIPsrv, port);
    sockDial.type = SOCK_CLIENT;
    ouvrirSocket(&sockDial, SOCK_STREAM, addrIPsrv, port);

    while(client_connected) {
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

            buffer.current_music =choix;
            buffer.type = SEND_MUSIC_CHOICE;

            envoyer(&sockDial, &buffer, (pFct) serializeMusicMessage);
            recevoir(&sockDial, &reponse, NULL);

            if (strcmp(reponse, "OK") != 0) {
                printf("Erreur de reception du choix\n");
                exit(EXIT_FAILURE);
            }

            if (buffer.type == MUSIC_RETURN) {
                strcpy(musicName, "current_");
                //strcat(musicName, buffer.current_music);
                strcat(musicName, current_music_str);
            streamAudioClient(addrIPsrv); // Use the IP to stream audio
            } else {
                printf("Erreur de reception de la musique\n");
                exit(EXIT_FAILURE);
            }

            strcpy(musicName, "current_");
            //strcat(musicName, buffer.current_music);
            strcat(musicName, current_music_str);
            streamAudioClient(addrIPsrv);
            vlcInstance = libvlc_new(0, NULL);
            mp = libvlc_media_player_new(vlcInstance);

            waitForMusicToEnd(vlcInstance, mp);

            printf("Voulez-vous choisir une autre musique (1 pour oui, %d pour non) ?\n", DISCONNECT_CHOICE);
            scanf("%d", &choix);
            if (choix == DISCONNECT_CHOICE) {
                client_connected = 0; // User chose to not continue
            }
        } else {
            printf("Erreur de reception de la playlist\n");
            exit(EXIT_FAILURE);
        }
    }

    closeSocket(&sockDial); // Implement this function as needed
}

static void signalHandler(int sig) {
    switch (sig) {
        case SIGINT:
            printf("Signal SIGINT reçu\n");
            
            // Vider le tampon d'entrée stdin
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            // Demander à l'utilisateur s'il veut quitter
            printf("Voulez-vous quitter ? (y/n) ou m pour mute\n");
            char choice = getchar();
            if (choice == 'y') {
                fermerSocket(&sockDial);
                CHECK(munmap(mute, shm_size) == 0, "munmap error");
                CHECK(close(shm_id) == 0, "close error"); 
                exit(EXIT_SUCCESS);
            } else if (choice == 'm') {
                if ((*mute) == FALSE)
                    *mute = TRUE;
                else
                    *mute = FALSE;
            }
            break;

        default:
            printf("Signal inconnu\n");
            break;
    }
}

void waitForMusicToEnd(libvlc_instance_t *vlcInstance, libvlc_media_player_t *mp) {
    const int pollIntervalMs = 100;
    while (libvlc_media_player_is_playing(mp)) {
        usleep(pollIntervalMs * 1000); // Convert milliseconds to microseconds
    }
    printf("Playback finished\n");
    // Cleanup should be handled here as well
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(vlcInstance);
}

