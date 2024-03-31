
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