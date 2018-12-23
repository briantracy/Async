#ifndef COMM_H_
#define COMM_H_

#include <errno.h>
#include <pthread.h>

#define BUFLEN 256
#define handle_error_en(en, msg) \
    do {                         \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

pthread_t start_listener(int port, void (*serve_func)(FILE *));
void comm_shutdown(FILE *cxstr);
int comm_serve(FILE *cxstr, char *resp, char *cmd);

#endif  // COMM_H_
