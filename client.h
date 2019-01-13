#ifndef _CLIENT_H
#define _CLIENT_H

#include "common.h"
#include <assert.h>

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

void run_game();
void get_input();
void move_player(direction_t);
void fire();
void render();

void *game_listener(void *); // thread func

void quit_game(int);

#define HOST_LEN 64
#define PORT_LEN 32

#define PLAYER_LEFT  '<'
#define PLAYER_RIGHT '>'
#define PLAYER_UP    '^'
#define PLAYER_DOWN  'v'

char dir_to_symbol(direction_t dir) {
    switch (dir) {
    case NORTH: return PLAYER_UP;
    case SOUTH: return PLAYER_DOWN;
    case EAST:  return PLAYER_RIGHT;
    case WEST:  return PLAYER_LEFT;
    default: { assert(0); }
    }
}

#define BEAM_HORZ  '-'
#define BREAM_VERT '|'

#define CLOAK '@'


#endif
