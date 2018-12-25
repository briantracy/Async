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
    char id_buff[ID_LEN];
    snprintf(id_buff, ID_LEN, "%c %d\n", H_ID, 4);
    fputs(id_buff, io);

    char size_buff[SIZE_LEN];
    snprintf(size_buff, SIZE_LEN, "%c (%d,%d)\n", H_SIZE, state.width, state.height);
    fputs(size_buff, io);

    // 1 char per map cell + newline + '\0'
    const size_t map_buff_size = state.width * state.height + 2;
    char map_buff[map_buff_size];
    snprintf(map_buff, map_buff_size, "%s\n", state.map);
    fputs(map_buff, io);
    
    fflush(io);
    //fputs("Z (12,12)\n", io);
    //printf("[%s]\n", state.map);
    //char j[1000];
    //snprintf(j, 1000, "%s\n", state.map);
    //fputs(j, io);
    //fputs("\n", io);
    //fflush(io);
    return ;
   /* printf("new player\n");
    char buff[24];
    fgets(buff, 24, io);
    printf("[%s]\n", buff);
    int i = fputs("I 12\n", io);
    fflush(io);
    printf("sent[%d]\n", i);*/
    char buff[NAME_LEN];
    fgets(buff, NAME_LEN, io);
    char *name = extract_message(buff);
    printf("name: [%s]\n", name);
    player_t *player = add_player(name);
    if (player == NULL) {
        printf("could not init player\n");
        pthread_exit(NULL);
    }
    player->name = name;
    printf("new player: name = [%s], id = [%d]\n", player->name, player->id);
    fputs("N 10\n", io);
    fflush(io);
    fputs("Z (40,20)\n", io);
    fflush(io);
    fputs("asdf\n", io);
    fflush(io);
    printf("sent map\n");
    /*printf("map[%s]\n", state.map);
    char b[400];
    snprintf(b, 400, "map:%s\n", "yeezus");
    fputs(b, io);
    fflush(io);
    */
}

int transfer_initial_data(FILE *io) {
    
    return 0;
}

player_t *add_player(char *name) {
    player_t *ret = NULL;

    pthread_mutex_lock(&state.mutex);
    if (state.num_players != state.max_players) {
        state.players[state.num_players - 1].id = state.num_players;
        ret = &state.players[state.num_players - 1];
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

