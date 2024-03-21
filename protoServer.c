#include "protoServer.h"


int *elapsedTime;
char *currentMusic;
int *isPlaying;
int  *isChoosing;
int shm_id, shm_size;
void *shm_ptr;


void server (char *addrIPsrv, short server_port){
    socket_t server_socket;
    pid_t pid;

    // on cree la memoire partagee
    CHECK(shm_id = shm_open("maZone", O_CREAT | O_RDWR, S_IRWXU), "shm_open error");

   // Calculate file size
    shm_size = sizeof(int) + sizeof(char) * MAX_BUFF + sizeof(int) + sizeof(int);

    // Set the size of the shared memory object
    CHECK(ftruncate(shm_id, shm_size) == 0, "ftruncate error");

    // Map the shared memory
    shm_ptr = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    CHECK_MAP(shm_ptr, "mmap error");


    // on initialise les variables
    elapsedTime = (int *)shm_ptr;
    currentMusic = (char *)(shm_ptr + sizeof(int));
    isPlaying = (int *)(shm_ptr + sizeof(int) + sizeof(char) * MAX_BUFF);
    isChoosing = (int *)(shm_ptr + sizeof(int) + sizeof(char) * MAX_BUFF + sizeof(int));


    // on initialise les variables
    *elapsedTime = 0;
    strcpy(currentMusic, "");
    *isPlaying = FALSE;
    *isChoosing = FALSE;

    signal(SIGINT, signalHandler);

    // Initialiser le serveur
    server_socket.type = SOCK_SERVEUR;
    server_socket.mode = SOCK_STREAM;
    ouvrirSocket(&server_socket, SOCK_STREAM, addrIPsrv, server_port);

    // on lance la radio en parallèle
     pid = fork();
    if (pid == -1) {
        perror("Erreur lors de la création du processus fils");
        exit(EXIT_FAILURE);
    } else
    if (pid == 0) {
        myRadio();
    }

    // on affiche les variables partagees dans un processus fils toutes les 10 secondes
    pid = fork();
    if (pid == -1) {
        perror("Erreur lors de la création du processus fils");
        exit(EXIT_FAILURE);
    } else
    if (pid == 0) {
        while (1) {
            printf("elapsedTime: %d\n", *elapsedTime);
            printf("currentMusic: %s\n", currentMusic);
            printf("isPlaying: %d\n", *isPlaying);
            printf("isChoosing: %d\n", *isChoosing);
            sleep(10);
        }
    }

    while (1) {
        // Attendre une connexion entrante
        socket_t client_socket = accepterConnection(&server_socket);

        // Dialogue avec le client
        pid_t pid = fork();
        if (pid == 0) {

            // Fermer le socket du serveur
            fermerSocket(&server_socket);
            
            while (1)
            {
                // Gérer la requête du client
                handle_client(&client_socket);
                
            }
        }
        else if (pid == -1) {
            perror("fork");
            exit(1);
        }
        else {
        // Fermer la connexion avec le client
        fermerSocket(&client_socket);
        }

    }

}


void handle_client(socket_t *client_socket) {
    buffer_t request;
    MusicMessage musicMessage;
    MusicMessage musicResponse;

    recevoir(client_socket, &musicMessage, (pFct) deserializeMusicMessage);

    // on fait un switch pour savoir quel commande a ete envoye par le client
    switch (musicMessage.type) {
        case SEND_MUSIC_REQUEST:
            printf("SEND_MUSIC_REQUEST received from client\n\n");
            // si currentMusic est vide, on envoie la playlist au client
            if (*isChoosing == FALSE && *isPlaying == FALSE) {
                sendPlaylist(client_socket);
            } else {
                // si le client est en train de choisir une musique, on attend qu'il ait fini
                if (*isChoosing == TRUE) {
                    while (*isChoosing == TRUE);
                }
                // sinon on envoie la musique courante au client
                sendCurrentMusic(client_socket);                
            }
            break;

        case SEND_MUSIC_CHOICE:
            printf("SEND_MUSIC_CHOICE received from client\n\n");

            printf("Received choice %s from client\n\n", musicMessage.current_music);
            // on met la musique choisie par le client dans currentMusic
            strcpy(currentMusic, musicMessage.current_music);
            envoyer(client_socket, "OK", NULL);

            break;

        case SEND_CURRENT_TIME_REQ:
            printf("SEND_CURRENT_TIME_REQ received from client\n\n");
            // on envoie elapsedTime au client
            sprintf(request, "%d", *elapsedTime);
            musicMessage.type = CURRENT_TIME_RETURN;
            musicMessage.current_time = *elapsedTime;
            envoyer(client_socket, &musicMessage, (pFct) serializeMusicMessage);
            
            break;

        case QUIT:
            printf("Client disconnected\n\n");
            // on ferme la connexion avec le client
            fermerSocket(client_socket);
            exit(0);
            break;
        
        default:
            // on envoie un message d'erreur au client
            envoyer(client_socket, "Commande inconnue", NULL);
            break;
    }
}


void sendCurrentMusic(socket_t *client_socket) {
    buffer_t buffer;
    char *file_name;
    int bytesRead;
    int i=0;
    MusicMessage bufferMusic;

    bufferMusic.type = MUSIC_RETURN;
    strcpy(bufferMusic.current_music, currentMusic);
    // on recupere le nom du fichier a telecharger et on ajoute le dossier dans lequel il se trouve devant

    printf("Sending %s to client...\n\n", currentMusic);

    envoyer(client_socket, &bufferMusic, (pFct) serializeMusicMessage);

    recevoir(client_socket, buffer, NULL);
    if (strcmp(buffer, "OK") != 0) {
        exit(EXIT_FAILURE);
    }

    file_name = malloc(MAX_BUFF);
    strcpy(file_name, "playlist/");
    strcat(file_name, currentMusic);

    FILE *file = fopen(file_name, "rb");

    CHECK_FILE(file, "Error opening file");
    // Envoyer le contenu du fichier mp3 au client
    while ((bytesRead = fread(buffer, 1, MAX_BUFF, file)) > 0) {
        envoyer(client_socket, buffer, NULL);
    }
    
    envoyer(client_socket, EXIT, NULL);
    fclose(file);
}



void sendPlaylist(socket_t *client_socket) {
    MusicMessage musicMessage;

    // on recupere le nom des musiques dans le fichier playlist.txt et on les met dans musicMessage.playlist
    char *line = malloc(MAX_BUFF);
    size_t len = 0;
    ssize_t read;
    int i=0;

    FILE *file = fopen("playlist.txt", "r");

    CHECK_FILE(file, "Error opening file");

    while ((read = getline(&line, &len, file)) != -1) {

        // sara_perche_ti_amo.mp3;3:12
        // on recupere le nom de la musique
        char *token = strtok(line, ";");
        strcpy(musicMessage.playlist[i], token);
        i++;
    }

    musicMessage.type = PLAYLIST_RETURN;

    fclose(file);
    if (line)
        free(line);
    
    // on met isChoosing a TRUE pour dire que le client est en train de choisir une musique
    *isChoosing = TRUE;


    printf("Sending playlist to client...\n");
    // on envoie musicMessage au client
    envoyer(client_socket, &musicMessage,(pFct) serializeMusicMessage);
}


void myRadio(){
    char *line = malloc(MAX_BUFF);
    size_t len = 0;
    ssize_t read;
    char * token = malloc(MAX_BUFF);
    char *token2 = malloc(MAX_BUFF);
    char *file_name = malloc(MAX_BUFF);
    char *minutes = malloc(MAX_BUFF);
    char *seconds = malloc(MAX_BUFF);
    int totalSeconds;
    FILE *file = fopen("playlist.txt", "r");

    CHECK_FILE(file, "Error opening file");

    while (strlen(currentMusic) == 0);

    // on lit le fichier ligne par ligne a partir de la musique courante et on commence a jouer la musique a partir de la musique courante
    while ((read = getline(&line, &len, file)) != -1) {
        *elapsedTime=0;
        
        // on recupere le nom du fichier et la durée
        token = strtok(line, ";");
        file_name = token;
        token = strtok(NULL, ";");   

        // si on doit retrouver la musique choisie par le client, on continue jusqu'a ce qu'on la trouve
        if (strcmp(file_name, currentMusic) != 0) {
            // on verifie que le client est en train de choisir une musique
            if (*isPlaying == FALSE) 
                continue;
        }     
        
        // on formate la durée pour pouvoir l'utiliser dans sleep
        minutes = strtok(token, ":");
        seconds = strtok(NULL, ":");
        totalSeconds = atoi(minutes)*60 + atoi(seconds);


        *isChoosing = FALSE;
        *isPlaying = TRUE;
        sleep(2);
        printf("Playing %s for %d seconds\n\n", file_name, totalSeconds);
        // on attend la durée de la musique et on incremente tempsEcoule de 1 tout les milliseconds sachant que totalSeconds est en secondes
        for (int i=0; i<totalSeconds*1000; i++) {
            usleep(1000);
            *elapsedTime += 1;
        }
    }

    CHECK(munmap(shm_ptr, shm_size) == 0, "munmap error");
    CHECK(close(shm_id) == 0, "close error");
    CHECK(shm_unlink("maZone"), "shm_unlink error");
    fclose(file);
    if (line)
        free(line);

    exit(EXIT_SUCCESS);    
}


static void signalHandler(int sig) {
    switch (sig) {
        case SIGINT:
            // Fermer le fichier et terminer le programme lorsque CTRL+C est détecté
            printf("\nFermeture du serveur...\n");
            CHECK(munmap(shm_ptr, shm_size) == 0, "munmap error");
            CHECK(close(shm_id) == 0, "close error"); 
            exit(EXIT_SUCCESS);
            break;

        default:
            printf("Signal inconnu\n");
            break;
    }
}
