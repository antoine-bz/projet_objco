#include "protoServer.h"


int *currentMusic;
int *isPlaying;
int  *isChoosing;
int *musicPid;

int shm_id, shm_size;
void *shm_ptr;




void server (char *addrIPsrv, short server_port){
    socket_t server_socket;
    pid_t pid;

    // on cree la memoire partagee
    CHECK(shm_id = shm_open("maZone", O_CREAT | O_RDWR, S_IRWXU), "shm_open error");

   // Calculate file size
    shm_size = sizeof(char) * MAX_BUFF + 3*sizeof(int);

    // Set the size of the shared memory object
    CHECK(ftruncate(shm_id, shm_size) == 0, "ftruncate error");

    // Map the shared memory
    shm_ptr = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    CHECK_MAP(shm_ptr, "mmap error");


    // on initialise les variables
    currentMusic = (int *)(shm_ptr + sizeof(int));
    isPlaying = (int *)(shm_ptr + sizeof(int) + sizeof(char) * MAX_BUFF);
    isChoosing = (int *)(shm_ptr + sizeof(int) + sizeof(char) * MAX_BUFF + sizeof(int));
    musicPid = (int *)(shm_ptr + sizeof(int) + sizeof(char) * MAX_BUFF + sizeof(int));


    // on initialise les variables
    *currentMusic = 0;
    *isPlaying = FALSE;
    *isChoosing = FALSE;
    *musicPid = 0;

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
            printf("currentMusic: %d\n", *currentMusic);
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
            MusicMessage musicMessage2;
            musicMessage2 = retrievePlaylist();
            *currentMusic = findMusic(musicMessage2, musicMessage.current_music);
            envoyer(client_socket, "OK", NULL);

            break;

        case REQUEST_PLAYLIST:
            printf("REQUEST_PLAYLIST received from client\n\n");
            kill (*musicPid, SIGKILL);
            sendPlaylist(client_socket);
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
    int bytesRead;
    int i=0;
    MusicMessage bufferMusic;

    bufferMusic = retrievePlaylist();
    strcpy(bufferMusic.current_music, bufferMusic.playlist[*currentMusic]);

    bufferMusic.type = MUSIC_RETURN;
    
    // on recupere le nom du fichier a telecharger et on ajoute le dossier dans lequel il se trouve devant

    printf("Sending %d to client...\n\n", *currentMusic);

    envoyer(client_socket, &bufferMusic, (pFct) serializeMusicMessage);

    recevoir(client_socket, buffer, NULL);
    if (strcmp(buffer, "OK") != 0) {
        exit(EXIT_FAILURE);
    }
}


void sendPlaylist(socket_t *client_socket) {
    MusicMessage musicMessage;
    *isChoosing = TRUE;

    // on recupere la playlist
    musicMessage = retrievePlaylist();

    musicMessage.type = PLAYLIST_RETURN; // Assuming PLAYLIST_RETURN is defined elsewhere
    // You should set other fields of musicMessage as needed

    printf("Sending playlist to client...\n");
    // Send musicMessage to the client
    envoyer(client_socket, &musicMessage, (pFct)serializeMusicMessage);

    // on attend que le client ait choisi une musique
    recevoir(client_socket, &musicMessage, (pFct)deserializeMusicMessage);

    // on met la musique choisie par le client dans currentMusic
    for (int i = 0; i < musicMessage.playlist_size; i++) {
        if (strcmp(musicMessage.playlist[i], musicMessage.current_music) == 0) {
            *currentMusic = i;
            break;
        }
    }

    *isChoosing = FALSE;

}


void myRadio(){
    MusicMessage musicMessage; 
    int i = 0;
    pid_t musicPlayPid, buttonPid;

    // on recupere la playlist
    musicMessage = retrievePlaylist();


    while (1) {
        // on attend que le client ait choisi une musique
        while (*isChoosing == TRUE);


        // on lance la musique
        musicPlayPid = fork();
        if (musicPlayPid == -1) {
            perror("Erreur lors de la création du processus fils");
            exit(EXIT_FAILURE);
        } else
        if (musicPlayPid == 0) {
            *musicPid = getpid();

            *isPlaying = TRUE;
            // Construire le chemin complet du fichier
            char path[MAX_BUFF];
            snprintf(path, sizeof(path)+9, "playlist/%s", musicMessage.playlist[i]);
            printf("path: %s\n", path);

            //streamAudioServer(path); // Jouer le fichier audio
            exit(EXIT_SUCCESS);
        }

        buttonPid = fork();
        if (buttonPid == -1) {
            perror("Erreur lors de la création du processus fils");
            exit(EXIT_FAILURE);
        } else
        if (buttonPid == 0) {
            buttonHandler(*musicPid);
            exit(EXIT_SUCCESS);
        }


        waitpid(musicPlayPid, NULL, 0);

        // on passe à la musique suivante
        isPlaying = FALSE;
        i++;
        if (i == musicMessage.playlist_size) {
            i = 0;
        }

    }
    
    // on remet currentMusic à vide
    *currentMusic = 0;
    *isPlaying = FALSE;
    *isChoosing = FALSE;


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


int buttonHandler(pid_t pid)
{
	/*
	wiringPiSetup () ;
	pinMode (PIN_IN_GAUCHE, INPUT) ;
    pinMode (PIN_IN_DROITE, INPUT) ;
    pinMode (PIN_IN_HAUT, INPUT) ;
    pinMode (PIN_IN_BAS, INPUT) ;

	while (1) {

        if (digitalRead(PIN_IN_GAUCHE) == LOW)
        {
            //  musique précédente
            *isPlaying = FALSE;
            kill(pid, SIGKILL);
            *currentMusic = *currentMusic - 1;
            if (*currentMusic < 0) {
                *currentMusic = 0;
            }

            return  1; // a completer
        }
        if (digitalRead(PIN_IN_DROITE) == LOW)
        {
            //  musique suivante
            *isPlaying = FALSE;
            kill(pid, SIGKILL);
            *currentMusic = *currentMusic + 1;
            if (*currentMusic == 0) {
                *currentMusic = 0;
            }
            return 2; // a completer
        }
	}
    */
	return 0 ;
}


MusicMessage retrievePlaylist(){
    MusicMessage musicMessage;
    DIR *dir;
    struct dirent *entry;
    int i = 0;

    // Open the directory
    dir = opendir("playlist");
    if (dir == NULL) {
        perror("Unable to open directory");
        return musicMessage;
    }

    // Read the directory entries
    if (dir)
    while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type != DT_REG) {
                continue;
            }
        // Ignore "." and ".." entries
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Copy the entry name to the playlist
            strncpy(musicMessage.playlist[i], entry->d_name, MAX_BUFF - 1);
            musicMessage.playlist[i][MAX_BUFF - 1] = '\0'; // Ensure null-termination
            i++;
        }
    }
    closedir(dir);

    musicMessage.type = PLAYLIST_RETURN; // Assuming PLAYLIST_RETURN is defined elsewhere
    // You should set other fields of musicMessage as needed

    return musicMessage;
}

int findMusic(MusicMessage musicMessage, char *musicName) {
    if (musicMessage.playlist_size == 0) {
        return -1;
    }
    for (int i = 0; i < musicMessage.playlist_size; i++) {
        if (strcmp(musicMessage.playlist[i], musicName) == 0) {
            return i;
        }
    }
    return -1;
}