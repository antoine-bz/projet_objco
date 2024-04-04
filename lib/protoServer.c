#include "protoServer.h"


int *currentMusic;
int *isPlaying;
int  *isChoosing;
int *musicPid;

int shm_id, shm_size;
void *shm_ptr;

int fd, lcd;

libvlc_instance_t *inst;
libvlc_media_t *m;
libvlc_media_player_t *mp;



void server (char *addrIPsrv, short server_port){
    socket_t server_socket;
    pid_t pid;
    lcd = initLCD();
    fd = init_7segment();
    inst = libvlc_new(0, NULL);

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
            exit(EXIT_SUCCESS);
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
                    while (*isChoosing == TRUE){
                        sleep(1); 
                    }
                }
                // sinon on envoie la musique courante au client
                sendCurrentMusic(client_socket);                
            }
            break;

        case SEND_MUSIC_CHOICE:
            printf("SEND_MUSIC_CHOICE received from client\n\n");

            printf("Received choice %d from client\n\n", musicMessage.current_music);
            // on met la musique choisie par le client dans currentMusic
            MusicMessage musicMessage2;
            *currentMusic =  musicMessage.current_music;

            musicMessage2.type = OK_REQ;
            envoyer(client_socket, &musicMessage2, (pFct)serializeMusicMessage);
            *isChoosing = FALSE;
            sendCurrentMusic(client_socket);
            break;

        case PLAYLIST_REQUEST:
            printf("PLAYLIST_REQUEST received from client\n\n");
            kill (*musicPid, SIGKILL);
            *isPlaying = FALSE;
            *currentMusic = UNDEFINED;
            *isChoosing = TRUE;
            sendPlaylist(client_socket);
            break;

        case QUIT:
            printf("Client disconnected\n\n");
            *isChoosing = FALSE;
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
    bufferMusic.current_music=*currentMusic;

    bufferMusic.type = MUSIC_RETURN;
    
    // on recupere le nom du fichier a telecharger et on ajoute le dossier dans lequel il se trouve devant

    printf("Sending %d to client...\n\n", *currentMusic);

    envoyer(client_socket, &bufferMusic, (pFct) serializeMusicMessage);

    printf("Waiting for client to confirm...\n\n");
    recevoir(client_socket, &bufferMusic, (pFct) deserializeMusicMessage);
    printf("Client confirmed\n\n");
    if (bufferMusic.type != OK_REQ) {
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
    if (musicMessage.type != OK_REQ) {
        printf("Error: client did not confirm\n");
        *isChoosing = FALSE;
        exit(EXIT_FAILURE);
    }
}


void myRadio(){
    MusicMessage musicMessage; 
    pid_t musicPlayPid, compoPid;
    
    // Construire le chemin complet du fichier
    char path[MAX_BUFF];
    
    // on recupere la playlist
    musicMessage = retrievePlaylist();

    while (1) {
        writeLCD(lcd,0,0, "En attente de   musique...");

        afficher_7segment_sec(fd, 0);
        
        // on attend que le client ait choisi une musique
        while (*isChoosing == TRUE || *currentMusic == UNDEFINED) {
            sleep(1);
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

            printf("currentMusic: %d\n", *currentMusic);
            snprintf(path, sizeof(path)+9, "playlist/%s", musicMessage.playlist[*currentMusic]);
            printf("path: %s\n", path);


            // Crée un nouvel objet média
            m = libvlc_media_new_path(inst, path);
            
            // Ajoute des options de streaming au média
            libvlc_media_add_option(m, ":sout=#transcode{acodec=mpga,ab=128,channels=2,samplerate=44100}:http{mux=ogg,dst=:8080/}");
            libvlc_media_add_option(m, ":no-sout-rtp-sap");
            libvlc_media_add_option(m, ":no-sout-standard-sap");
            libvlc_media_add_option(m, ":sout-keep");

            // Crée un lecteur média à partir du média
            mp = libvlc_media_player_new_from_media(m);

            // Ne plus avoir besoin du média
            libvlc_media_release(m);

            // Joue le média
            libvlc_media_player_play(mp);

            sleep(1);
            // Attendre la fin de la lecture
            while(libvlc_media_player_is_playing(mp));
            exit(EXIT_SUCCESS);
        }

        compoPid = fork();
        if (compoPid == -1) {
            perror("Erreur lors de la création du processus fils");
            exit(EXIT_FAILURE);
        } else
        if (compoPid == 0) {
            int sec = 0;
            // on affiche le nom de la musique sur l'ecran LCD
            printf("allumage LCD\n");
            // on efface l'ecran
            writeLCD(lcd,0,0, "                                                        ");
            // on ecrit le nom de la musique
            writeLCD(lcd,0,0, musicMessage.playlist[*currentMusic]);
            printf("allumage 7seg\n");
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
        kill(compoPid, SIGKILL);
  

    
        // on remet currentMusic à vide
        *currentMusic = UNDEFINED;
        *isPlaying = FALSE;
        *isChoosing = FALSE;
        /*
        *currentMusic=*currentMusic+1;
        if (*currentMusic == musicMessage.playlist_size+1) {
            *currentMusic = 0;
        }
        */   

    }
    
    // on remet currentMusic à vide
    *currentMusic = UNDEFINED;
    *isPlaying = FALSE;
    *isChoosing = FALSE;
    // Nettoyage
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(inst);   

    exit(EXIT_SUCCESS);    
}


static void signalHandler(int sig) {
    switch (sig) {
        case SIGINT:
            // Fermer le fichier et terminer le programme lorsque CTRL+C est détecté
            printf("\nFermeture du serveur...\n");
            CHECK(munmap(shm_ptr, shm_size) == 0, "munmap error");
            CHECK(close(shm_id) == 0, "close error"); 
            // Nettoyage
            libvlc_media_player_stop(mp);
            libvlc_media_player_release(mp);
            libvlc_release(inst);
            exit(EXIT_SUCCESS);
            break;

        default:
            printf("Signal inconnu\n");
            break;
    }
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
