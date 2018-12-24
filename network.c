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


#include "network.h"

/* Serverside I/O functions */

int lsock;

static void *listener(void (*server)(FILE *));

static int comm_port;

pthread_t start_listener(int port, void (*server)(FILE *)) {
    comm_port = port;
    pthread_t tid;
    int err;

    if ((err = pthread_create(&tid, 0, (void *(*)(void *))listener,
                              (void *)server)))
        handle_error_en(err, "pthread_create");
    if ((err = pthread_detach(tid))) handle_error_en(err, "pthread_detach");

    return tid;
}

void *listener(void (*server)(FILE *)) {
    if ((lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    int y = 1;
    setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)comm_port);
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

    fprintf(stderr, "listening on port %d\n", comm_port);

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

        server(cxstr);
    }

    return NULL;
}

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


void comm_shutdown(FILE *cxstr) {
    if (fclose(cxstr) < 0) perror("fclose");
}

int comm_serve(FILE *cxstr, char *response, char *command) {
    if (strlen(response) > 0) {
        if (fputs(response, cxstr) == EOF || fputc('\n', cxstr) == EOF ||
            fflush(cxstr) == EOF) {
            fprintf(stderr, "client connection terminated\n");
            return -1;
        }
    }

    if (fgets(command, BUFLEN, cxstr) == NULL) {
        fprintf(stderr, "client connection terminated\n");
        return -1;
    }

    return 0;
}
