#include <stdio.h>
#include <assert.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

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
    (void)listener;
    pause();
    return 0;
}

// This runs on the (1) listener thread.
void player_joined(FILE *io) {
    if (state.num_players >= state.max_players) {
        fprintf(stderr, "async-server: player refused (too many players)\n");
        fputs(SERVER_ERROR, io);
        fflush(io);
        fclose(io);
        return;
    }
    const int name_buff_len = NAME_LEN + 4;
    char nbuff[name_buff_len];
    fgets(nbuff, name_buff_len, io);

    player_t *p = add_player(extract_message(nbuff), io);
    printf("async-server: player joined (`%s[%d]`)\n", p->name, p->id);

    int s = pthread_create(&p->thread, NULL, &run_player, (void *)p);
    if (s != 0) {
        fprintf(stderr, "async-server: could not create player thread\n");
    }
}
void *run_player(void *arg) {
    player_t *player = (player_t *)arg;

    const int id_buff_len = ID_LEN + 4;
    char id_buff[id_buff_len];
    snprintf(id_buff, id_buff_len, "%c %d\n", H_ID, player->id);
    fputs(id_buff, player->io);
    fflush(player->io);

    const int size_buff_len = SIZE_LEN + 4;
    char size_buff[size_buff_len];
    snprintf(size_buff, size_buff_len, "%c (%d,%d)\n", H_SIZE, state.width, state.height);
    fputs(size_buff, player->io);
    fflush(player->io);

    // 1 char per map cell + newline + '\0'
    const size_t map_buff_size = state.width * state.height + 2;
    char map_buff[map_buff_size];
    snprintf(map_buff, map_buff_size, "%s\n", state.map);
    fputs(map_buff, player->io);
    fflush(player->io);

    char msg[100];
    while (fgets(msg, 100, player->io) != NULL) {
        respond(player, msg);
    }
    printf("async-server: player `%s[%d]` disconnected\n", player->name, player->id);
    fflush(player->io);
    fclose(player->io);
    return NULL;
}

void respond(player_t *p, char *cmd) {
    char *msg = extract_message(cmd);
    printf("responding to `%s`\n", msg);
    switch (cmd[0]) {
    case H_MOVE: {
        sscanf(msg, "(%d,%d)", &p->loc.x, &p->loc.y);
        printf("player loc: [%d], (%d,%d)\n", p->id, p->loc.x, p->loc.y);
        break;
    }
    case H_FIRE: {
        unsigned int x, y;
        direction_t d;
        sscanf(msg, "(%d,%d)@%d", &x, &y, &d);
        printf("shooting beam in dir: %d from (%d,%d)\n", d, x, y);
    }
    }
    publish_positions();
}

player_t *add_player(char *name, FILE *io) {

    player_t *ret = &state.players[state.num_players];
    ret->id = state.num_players;
    ret->name = malloc(sizeof(char) * strlen(name) + 1);
    strcpy(ret->name, name);
    ret->io = io;

    state.num_players++;
    return ret;
}

void remove_player(player_t *p) {
    (void)p;
}

void *publisher(void *arg) {
    (void)arg;
    while (1) {
        sleep(1);
        publish_positions();
    }
}

void publish_positions() {
    printf("publishing locations\n");
    for (int i = 0; i < state.num_players; i++) {
        player_t *p = &state.players[i];
        pthread_mutex_lock(&p->stream_mutex);
        fputs("P (12,12)\n", p->io);
        fflush(p->io);
        pthread_mutex_unlock(&p->stream_mutex);
    }

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
    
    srand(time(NULL));
    for (int x = 0; x < state.width; x++) {
        for (int y = 0; y < state.height; y++) {
            if (rand() % 100 < 15) {
                set_char(state.map, x, y, state.width, '#');
            }
        }
    }
    

    //print_map(state.map, state.width, state.height);
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

