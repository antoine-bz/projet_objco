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
    isPlaying = (int *)(shm_ptr + sizeof(int) +sizeof(int) );
    isChoosing = (int *)(shm_ptr + sizeof(int) + sizeof(int) + sizeof(int));
    musicPid = (int *)(shm_ptr + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int));


    // on initialise les variables
    *currentMusic = UNDEFINED;
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
            printf("musicPid: %d\n", *musicPid);
            sleep(10);
        }
    }

    while (1) {
       socket_t client_socket = accepterConnection(&server_socket);
        pid_t pid = fork();

        if (pid == 0) { // Child process
            fermerSocket(&server_socket);
            // Here, the client communication is handled in a loop, and exit is called when done.
            while (handle_client(&client_socket)) { /* Handle client communication */ }
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0) { // Parent process
            fermerSocket(&client_socket); // The parent doesn't need this anymore.
        }
        else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    
    }

}


int handle_client(socket_t *client_socket) {
    buffer_t request;
    MusicMessage musicMessage;
    MusicMessage musicResponse;

    recevoir(client_socket, &musicMessage, (pFct) deserializeMusicMessage);

    // on fait un switch pour savoir quel commande a ete envoye par le client
    switch (musicMessage.type) {
        case SEND_MUSIC_REQUEST:
            sendPlaylist(client_socket);
            break;

        case SEND_MUSIC_CHOICE:
            printf("SEND_MUSIC_CHOICE received from client\n\n");

            printf("Received choice %d from client\n\n", musicMessage.current_music);
            // on met la musique choisie par le client dans currentMusic
            MusicMessage musicMessage2;
            musicMessage2 = retrievePlaylist();
            *currentMusic =  musicMessage.current_music;

            musicMessage2.type = OK;
            envoyer(client_socket, &musicMessage2, (pFct)serializeMusicMessage);

            break;

        case PLAYLIST_REQUEST:
           if (*isPlaying) {
                kill(*musicPid, SIGKILL);
            }
            *isPlaying = FALSE;
            *currentMusic = UNDEFINED;
            *isChoosing = TRUE; // Allow the client to make a new choice.
            sendPlaylist(client_socket);
            break;

        case QUIT:
            printf("Client disconnected\n\n");
            return 0;
            break;
        
        default:
            // on envoie un message d'erreur au client
            envoyer(client_socket, "Commande inconnue", NULL);
            break;
    }return 1;
}


void sendCurrentMusic(socket_t *client_socket) {
    buffer_t buffer;
    int bytesRead;
    int i=0;
    MusicMessage bufferMusic;

    bufferMusic = retrievePlaylist();
    bufferMusic.current_music=*currentMusic;

    bufferMusic.type = MUSIC_RETURN;
    
    // on recupere le nom du fichier a telecharger et on ajoute le dossier dans lequel il se trouve devant

    printf("Sending %d to client...\n\n", *currentMusic);

    envoyer(client_socket, &bufferMusic, (pFct) serializeMusicMessage);

    recevoir(client_socket, &bufferMusic, (pFct) deserializeMusicMessage);
    if (bufferMusic.type != OK) {
        exit(EXIT_FAILURE);
    }
}


void sendPlaylist(socket_t *client_socket) {
    MusicMessage musicMessage;
    *isChoosing = FALSE;

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
    *currentMusic = musicMessage.current_music;

    musicMessage.type = OK;
    envoyer(client_socket, &musicMessage, (pFct)serializeMusicMessage);


    *isChoosing = FALSE;
    *isChoosing = 0;

}

/*
void myRadio(){
    MusicMessage musicMessage; 
    pid_t musicPlayPid, buttonPid, segmentPid;
    int fd = init_7segment();
    int lcd = initLCD();
    
    while(*currentMusic == UNDEFINED);

    // on recupere la playlist
    musicMessage = retrievePlaylist();


    while (1) {
        
        if (*isChoosing || *currentMusic == UNDEFINED) {
            sleep(1); // Utiliser sleep pour éviter la consommation inutile de CPU
            continue; // Revenir au début de la boucle while
        }


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

            printf("currentMusic: %d\n", *currentMusic);
            snprintf(path, sizeof(path)+9, "playlist/%s", musicMessage.playlist[*currentMusic]);
            printf("path: %s\n", path);

            // on affiche le nom de la musique sur l'ecran LCD
            writeLCD(lcd,0,0, musicMessage.playlist[*currentMusic]);

            streamAudioServer(path); // Jouer le fichier audio
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

        segmentPid = fork();
        if (segmentPid == -1) {
            perror("Erreur lors de la création du processus fils");
            exit(EXIT_FAILURE);
        } else
        if (segmentPid == 0) {
            int sec = 0;
            // on affiche le temps de la musique
            while (*isPlaying == TRUE) {
                afficher_7segment_sec(fd, sec);
                sleep(1);
                sec++;
            }
            exit(EXIT_SUCCESS);
        }
        

        waitpid(musicPlayPid, NULL, 0);
        
        sleep(1);

        // on passe à la musique suivante
        *isPlaying = FALSE;
        printf("Playing next music...\n");
        kill(buttonPid, SIGKILL);
        kill(segmentPid, SIGKILL);
  
        
        *currentMusic=*currentMusic+1;
        
        if (*currentMusic == musicMessage.playlist_size+1) {
            *currentMusic = 0;
        }
               

    }
    
    // on remet currentMusic à vide
    *currentMusic = UNDEFINED;
    *isPlaying = FALSE;
    *isChoosing = FALSE;


    exit(EXIT_SUCCESS);    
}
*/
void myRadio() {
    MusicMessage musicMessage; 
    pid_t musicPlayPid, buttonPid, segmentPid;
    int fd = init_7segment();
    int lcd = initLCD();
    
    // Initialisation et récupération de la playlist
    musicMessage = retrievePlaylist();
    
    while (1) {
        // Attendre que le client fasse un choix ou que la musique courante ait fini de jouer
        if (*isChoosing || *currentMusic == UNDEFINED) {
            sleep(1); // Utiliser sleep pour éviter la consommation inutile de CPU
            continue; // Revenir au début de la boucle while
        }
        
        // Si une musique est choisie et qu'on n'est pas en train de jouer, lancer la musique
        if (!*isPlaying && *currentMusic != UNDEFINED) {
            musicPlayPid = fork();
            if (musicPlayPid == 0) { // Child process
                *musicPid = getpid();
                *isPlaying = TRUE;

                // Construire le chemin complet du fichier
                char path[MAX_BUFF];
                snprintf(path, sizeof(path), "playlist/%s", musicMessage.playlist[*currentMusic]);

                // Afficher le nom de la musique sur l'écran LCD et jouer la musique
                writeLCD(lcd, 0, 0, musicMessage.playlist[*currentMusic]);
                streamAudioServer(path);
                exit(EXIT_SUCCESS);
            }
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

        segmentPid = fork();
        if (segmentPid == -1) {
            perror("Erreur lors de la création du processus fils");
            exit(EXIT_FAILURE);
        } else
        if (segmentPid == 0) {
            int sec = 0;
            // on affiche le temps de la musique
            while (*isPlaying == TRUE) {
                afficher_7segment_sec(fd, sec);
                sleep(1);
                sec++;
            }
            exit(EXIT_SUCCESS);
        }
        

        waitpid(musicPlayPid, NULL, 0);
        
        sleep(1);

        // on passe à la musique suivante
        *isPlaying = FALSE;
        printf("Playing next music...\n");
        kill(buttonPid, SIGKILL);
        kill(segmentPid, SIGKILL);
  
        
        *currentMusic=*currentMusic+1;
        
        if (*currentMusic == musicMessage.playlist_size+1) {
            *currentMusic = 0;
        }
               

    }
        // Gestion des boutons et affichage 7-segments (buttonPid et segmentPid)

        // Attendre que la musique en cours finisse de jouer
        int status;
        waitpid(musicPlayPid, &status, 0);
        *isPlaying = FALSE;
        *currentMusic = UNDEFINED; // Réinitialiser currentMusic pour indiquer qu'on attend le prochain choix
        
        // On ne passe pas à la musique suivante automatiquement, donc on retire la partie qui incrémentait currentMusic
    }
    // Fermeture des ressources et sortie de la fonction



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
	#ifdef JOYPI

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
            *currentMusic = *currentMusic - 2;
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
            *currentMusic = *currentMusic ;
            if (*currentMusic == 0) {
                *currentMusic = 0;
            }
            return 2; // a completer
        }
	}

    #endif
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
    musicMessage.playlist_size = i;
    musicMessage.type = PLAYLIST_RETURN; // Assuming PLAYLIST_RETURN is defined elsewhere
    // You should set other fields of musicMessage as needed

    return musicMessage;
}
