#include <stdio.h>
#include <unistd.h>
#include <curses.h>



int main(int argc, char *argv[]) {
    
    initscr();
    noecho();
    curs_set(FALSE);

    while (1) {
        mvaddstr(3, 3, "getting key\n");
        int k = getch();
        printf("k = %c\n", (char)k);
        if (k == 'a') break;
        sleep(1);
    }

/*char c = 'a';
while (1) {
    int k = getch();
    printf("%c\n", k);
    return 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            mvaddch(i, j, c);
        }
    }
c++;
refresh();
sleep(1);
}   
    mvaddstr(12, 12, "standard error");   
*/
    refresh();


   // pause();
    endwin();
    printf("done\n");
    return 0;
}
