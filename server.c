#include <stdio.h>
#include <assert.h>

#include "server.h"
#include "common.h"



int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: async-server <port> <n-players>\n");
    }
    return 0;
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
