#include "protoClient.h"

socket_t sockDial;
int mute; // Local mute variable
WINDOW *playlist_win; // Window for the playlist

void client(char *addrIPsrv, short port) {
    char reponse[MAX_BUFF];
    MusicMessage buffer;
    int choix;
    int client_connected = 1;

    initscr();              // Initialize ncurses
    start_color();          // Start color functionality
    init_pair(2, COLOR_WHITE, COLOR_MAGENTA);  // Background close to terminal background
    init_pair(3, COLOR_BLACK, COLOR_MAGENTA);            // Text color close to playlist background
    init_pair(4, COLOR_BLACK, COLOR_WHITE);    // Text color close to terminal background
    

    cbreak();
    noecho();
    keypad(stdscr, TRUE);    // Enable keyboard mapping
    curs_set(0);             // Hide cursor

    // Connect to the server
    sockDial.mode = SOCK_STREAM;
    sockDial.sock = connecter(addrIPsrv, port);
    sockDial.type = SOCK_CLIENT;
    ouvrirSocket(&sockDial, SOCK_STREAM, addrIPsrv, port);


        // Get the screen size and calculate the window size and position
        int max_y, max_x, window_y, window_x, start_y, start_x;
        getmaxyx(stdscr, max_y, max_x);  // Get screen size
        // Calculate the window size and position
        window_y = max_y / 2;
        window_x = max_x / 2;
        start_y = (max_y - window_y) / 2;
        start_x = (max_x - window_x) / 2;
        // Create a new window for the playlist
        playlist_win = newwin(window_y, window_x, start_y, start_x);
        // Set the window color and text attributes
        wbkgd(playlist_win, COLOR_PAIR(2));  // Window background color
        wattron(playlist_win, COLOR_PAIR(2)); // Window text color
        box(playlist_win, 0, 0); // Standard box border for a fine line


            wattron(playlist_win, A_BOLD); // Set bold attribute
            wattron(playlist_win, A_DIM);  // Set dim attribute (reduce intensity, which can simulate smaller text)
            mvwprintw(playlist_win, 1, (window_x - 9) / 2, "myRadio");
            wattroff(playlist_win, A_BOLD); // Turn off bold attribute
            wattroff(playlist_win, A_DIM);  // Turn off dim attribute


        mvwprintw(playlist_win, window_y / 2, (window_x - 19) / 2, "Welcome to myRadio!");
        // waiting for music marque en dessous de welcome to myRadio
        mvwprintw(playlist_win, window_y / 2 + 1, (window_x - 36) / 2, "Waiting for music from the server...");
        // Print the window border
        wrefresh(playlist_win);
        
        // Request the playlist from the server
        buffer.type = SEND_MUSIC_REQUEST;
        envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);
        recevoir(&sockDial, &buffer, (pFct)deserializeMusicMessage);

    while (client_connected) {
        if (buffer.type == PLAYLIST_RETURN) {
            // on supprime le message d'attente
            werase(playlist_win);
            refresh();

            // on affiche le titre
            
            ITEM *cur_item;
            MENU *my_menu;
            ITEM **my_items;
            int n_choices = buffer.playlist_size+1;

            my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
            for (int i = 0; i < n_choices-1; ++i) {
                my_items[i] = new_item(buffer.playlist[i], "");
            }
            // on ajoute un item pour quitter
            my_items[n_choices-1] = new_item("Quit", "");

            my_items[n_choices] = (ITEM *)NULL;

            my_menu = new_menu((ITEM **)my_items);
            set_menu_win(my_menu, playlist_win);
            
            // Calculate the center position for the menu window
            int menu_start_y = (window_y - n_choices) / 2; // Place the menu vertically centered
            int menu_start_x = (window_x - n_choices*10) / 2; // Place the menu horizontally centered

            // Create the menu window
            set_menu_sub(my_menu, derwin(playlist_win, n_choices, n_choices*10, menu_start_y, menu_start_x));



            set_menu_format(my_menu, n_choices, 1);
            set_menu_mark(my_menu, " * ");

            menu_opts_off(my_menu, O_SHOWDESC);

            set_menu_fore(my_menu, COLOR_PAIR(4));
            set_menu_back(my_menu, COLOR_PAIR(2));
            
        
            box(playlist_win, 0, 0); 
            // on affiche MyRadio
            wattron(playlist_win, A_BOLD); // Set bold attribute
            wattron(playlist_win, A_DIM);  // Set dim attribute (reduce intensity, which can simulate smaller text)
            mvwprintw(playlist_win, 1, (window_x - 9) / 2, "myRadio");
            wattroff(playlist_win, A_BOLD); // Turn off bold attribute
            wattroff(playlist_win, A_DIM);  // Turn off dim attribute

            mvwprintw(playlist_win, 2, (window_x - 16) / 2, "Choose a song:");
            refresh();

            // on place le menu au milieu de la fenetre
            post_menu(my_menu);
            wrefresh(playlist_win);

            int c;
            while ((c = getch()) != 10) {
                switch (c) {
                    case KEY_DOWN:
                        menu_driver(my_menu, REQ_DOWN_ITEM);
                        break;
                    case KEY_UP:
                        menu_driver(my_menu, REQ_UP_ITEM);
                        break;
                }
                wrefresh(playlist_win);
            }

            // Get the selected item
            cur_item = current_item(my_menu);
            choix = item_index(cur_item); // Adjust the choice for a 1-based index

            unpost_menu(my_menu);
            free_menu(my_menu);
            for(int i = 0; i < n_choices; ++i) {
                free_item(my_items[i]);
            }

            // on affiche le choix
            mvwprintw(playlist_win, window_y / 2, (window_x - strlen(buffer.playlist[choix ]) - 16) / 2,
                    "You chose '%s'.", buffer.playlist[choix]);

            int prompt_y_pos = window_y - 1;
            int prompt_x_pos = 1;


            buffer.type = OK_REQ;
            envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);

            // Check if the user wants to quit
            if (choix == (n_choices-1)) {
                buffer.type = QUIT;
                envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);
                client_connected = 0;
                break;
            } else if (choix >= 0 && choix < n_choices) {
                buffer.current_music = choix; // Adjust the choice for a 0-based index

                //print un msg pour debug
                //printw("Sending music choice to server...\n");
                buffer.type = SEND_MUSIC_CHOICE;
                envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);
                recevoir(&sockDial, &buffer, (pFct)deserializeMusicMessage); 

                if (buffer.type != OK_REQ) {
                    // Handle invalid choice
                     box(playlist_win, 0, 0); 
                    mvwprintw(playlist_win ,window_y / 2, (window_x - 50 - 16) / 2, "Invalid music choice. Press any key to try again.");
                    wrefresh(playlist_win);
                    wgetch(playlist_win); // Wait for any key press
                }
            } else {
                // Handle invalid input (non-integer or out of range)
                box(playlist_win, 0, 0); 
                mvwprintw(playlist_win, window_y / 2, (window_x - 50 - 16) / 2, "Invalid choice. Press any key to try again.");
                wrefresh(playlist_win);
                wgetch(playlist_win); // Wait for any key press
            }
        }
        else if (buffer.type != MUSIC_RETURN) {
            // Handle invalid playlist
            box(playlist_win, 0, 0); 
            mvwprintw(playlist_win, window_y / 2, (window_x - 50 - 16) / 2, "Invalid playlist. Press any key to try again.");
            wrefresh(playlist_win);
            wgetch(playlist_win); // Wait for any key press
        }
        else
        if (buffer.type == MUSIC_RETURN) {
            buffer.type = OK_REQ;
            envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);
        }
        sleep(2); // Pause to read message
        
        choix = buffer.current_music;
        
        werase(playlist_win);
        refresh();
        wattron(playlist_win, A_BOLD); // Set bold attribute
        wattron(playlist_win, A_DIM);  // Set dim attribute (reduce intensity, which can simulate smaller text)
        mvwprintw(playlist_win, 1, (window_x - 9) / 2, "myRadio");
        wattroff(playlist_win, A_BOLD); // Turn off bold attribute
        wattroff(playlist_win, A_DIM);  // Turn off dim attribute

        // Music choice is valid; proceed to play music
        // print the playlist wirh the chosen music in black and bold

        for (int i = 0; i < buffer.playlist_size; i++) {
            if (i == choix) {
                wattron(playlist_win, COLOR_PAIR(3)); // Turn off the playlist text color            
                
                wattron(playlist_win, A_BOLD); // Set bold attribute
                mvwprintw(playlist_win, i + 3, (window_x - strlen(buffer.playlist[i])) / 2, "- %s -", buffer.playlist[i]);
                wattroff(playlist_win, A_BOLD); // Turn off bold attribute
                wattroff(playlist_win, COLOR_PAIR(3)); // Turn on the playlist text color
                
            } else {
                mvwprintw(playlist_win, i + 3, (window_x - strlen(buffer.playlist[i])) / 2, "%s", buffer.playlist[i]);
            }
        }
        
        // affichage des bordures
        box(playlist_win, 0, 0);
        wattron(playlist_win, COLOR_PAIR(3)); // Turn off the playlist text color 
        mvwprintw(playlist_win, window_y / 2, (window_x - strlen(buffer.playlist[choix]) - 16) / 2,
                "'%s' is playing...", buffer.playlist[choix]);
        wattroff(playlist_win, COLOR_PAIR(3)); // Turn on the playlist text color
        refresh();
        wrefresh(playlist_win);

        // Play music
        streamAudioClient(addrIPsrv);

        sleep(2); // Pause to read message
        
        // Ask the user if they want to choose another song
        if (client_connected){
            werase(playlist_win);
            wattron(playlist_win, A_BOLD); // Set bold attribute
            wattron(playlist_win, A_DIM);  // Set dim attribute (reduce intensity, which can simulate smaller text)
            mvwprintw(playlist_win, 1, (window_x - 9) / 2, "myRadio");
            wattroff(playlist_win, A_BOLD); // Turn off bold attribute
            wattroff(playlist_win, A_DIM);  // Turn off dim attribute
            refresh();

            box(playlist_win, 0, 0); 
            // Print the instructions
            mvwprintw(playlist_win, window_y / 2, (window_x - 50 - 16) / 2, "Press any key to play another song or 'q' to quit.");
            wrefresh(playlist_win);
            int c = wgetch(playlist_win); // Wait for any key press
            if (c == 'q') {
                client_connected = 0;
            }else{
                // Request the playlist from the server
                werase(playlist_win);
                buffer.type = SEND_MUSIC_REQUEST;
                envoyer(&sockDial, &buffer, (pFct)serializeMusicMessage);
                recevoir(&sockDial, &buffer, (pFct)deserializeMusicMessage);
                wattron(playlist_win, A_BOLD); // Set bold attribute
                wattron(playlist_win, A_DIM);  // Set dim attribute (reduce intensity, which can simulate smaller text)
                mvwprintw(playlist_win, 1, (window_x - 9) / 2, "myRadio");
                wattroff(playlist_win, A_BOLD); // Turn off bold attribute
                wattroff(playlist_win, A_DIM);  // Turn off dim attribute


                mvwprintw(playlist_win, window_y / 2, (window_x - 19) / 2, "Welcome to myRadio!");
                // waiting for music marque en dessous de welcome to myRadio
                mvwprintw(playlist_win, window_y / 2 + 1, (window_x - 36) / 2, "Waiting for music from the server...");
                // Print the window border
                wrefresh(playlist_win);
                sleep(2); // Pause to read message
            }
        }
    }

    // Close the socket when done
    printw("Client disconnecting...\n");
    closeSocket(&sockDial);
    delwin(playlist_win); // Delete the playlist window
    endwin(); // End ncurses
}
        
void closeSocket(socket_t* sockDial) {
    close(sockDial->sock);
}

