#ifndef _ASYNC_MESSAGES_H
#define _ASYNC_MESSAGES_H

/*
    All network communication takes place on a line per line basis.
    Lines start with a single character identifying the format.
*/

/* C -> S: players tells server its name
 * > N cool name
 */
#define H_NAME 'N'

/* S -> C: server gives client an id
 * > I 12
 */
#define H_ID 'I'

/* S -> C: message from the server
 * > L player one killed player two
 */
#define H_LOG 'L'

/* C -> S: movement of player
 * > M (12,13)
 */
#define H_MOVE 'M'


#endif
