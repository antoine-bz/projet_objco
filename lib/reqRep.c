
#include "reqRep.h"

void deserializeMusicMessage(buffer_t buff, generic quoi)
{
    //printf("Deserialized message: %s\n", buff);
    MusicMessage *msg = (MusicMessage *)quoi;
    ////////////////DEFINI TYPE/////////////////////
    char type[MAX_BUFF];
    strcpy(type,strtok(buff, "|"));

    if (strcmp(type, "PLAYLIST_RETURN") == 0) {
        msg->type = PLAYLIST_RETURN;
    }
    else if (strcmp(type, "MUSIC_RETURN") == 0) {
        msg->type = MUSIC_RETURN;
    }
    else if (strcmp(type, "SEND_MUSIC_CHOICE") == 0) {
        msg->type = SEND_MUSIC_CHOICE;
    }
    else if (strcmp(type, "SEND_MUSIC_REQUEST") == 0) {
        msg->type = SEND_MUSIC_REQUEST;
    }
    else {
        printf("Erreur de type de message");
    }
    ///////////////////SWITCH TYPE/////////////////////

    switch (msg->type)
    {
    case PLAYLIST_RETURN: 
        char *token = strtok(NULL, "|"); // Récupérer la partie de la chaîne après le délimiteur '|'
        msg->playlist_size = atoi(token); // Convertir la taille de la playlist en entier
        token = strtok(NULL, "|"); // Récupérer la partie de la chaîne après le délimiteur '|'
        if (token != NULL) {
            char music[MAX_BUFF]; // Stocker temporairement chaque nom de musique
            int index = 0;
            char *ptr = token;
            while (*ptr != '\0' && index < MAX_BUFF) {
                int i = 0;
                // Copier chaque caractère jusqu'à '/' ou la fin de la chaîne
                while (*ptr != '/' && *ptr != '\0' && i < MAX_BUFF - 1) {
                    music[i] = *ptr;
                    ptr++;
                    i++;
                }
                music[i] = '\0'; // Assurer une terminaison nulle
                strncpy(msg->playlist[index], music, sizeof(msg->playlist[index]) - 1);
                msg->playlist[index][sizeof(msg->playlist[index]) - 1] = '\0'; // Assurer une terminaison nulle
                index++;
                if (*ptr == '/') {
                    ptr++; // Passer au prochain nom de musique s'il y en a un
                }
            }
        }
        break;

    case MUSIC_RETURN:
        strcpy(msg->current_music, strtok(NULL, "|"));
        break;
    case SEND_MUSIC_CHOICE:
        strcpy(msg->current_music, strtok(NULL, "|"));
        break;
    case SEND_MUSIC_REQUEST:
        break;
    default:
        break;
    }
}

void serializeMusicMessage(generic quoi, buffer_t buff){
    MusicMessage *msg = (MusicMessage *)quoi;
    char time_str[MAX_BUFF];

    switch (msg->type)
    {
    case PLAYLIST_RETURN:
        strcpy(buff, "PLAYLIST_RETURN|");
        sprintf(time_str, "%d", msg->playlist_size);
        strcat(buff, time_str);
        strcat(buff, "|");
    
        for (int i = 0; i < MAX_BUFF; i++) {
            if (msg->playlist[i][0] == '\0') {
                break;
            }
            strcat(buff, msg->playlist[i]);
            strcat(buff, "/");
        }
        break;
    case MUSIC_RETURN:
        strcpy(buff, "MUSIC_RETURN|");
        strcat(buff, msg->current_music);
        break;
    case SEND_MUSIC_CHOICE:
        strcpy(buff, "SEND_MUSIC_CHOICE|");
        strcat(buff, msg->current_music);
        break;
    case SEND_MUSIC_REQUEST:
        strcpy(buff, "SEND_MUSIC_REQUEST|");
        strcat(buff, msg->current_music);
        break;
    default:
        break;
    }

    //printf("Serialized message: %s\n", buff);
}
