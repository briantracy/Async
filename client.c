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

#include "client.h"
#include "messages.h"
#include "network.h"
#include "common.h"



int usage() {
    printf("usage: async-client -n <player-name> -h <hostname> -p <port>\n");
    return 1;
}
void x();
int main(int argc, char *argv[]) {
    
    const int num_args = 7;
    char name[16], host[32], port[32];
    int ch;

    if (argc != num_args) { return usage(); }

    while ((ch = getopt(argc, argv, "n:h:p:")) != -1) {
        printf("[%c] -> [%s]\n", ch, optarg);
        switch (ch) {
        case 'n': { strcpy(name, optarg); break; }
        case 'h': { strcpy(host, optarg); break; }
        case 'p': { strcpy(port, optarg); break; }
        default: { return usage(); }
        }
    }
    if (optind != num_args) { return usage(); }

    int sock = get_socket(host, port);
    if (sock == -1) {
        fprintf(stdout, "could not connect to %s:%s\n", host, port);
        return 1;
    }

    FILE *io = fdopen(sock, "r+");
    if (io == NULL) {
        fprintf(stdout, "could not open socket\n");
        return 1;
    }

    char buff[NAME_LEN];
    snprintf(buff, NAME_LEN, "N %s\n", name);
    fputs(buff, io);
    fflush(io);
    
    if (receive_initial_data(io) != 0) {
        printf("Too many players\n");
        return 1;
    }
    return 0;
}

/* Get from the server our unique ID, the map size, map data */
int receive_initial_data(FILE *io) {
    
    char id_buff[ID_LEN];
    fgets(id_buff, ID_LEN, io);
    char *id_str = extract_message(id_buff);
    printf("id_str=[%s]\n", id_str);

    char size_buff[SIZE_LEN];
    fgets(size_buff, SIZE_LEN, io);
    char *size_tuple = extract_message(size_buff);
    printf("map_size=[%s]\n", size_tuple);

    int width, height;
    sscanf(size_tuple, "(%d,%d)", &width, &height);
    printf("map size = %d x %d\n", width, height);

    char map[width * height];
    fgets(map, width * height + 1, io);

    print_map(map, width, height);   

    return 1;
}

void x() {

    initscr();
    noecho();
    curs_set(FALSE);
    
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
