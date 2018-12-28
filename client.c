#include <stdio.h>
#include <unistd.h>
#include <curses.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <signal.h>

#include "client.h"
#include "messages.h"
#include "network.h"
#include "common.h"



int usage() {
    printf("usage: async-client -n <player-name> -h <hostname> -p <port>\n");
    return 1;
}

game_t game;


int main(int argc, char *argv[]) {

    const int num_args = 7;
    char name[NAME_LEN + 1], host[HOST_LEN + 1], port[PORT_LEN + 1];
    int ch;

    if (argc != num_args) { return usage(); }

    while ((ch = getopt(argc, argv, "n:h:p:")) != -1) {
        switch (ch) {
        case 'n': { strncpy(name, optarg, NAME_LEN); break; }
        case 'h': { strncpy(host, optarg, HOST_LEN); break; }
        case 'p': { strncpy(port, optarg, PORT_LEN); break; }
        default: { return usage(); }
        }
    }
    if (optind != num_args) { return usage(); }

    name[NAME_LEN] = '\0';
    host[HOST_LEN] = '\0';
    port[PORT_LEN] = '\0';

    signal(SIGINT, &quit_game);

    int sock = get_socket(host, port);
    if (sock == -1) {
        fprintf(stderr, "async: could not connect to `%s:%s`\n", host, port);
        return 1;
    }

    FILE *io = fdopen(sock, "r+");
    if (io == NULL) {
        fprintf(stderr, "async: could not open socket\n");
        return 1;
    }
    
    game.io = io;
    game.name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(game.name, name);
    
    printf("async: joining server `%s:%s` with name `%s`\n",
                host, port, game.name);
    
    if (!send_name()) {
        fprintf(stderr, "async: could not send name to server\n");
        return 1;
    }

    if (!receive_id()) {
        fprintf(stderr, "async: could not receive player id from server\n");
        return 1;
    }

    if (!download_map()) {
        fprintf(stderr, "async: could not download map\n");
        return 1;
    } else {
        printf("async: downloaded map, `%lu` bytes\n", strlen(game.map));
    }
    game.player_loc.x = game.width / 2;
    game.player_loc.y = game.height / 2;
    game.player_dir = NORTH;
    run_game();    

    return 0;
}

void quit_game(int sig) {
    endwin();
    fflush(game.io);
    fclose(game.io);
    exit(sig);
}

void run_game() {
    initscr();
    curs_set(FALSE);
    while (1) {
        render();
        refresh();
        get_input();
    }
}

void get_input() {
    int key = getch();
    switch (key) {
    case 'w': { return move_player(NORTH); }
    case 'a': { return move_player(WEST);  }
    case 's': { return move_player(SOUTH); }
    case 'd': { return move_player(EAST);  }
    case 'j': { return fire(); }
    }
}

void move_player(direction_t dir) {
    unsigned int x = game.player_loc.x;
    unsigned int y = game.player_loc.y;
    int success = 0;
    switch (dir) {
    case NORTH: { 
        if (get_char(game.map, x, y - 1, game.width) != '#') {
            game.player_loc.y--; success = 1;
        }
        break;
    }
    case EAST:  { 
        if (get_char(game.map, x + 1, y, game.width) != '#') {
            game.player_loc.x++; success = 1;
        }
        break;
    }
    case SOUTH: { 
        if (get_char(game.map, x, y + 1, game.width) != '#') {            
            game.player_loc.y++; success = 1;
        }
        break;
    }
    case WEST:  { 
        if (get_char(game.map, x - 1, y, game.width) != '#') {        
            game.player_loc.x--; success = 1;
        }
        break;
    }
    }
    game.player_dir = dir;
    if (!success) { return; }
    char buff[MOVE_LEN];
    snprintf(buff, MOVE_LEN, "%c (%d,%d)\n", H_MOVE,
                    game.player_loc.x, game.player_loc.y);
    fputs(buff, game.io);
    fflush(game.io);
}

void fire() {
    char buff[FIRE_LEN];
    snprintf(buff, FIRE_LEN, "%c (%d,%d)@%d\n", H_FIRE, game.player_loc.x, game.player_loc.y, game.player_dir);
    fputs(buff, game.io);
    fflush(game.io);
}

void render() {
    for (int x = 0; x < game.width; x++) {
        for (int y = 0; y < game.height; y++) {
            mvaddch(y, x, get_char(game.map, x, y, game.width));
        }
    }
    mvaddch(game.player_loc.y, game.player_loc.x, dir_to_symbol(game.player_dir));
}

/* msg_len = <char><space><field length><\n><\0> */
int send_name() {
    const int msg_len = NAME_LEN + 4;
    char buff[msg_len];
    snprintf(buff, msg_len, "%c %s\n", H_NAME, game.name);
    return fputs(buff, game.io) != EOF;
}

int receive_id() {
    const int msg_len = ID_LEN + 4;
    char buff[msg_len];
    if (fgets(buff, msg_len, game.io) == NULL) { return 0; }
    char *id_str = extract_message(buff);
    if (strcmp(id_str, "err") == 0) { return 0; }
    game.id = atoi(id_str);
    return 1;
}

int download_map() {
    const int size_buff_len = SIZE_LEN + 4;
    char buff[size_buff_len];
    if (fgets(buff, size_buff_len, game.io) == NULL) { return 0; }
    char *size_tuple = extract_message(buff);
    sscanf(size_tuple, "(%d,%d)", &game.width, &game.height);
    
    const int map_buff_size = sizeof(char) * game.width * game.height + 1;
    game.map = malloc(map_buff_size);
    if (fgets(game.map, map_buff_size, game.io) == NULL) { return 0; }
    return 1;
}


void x() {

    initscr();
    noecho();
    curs_set(FALSE);
    mvaddstr(20, 10, "abcdefghijklmnop");
    while (1) {
        int k = getch();
        char b[20];
        snprintf(b, 20, "[key=%c,%d", (char)k, k);
        doupdate();
        refresh();
        for (int i = 0; i < 40; i++) {
            for (int j = 0; j < 15; j++) {
                mvaddch(j, i, (char)((rand() % 80) + 33));
            }
        }
        mvaddstr(20, 10, b);
    }
    //mvaddstr(3, 3, "\033[1;31mred\033[0;0m");

   /* while (1) {
        mvaddstr(3, 3, "getting key\n");
        int k = getch();
        printf("k = %c\n", (char)k);
        if (k == 'a') break;
        sleep(1);
    }*/

/*char c = 'a';
while (1) {
    int k = getch();
    printf("%c\n", k);
    return 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            mvaddch(i, j, c);
        }
    }
c++;
refresh();
sleep(1);
}   
    mvaddstr(12, 12, "standard error");   
*/
    refresh();


    pause();
    endwin();
    printf("done\n");
}
