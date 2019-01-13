#ifndef _NETWORK_H
#define _NETWORK_H

#include <pthread.h>


pthread_t start_listener(int port, void (*serve_func)(FILE *));

int get_socket(const char *server, const char *port);

#endif
