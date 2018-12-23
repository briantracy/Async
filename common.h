#ifndef _ASYNC_COMMON_H
#define _ASYNC_COMMON_H


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
    int x, y;
} point_t;


typedef enum {
    NORTH = 0,
    SOUTH = 1,
    EAST  = 2,
    WEST  = 3
} direction_t;


#endif /* _ASYNC_COMMON_H */
