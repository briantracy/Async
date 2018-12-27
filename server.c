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
    state.players = malloc(sizeof(player_t) * state.max_players);
    state.port = port;
    state.width = width;
    state.height = height;
    
    generate_map();
    pthread_t listener = start_listener(state.port, &player_joined);

    pause();
    return 0;
}

void player_joined(FILE *io) {
    // Client starts handshake with name.
    const int name_buff_len = NAME_LEN + 4;
    char buff[name_buff_len];
    fgets(buff, name_buff_len, io);
    char *name = extract_message(buff);
    player_t *player = add_player(name);
    if (player == NULL) {
        fprintf(stderr, "could not create new player\n");
        fputs(SERVER_ERROR, io);
        fflush(io);
        pthread_exit(NULL);
    }
    printf("new player: name = `%s`, id = `%d`\n", player->name, player->id);

    const int id_buff_len = ID_LEN + 4;
    char id_buff[id_buff_len];
    snprintf(id_buff, id_buff_len, "%c %d\n", H_ID, player->id);
    fputs(id_buff, io);
    fflush(io);

    const int size_buff_len = SIZE_LEN + 4;
    char size_buff[size_buff_len];
    snprintf(size_buff, size_buff_len, "%c (%d,%d)\n", H_SIZE, state.width, state.height);
    fputs(size_buff, io);
    fflush(io);

    // 1 char per map cell + newline + '\0'
    const size_t map_buff_size = state.width * state.height + 2;
    char map_buff[map_buff_size];
    snprintf(map_buff, map_buff_size, "%s\n", state.map);
    print_map(map_buff, state.width, state.height);
    fputs(map_buff, io);
    fflush(io);
    
    char msg[100];
    while (fgets(msg, 100, io) != NULL) {
        printf("`%s`\n", msg);
    }
}

int transfer_initial_data(FILE *io) {
    
    return 0;
}

player_t *add_player(char *name) {
    player_t *ret = NULL;

    pthread_mutex_lock(&state.mutex);
    if (state.num_players != state.max_players) {
        ret = &state.players[state.num_players - 1];
        ret->id = state.num_players;
        ret->name = name;
        state.num_players++;
    }
    
    pthread_mutex_unlock(&state.mutex);
    
    return ret;
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
    }
    for (int y = 0; y < state.height; y++) {
        set_char(state.map, 0, y, state.width, '#');
        set_char(state.map, state.width - 1, y, state.width, '#');
    }
    
    set_char(state.map, 4, 4, state.width, '#');

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

