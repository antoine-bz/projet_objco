#include "protoClient.h"

socket_t sockDial;
int mute; // Local mute variable

void client(char *addrIPsrv, short port) {
    char reponse[MAX_BUFF];
    MusicMessage buffer = {0};
    int choix;
    int client_connected = 1;

    // Initialization of the connection to the service
    sockDial.mode = SOCK_STREAM;
    sockDial.sock = connecter(addrIPsrv, port);
    sockDial.type = SOCK_CLIENT;
    ouvrirSocket(&sockDial, SOCK_STREAM, addrIPsrv, port);

    while (client_connected) {
        // Request the playlist from the server
        buffer.type = SEND_MUSIC_REQUEST;
        envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);
        recevoir(&sockDial, &buffer, (pFct)deserializeMusicMessage);

        if (buffer.type == PLAYLIST_RETURN) {
            // Display playlist and get user choice
            printf("\nPlaylist:\n");
            for (int i = 0; i < buffer.playlist_size; i++) {
                printf("%d - %s\n", i + 1, buffer.playlist[i]);
            }
            printf("\nChoose a music (0 to quit): ");
            scanf("%d", &choix);
            if (choix == 0) {
                client_connected = 0;
                break;
            }

            // Send the music choice to the server
            buffer.current_music = choix - 1; // Adjust the choice for a 0-based index
            buffer.type = SEND_MUSIC_CHOICE;
            envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);
            recevoir(&sockDial, &reponse, NULL); // Wait for the server's OK

            // Play the chosen music
            streamAudioClient(addrIPsrv); // This function should block until the end of the music
            printf("Playback finished. Would you like to choose another song? (1 for yes, 0 for no)\n");
            scanf("%d", &choix);
            if (choix == 0) {
                client_connected = 0; // If the user does not want to continue, end the loop
            }
            // No need to reset the buffer here since it will be set again at the beginning of the loop
        } else {
            // Handle errors
            printf("Error receiving data from the server.\n");
            break;
        }
    }

    // Clean up and close the socket when done
    printf("Client disconnecting...\n");
    closeSocket(&sockDial);
}

void closeSocket(socket_t* sockDial) {
    close(sockDial->sock);
}
