#ifndef _ASYNC_SERVER_H
#define _ASYNC_SERVER_H

#include <pthread.h>

#include "common.h"

typedef struct {
    unsigned int id;
    point_t loc;
    char *name;
    FILE *io;
    
    pthread_t thread;
    pthread_mutex_t loc_mutex;
    pthread_mutex_t stream_mutex;
} player_t;

typedef struct {
    player_t *players;
    unsigned int max_players;
    unsigned int num_players;

    int port;
    unsigned int width, height;

    char *map;

    pthread_mutex_t mutex;
} server_state_t;


void player_joined(FILE *);
void *run_player(void *); // thread func
player_t *add_player(char *, FILE *);
void remove_player(player_t *);
void respond(char *);



/* Game Logic */
int hit_test(player_t *, point_t, direction_t);
void generate_map();

#endif /* ASYNC_SERVER_H */

