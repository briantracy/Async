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

#include "messages.h"
#include "network.h"


int handshake(FILE *);

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
    
    handshake(io);
    
    return 0;
}

int handshake(FILE *io) {
    fputs("N name\n", io);
    printf("sent\n");
    char id[10];
        printf("waiting for fgets\n");
        fgets(id, 10, io);
        printf("id: [%s]\n", id);
    printf("done\n");
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
