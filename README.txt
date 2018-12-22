
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
    To move, press a directional key (WASD) and to shoot press (SPACE). There
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

