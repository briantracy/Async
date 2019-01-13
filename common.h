#ifndef _COMMON_H
#define _COMMON_H

/*
    Origin of the frame is top left.

    (0,0) ----------> (10, 0)
     |
     |
     |
     V
    (0, 10)
*/
typedef struct {
    unsigned int x, y;
} point_t;


typedef enum {
    NORTH = 0,
    SOUTH = 1,
    EAST  = 2,
    WEST  = 3
} direction_t;


/* Map is layed out in row major order. */
void set_char(char *map, int x, int y, int width, char c) {
    map[y * width + x] = c;
}

char get_char(char *map, int x, int y, int width) {
    return map[y * width + x];
}

void print_map(char *map, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%c", map[y * width + x]);
        }
        printf("\n");
    }
}

#endif
