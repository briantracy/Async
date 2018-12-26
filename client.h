#ifndef _ASYNC_CLIENT_H
#define _ASYNC_CLIENT_H

#include "common.h"

typedef struct {
    unsigned int width, height;
    char *map;
    
    char *name;
    unsigned int id;

    point_t player_loc;
    direction_t player_dir;

    FILE *io;
} game_t;

int send_name();
int receive_id();
int download_map();

#define HOST_LEN 64
#define PORT_LEN 32

#define PLAYER_LEFT  '<'
#define PLAYER_RIGHT '>'
#define PLAYER_UP    '^'
#define PLAYER_DOWN  'v'

#define BEAM_HORZ  '-'
#define BREAM_VERT '|'

#define CLOAK '@'


#endif
