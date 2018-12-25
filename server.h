#ifndef _ASYNC_SERVER_H
#define _ASYNC_SERVER_H

#include <pthread.h>

#include "common.h"

typedef struct {
    unsigned int id;
    point_t loc;
    char *name;
    FILE *stream;
    
    pthread_mutex_t loc_mutex;
    pthread_mutex_t stream_mutex;
} player_t;

typedef struct {
    player_t *players;
    int max_players;
    int num_players;

    int port;
    int width, height;

    char *map;

    pthread_mutex_t mutex;
} server_state_t;


void player_joined(FILE *);
player_t *add_player(char *);
void remove_player(player_t *);

int transfer_initial_data(FILE *);


/* Game Logic */
int hit_test(player_t *, point_t, direction_t);
void generate_map();

#endif /* ASYNC_SERVER_H */

