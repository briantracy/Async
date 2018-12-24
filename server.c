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
#include <pthread.h>

#include "server.h"
#include "common.h"
#include "network.h"
#include "messages.h"


server_state_t state;

int handshake(FILE *);

int usage() {
    printf("usage: async-server -p <port> -n <num-players> -w <board-width> -h <board-height>\n");
    return 1;
}

int main(int argc, char *argv[]) {
    const int num_args = 9;
    int port, players, width, height;
    int ch;

    if (argc != num_args) { return usage(); }
    
    while ((ch = getopt(argc, argv, "p:n:w:h:")) != -1) {
        printf("[%c] -> [%s]\n", ch, optarg);
        switch (ch) {
        case 'p': { port = atoi(optarg); break; }
        case 'n': { players = atoi(optarg); break; }
        case 'w': { width = atoi(optarg); break; }
        case 'h': { height = atoi(optarg); break; }
        default: { return usage(); }
        }
    }
    if (optind != num_args) { return usage(); }
    
    state.num_players = 0;
    state.max_players = players;
    state.port = port;
    state.width = width;
    state.height = height;
    
    generate_map();
    //pthread_t listener = start_listener(state.port, &player_joined);

    pause();
    return 0;
}

void player_joined(FILE *io) {
    printf("new player\n");
    char buff[24];
    fgets(buff, 24, io);
    printf("[%s]\n", buff);
    int i = fputs("I 12\n", io);
    fflush(io);
    printf("sent[%d]\n", i);
}

int handshake(FILE *io) {
    // 1. Read players name
    char name[NAME_LEN];
    return 0;
}

player_t *add_player(char *name) {
    int ret = 0;

    pthread_mutex_lock(&state.mutex);
    if (state.num_players == state.max_players) { ret = 1; }
    else { 
        state.num_players++;
        state.players[state.num_players - 1].id = 0;
    }
    
    pthread_mutex_unlock(&state.mutex);
    
    return NULL;
}

void remove_player(player_t *p) {

}

/* Game Logic */

void generate_map() {
    assert(state.width > 5 && state.height > 5);
    if (state.map != NULL) { free(state.map); }

    state.map = malloc(sizeof(char) * state.width * state.height);
    memset(state.map, ' ', state.width * state.height);

    for (int x = 0; x < state.width; x++) {
        set_char(state.map, x, 0, state.width, '#');
        set_char(state.map, x, state.height - 1, state.width, '#');
        //state.map[0 * state.width + x] = '#';
        //state.map[(state.height - 1) * state.width + x] = '#';
    }
    for (int y = 0; y < state.height; y++) {
        set_char(state.map, 0, y, state.width, '#');
        set_char(state.map, state.width - 1, y, state.width, '#');
        //state.map[y * state.width + 0] = '#';
        //state.map[y * state.width + (state.width - 1)] = '#';
    }

    set_char(state.map, 20, 1, state.width, '&');

    print_map(state.map, state.width, state.height);
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

