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


server_state_t state;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: server <port> <players>\n");
        return 1;
    }

    init_state(atoi(argv[2]));

    pthread_t listener = start_listener(atoi(argv[1]), &player_joined);

    pause();    
    return 0;
}

void init_state(int max_players) {
    state.num_players = 0;
    state.max_players = max_players;
    state.players = malloc(sizeof(player_t) * max_players);
    pthread_mutex_init(&state.mutex, NULL);
}

void player_joined(FILE *io) {
    printf("new client !\n");
}

int add_player(player_t *p) {
    int ret = 0;

    pthread_mutex_lock(&state.mutex);
    if (state.num_players == state.max_players) { ret = 1; }
    else { 
        state.num_players++;
        players[state.num_players - 1]
    }
    
    pthread_mutex_unlock(&state.mutex);
    return ret;

}

void remove_player(player_t *p) {

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
