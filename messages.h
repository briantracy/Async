#ifndef _ASYNC_MESSAGES_H
#define _ASYNC_MESSAGES_H



char *extract_message(char *raw) {
    char *n = strchr(raw, '\n');
    if (n != NULL) {
        *n = '\0';
    }
    return &raw[2];
}


/*
    All network communication takes place on a line per line basis.
    Lines start with a single character identifying the format.
*/

/* C -> S: player tells server its name. A player's name is <= 16 chars
           in length.
 * > N cool name
 */
#define H_NAME 'N'
#define NAME_LEN 16

/* S -> C: server gives client an id. Unique Id's are integers.
 * > I 12
 */
#define H_ID 'I'
#define ID_LEN 8


/* S -> C: message from the server
 * > L player one killed player two
 */
#define H_LOG 'L'
#define LOG_LEN 64

/* C -> S, S -> C: chat message
 * > C play again?
 */
#define H_CHAT 'C'
#define CHAT_LEN 64

/* C -> S: movement of player
 * > M (12,13)
 */
#define H_MOVE 'M'
#define MOVE_LEN 16

#define H_POSITION 'P'
// (xxx,yyy) repeated ~ 20 times = 256
#define POSITION_LEN 256

#define H_SIZE 'Z'
#define SIZE_LEN 16

#define H_FIRE 'F'
// (xxx,yyy) N\n
#define FIRE_LEN 32

#define SERVER_ERROR "E err\n"

#endif
