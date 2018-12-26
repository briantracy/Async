
    +-----------------------------------+
    | async -- the command line shooter |
    +-----------------------------------+

30,000ft:
    async is a multi-player command line game.

20,000ft:
    While playing, you control a little ship on a two dimensional battlefield.
    You can play against any number of opponents who are connected to the 
    same server as you. The goal is to be the last ship alive.

15,000ft:
    To move, press a directional key (WASD) and to shoot press J. There
    is no ammo limit, but there is a firing cooldown. The shots you fire are
    called beams and they are a one hit kill against enemy ships.

10,000ft:
    You cannot see your opponents at all times, however, once per second the
    server will send you their location and it will appear on your screen.

5,000ft:
    Your beams (and others) travel through walls in a straight line until they
    hit the outer boundary. Beams are emitted from the tip of your ship in the
    direction in which you are facing.

2,000ft:
    Here is a table of things you will encounter:

        +--------+----------------------+
        | Entity | Description          |
        +--------+----------------------+
        | >      | Ship facing east     |
        | <      | Ship facing west     |
        | ^      | Ship facing north    |
        | v      | Ship facing south    |
        | #      | Wall                 |
        | @      | Cloak                |
        | *      | Enemy player         |
        | ------ | Beam (left to right) |
        +--------+----------------------+

1,000ft:
    If you obtain a cloak, your location will not be sent to the server for a
    short duration. This means others cannot see you. However, beams will always
    show up when you fire them.

500ft:
    To play, you must launch a server, then point as many clients as you want at it.

10ft:
    async is written in c99 and requires no external dependencies except what you would
    usually find on any linux install (ncurses, pthread, arpanet).

2ft:
    To start an async-server, run async-server and specify the number of players, the port
    on which you want to accept clients, and the size of the arena. Then players can point
    their clients at this server to join.

0ft:
    Now that you know the rules and how to play, see below for the technical details on how
    to get async and async-server up and running.


    +-------------------------------------+
    | Technical Details - Getting Started |
    +-------------------------------------+

1. Compile the source

Implementation
==============
Client server handshake is as follows. 
C->S: N <player name> (player tells server its name)
S->C: I <players unique id> (server gives player a unique id)
S->C: Z <map size> (server gives player the map size)
S->C: <raw map data> (server gives palayer map data)

If the game is full, or something else goes wrong, the server will let the client
know when it sends back the id.
