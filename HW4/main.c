#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>
int row,column,epoch; //row column no contain border
bool **plate; //for game
bool **duplicateplate; //for copy to
typedef struct workpart{ //contain [,]
    int beginrow;
    int endrow;
    int begincolumn;
    int endcolumn;
}Workpart;
int processpartition(void){
    return 0;
}
void* dealthread(void* vars){
    Workpart *needtowork = (Workpart*)vars;
    for (int i = needtowork->beginrow; i <= needtowork->endrow; i++){
        for (int j = needtowork->begincolumn; j <= needtowork->endcolumn; j++){
            int count = 0;
            for (int a = i-1; a <= i+1; a++){
                for (int b = j-1; b <= j+1; b++){
                    if (!(a == i && b == j)){
                        if (plate[a][b] == true) count++;
                    }
                }
            }
            if (plate[i][j] == true){ // live
                if (count == 2 || count == 3){
                    duplicateplate[i][j] = true;
                }
                else duplicateplate[i][j] = false;
            }
            else { //die
                if (count == 3){
                    duplicateplate[i][j] = true;
                }
                else duplicateplate[i][j] = false;
            }
        }
    }
    pthread_exit(NULL);
}
int dealprocess(void){
    return 0;
}
int main(int argc,char *argv[]){
    bool isthread;
    int threadcount;
    FILE *inputfile,*outputfile;
    if (argv[1][1] == 't') isthread = true;
    else if (argv[1][1] == 'p') isthread = false;
    else perror("Neither thread nor process\n");
    threadcount = atoi(argv[2]);
    inputfile = fopen(argv[3],"r");
    outputfile = fopen(argv[4],"w+");
    fscanf(inputfile,"%d %d %d\n",&row,&column,&epoch);
    char putinstr[column+2];
    //malloc 2D array
    plate = (bool**)malloc((row+2)*sizeof(bool*));
    duplicateplate = (bool**)malloc((row+2)*sizeof(bool*));
    for (int i = 0; i < row+2; i++){
        plate[i] = (bool*)malloc((column+2)*sizeof(bool));
        duplicateplate[i] = (bool*)malloc((column+2)*sizeof(bool));
    } 
    //malloc 2D array
    //deal with input
    for (int i = 0; i < column+2; i++){
        plate[0][i] = false;
        duplicateplate[0][i] = false;
    }
    for (int i = 1; i < row+1; i++){
        plate[i][0] = false;
        duplicateplate[i][0] = false;
        fscanf(inputfile,"%s",putinstr);
        for (int j = 1; j < column+1; j++){
            if (putinstr[j-1] == 'O'){
                plate[i][j] = true;
            }
            else if (putinstr[j-1] == '.'){
                plate[i][j] = false;
            }
        }
        plate[i][column+1] = false;
        duplicateplate[i][column+1] = false;
    }
    for (int i = 0; i < column+2; i++){
        plate[row+1][i] = false;
        duplicateplate[row+1][i] = false;
    }
    Workpart *workingla = (Workpart*)malloc(sizeof(Workpart)*threadcount);
    int realworkingthreadcount = 0;
    if (isthread == true){
        if (row > column){
            int row_per = row/threadcount,rowmod = row%threadcount;
            int Rowpart[threadcount][2];
            if (row_per == 0){
                realworkingthreadcount = row;
                for (int i = 0; i < realworkingthreadcount; i++){
                    Rowpart[i][0] = i+1; Rowpart[i][1] = i+1;
                }
                for (int i = realworkingthreadcount; i < threadcount; i++){
                    Rowpart[i][0] = 2; Rowpart[i][1] = 1;
                }
            }
            else{
                realworkingthreadcount = threadcount;
                int modcount = 0;
                int acount = 1;
                for (int i = 0; i < threadcount; i++){
                    if (modcount < rowmod){
                        Rowpart[i][0] = acount; Rowpart[i][1] = acount+row_per;
                        modcount++;
                        acount += row_per+1;
                    }
                    else{
                        Rowpart[i][0] = acount; Rowpart[i][1] = acount+row_per-1;
                        acount += row_per;
                    }
                } 
            }
            for (int i = 0; i < threadcount; i++){
                workingla[i].begincolumn = 1;
                workingla[i].endcolumn = column;
                workingla[i].beginrow = Rowpart[i][0];
                workingla[i].endrow = Rowpart[i][1];
            }
        }
        else{
            int col_per = column/threadcount,colmod = column%threadcount;
            int Colpart[threadcount][2];
            if (col_per == 0){
                realworkingthreadcount = column;
                for (int i = 0; i <realworkingthreadcount; i++){
                    Colpart[i][0] = i+1; Colpart[i][1] = i+1;
                }
                for (int i = realworkingthreadcount; i < threadcount; i++){
                    Colpart[i][0] = 2; Colpart[i][1] = 1;
                }
            }
            else{
                realworkingthreadcount = threadcount;
                int modcount = 0;
                int acount = 1;
                for (int i = 0; i < threadcount; i++){
                    if (modcount < colmod){
                        Colpart[i][0] = acount; Colpart[i][1] = acount+col_per;
                        modcount++;
                        acount += col_per+1;
                    }
                    else{
                        Colpart[i][0] = acount; Colpart[i][1] = acount+col_per-1;
                        acount += col_per;
                    }
                }
            }
            for (int i = 0; i < threadcount; i++){
                workingla[i].begincolumn = Colpart[i][0];
                workingla[i].endcolumn = Colpart[i][1];
                workingla[i].beginrow = 1;
                workingla[i].endrow = row;
                /*printf("%d %d\n",Colpart[i][0],Colpart[i][1]);*/
            }
        }
    }
    else if (isthread == false) processpartition();  //later deal
    if (isthread == true){
        pthread_t threadarr[threadcount];
        for (int i = 0; i < epoch; i++){
            for (int j = 0; j < threadcount; j++){
                pthread_create(&threadarr[j],NULL,dealthread,(void*)&workingla[j]);
            }
            for (int j = 0; j < threadcount; j++){
                pthread_join(threadarr[j],NULL);
            }
            bool **tempplate;
            tempplate = plate;
            plate = duplicateplate;
            duplicateplate = tempplate;
        }
    }
    else if (isthread == false) dealprocess();  //later deal
    //write in
    rewind(outputfile);
    for (int i = 1; i <= row; i++){
        for (int j = 1; j <= column; j++){
            if (plate[i][j] == true) fputc('O',outputfile);
            else fputc('.',outputfile);
        }
        if (i != row) fputc('\n',outputfile);
    }
    //end recycle
    for (int i = 0; i < row+2; i++){
        free(plate[i]);
        free(duplicateplate[i]);
    }
    free(plate);
    free(duplicateplate);
    fclose(inputfile);
    fclose(outputfile);
    free(workingla);
    return 0;
}