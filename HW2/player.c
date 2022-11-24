#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(int argc,char *argv[]){
    /*change to getopt nono*/
    char player[128];
    /*if (strcmp(argv[1],"-n") != 0) fprintf(stderr,"%s","flag wrong condition!\n");*/
    strcpy(player,argv[2]);
    int player_id = atoi(player);
    for (int round = 1; round <= 10; round++){
        int guess;
        srand ((player_id + round) * 323);
        guess = rand() % 1001;
        printf("%d %d\n",player_id,guess);
        fflush(stdout);
    }
    exit(0);
}