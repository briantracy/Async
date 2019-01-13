#ifndef _ASYNC_SERVER_H
#define _ASYNC_SERVER_H

#include <pthread.h>

#include "common.h"

typedef struct {
    unsigned int id;
    point_t loc;
    char *name;
    FILE *io, publish_stream;
    
    // Each client gets its own thread on the server
    pthread_t thread;
    // Each client thread updates location, but main thread
    // has to read locations to send updates to clients.
    pthread_mutex_t loc_mutex;
    // Make sure only one thread (either the client thread or the
    // publisher thread) is writing to each clients stream at once.
    // TODO: are FILE * operations threadsafe?
    pthread_mutex_t stream_mutex;
} player_t;

typedef struct {
    player_t *players;
    unsigned int max_players;
    unsigned int num_players;

    int port;
    unsigned int width, height;

    char *map;

    pthread_mutex_t player_mutex;
} server_state_t;


void player_joined(FILE *);
void *run_player(void *); // thread func
player_t *add_player(char *, FILE *);
void remove_player(player_t *);
void respond(player_t *, char *);

void *publisher(void *);
void publish_positions();


/* Game Logic */
int hit_test(player_t *, point_t, direction_t);
void generate_map();

#endif /* ASYNC_SERVER_H */

