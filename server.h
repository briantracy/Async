#ifndef _ASYNC_SERVER_H
#define _ASYNC_SERVER_H

#include <pthread.h>

#include "common.h"

typedef struct {
    unsigned int id;
    point_t loc;
    char *name;
    FILE *stream;

    pthread_mutex_t stream_mutex;
} player_t;

typedef struct {
    player_t *players;
    unsigned int max_players;
    unsigned int num_players;

    pthread_mutex_t mutex;
} server_state_t;

void init_state(int);

void player_joined(FILE *);
int add_player(char *);
void remove_player(player_t *);

/* Game Logic */
int hit_test(player_t *, point_t, direction_t);


#endif /* ASYNC_SERVER_H */

