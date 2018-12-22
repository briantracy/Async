#include <stdio.h>
#include <assert.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "server.h"
#include "common.h"



int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: async-server <port>\n");
        return 1;
    }
    int port = atoi(argv[1]);
    int lsock;
    if ((lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    int y = 1;
    setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(lsock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        if (close(lsock) < 0) perror("close");
        exit(1);
    }

    if (listen(lsock, 100) < 0) {
        perror("listen");
        if (close(lsock) < 0) perror("close");
        exit(1);
    }

    fprintf(stderr, "listening on port %d\n", port);

    while (1) {
        int csock;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        if ((csock = accept(lsock, (struct sockaddr *)&client_addr,
                            &client_len)) < 0) {
            perror("accept");
            continue;
        }

        fprintf(stderr, "received connection from %s#%hu\n",
                inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

        FILE *cxstr;
        if (!(cxstr = fdopen(csock, "w+"))) {
            perror("fdopen");
            if (close(csock) < 0) perror("close");
            continue;
        }
        
        printf("ready to read\n");
    }

    return 0;
}


/* Return true iff a beam shot from origin in direction will
   collide with player */
int hit_test(player_t *p, point_t o, direction_t d) {
    switch (d) {
        case NORTH: { return p->loc.x == o.x && p->loc.y < o.y; }
        case SOUTH: { return p->loc.x == o.x && p->loc.y > o.y; }
        case EAST:  { return p->loc.y == o.y && p->loc.x > o.x; }
        case WEST:  { return p->loc.y == o.y && p->loc.x < o.x; }
        default: { assert(0); }
    }
    return 0;
}
