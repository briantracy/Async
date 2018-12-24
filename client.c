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


int get_socket(const char *server, const char *port) {
    // setup for getaddrinfo
    int sock;
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int err;
    if ((err = getaddrinfo(server, port, &hints, &result)) != 0) {
        fprintf(stderr, "Error in getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }

    // find the right interface
    struct addrinfo *res;
    for (res = result; res != NULL; res = res->ai_next) {
        if ((sock = socket(res->ai_family, res->ai_socktype,
                           res->ai_protocol)) < 0) {
            continue;
        }
        if (connect(sock, res->ai_addr, res->ai_addrlen) >= 0) {
            break;
        }
        close(sock);
    }

    freeaddrinfo(result);

    if (res == NULL) {
        return -1;
    }

    return sock;
}

int usage() {
    printf("usage: async-client -n <player-name> -h <hostname> -p <port>\n");
    return 1;
}

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

    FILE *cxn = fdopen(sock, "r+");
    fputs("abcdef", cxn);
    
    return 0;
}

void x() {

    initscr();
    noecho();
    curs_set(FALSE);

    while (1) {
        mvaddstr(3, 3, "getting key\n");
        int k = getch();
        printf("k = %c\n", (char)k);
        if (k == 'a') break;
        sleep(1);
    }

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


   // pause();
    endwin();
    printf("done\n");
}
