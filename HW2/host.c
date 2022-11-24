#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc,char *argv[]){
    int host_id,depth,lucky_number;
    for (int i = 0; i < 3; i++){
        int flag = getopt(argc,argv,"m:d:l:");
        switch (flag){
            case 'm':
            host_id = atoi(optarg);
            break;
            case 'd':
            depth = atoi(optarg);
            break;
            case 'l':
            lucky_number = atoi(optarg);
            break;
            default:
            fprintf(stderr,"flag broke");    
            break;
        }
    }
    //printf("%d %d %d\n",host_id,depth,lucky_number);
    //exit(0);
    if (depth == 0){
        char forcommandline[3][128];
        int fd1[2],fd2[2],fd3[2],fd4[2];
        pipe(fd1);pipe(fd2);pipe(fd3);pipe(fd4);
        FILE *readchildfile_left = fdopen(fd3[0],"r");
        FILE *readchildfile_right = fdopen(fd4[0],"r");
        for (int i = 0; i < 3; i++){
            switch (i){
                case 0:
                sprintf(forcommandline[i],"%d",host_id);
                break;
                case 1:
                sprintf(forcommandline[i],"%d",(depth+1));
                break;
                case 2:
                sprintf(forcommandline[i],"%d",lucky_number);
                break;
            }
        }
        if (fork() == 0){
            dup2(fd1[0],0); 
            dup2(fd3[1],1);
            close(fd1[1]); close(fd3[0]);
            close(fd2[0]); close(fd2[1]);
            close(fd4[0]); close(fd4[1]);
            //execlp
            execlp("./host","./host","-m",forcommandline[0],"-d",forcommandline[1],"-l",forcommandline[2],NULL);

        }
        if (fork() == 0){
            dup2(fd2[0],0); 
            dup2(fd4[1],1);
            close(fd2[1]); close(fd4[0]);
            close(fd1[0]); close(fd1[1]);
            close(fd3[0]); close(fd3[1]);
            //execlp
            execlp("./host","./host","-m",forcommandline[0],"-d",forcommandline[1],"-l",forcommandline[2],NULL);
        }    
        while(1){
            char readin[128];
            FILE *openfile;
            sprintf(readin,"fifo_%d.tmp",host_id);
            openfile = fopen(readin,"r");
            FILE *writeinfile = fopen("fifo_0.tmp","a+");
            //becareful no file exist!!!
            int readplayer;
            int allID[8];//record 8 ids in this game
            int checkcheckhaha = 0;
            for (int i = 1; i <= 4; i++){
                fscanf(openfile,"%d",&readplayer);
                allID[i-1] = readplayer;
                if (i != 4)
                dprintf(fd1[1],"%d ",readplayer);
                else 
                dprintf(fd1[1],"%d\n",readplayer);
            }
            for (int i = 1; i <= 4; i++){
                fscanf(openfile,"%d",&readplayer);
                allID[i+3] = readplayer;
                if (i != 4)
                dprintf(fd2[1],"%d ",readplayer);
                else 
                dprintf(fd2[1],"%d\n",readplayer);
            }
            if (readplayer != -1){
                //read child
                int child_id[10][2],guess_num[10][2];
                for (int i = 0; i < 10; i++){
                    fscanf(readchildfile_left,"%d %d",&child_id[i][0],&guess_num[i][0]);
                    fscanf(readchildfile_right,"%d %d",&child_id[i][1],&guess_num[i][1]);
                }
                
                int finalscore[13]={0};
                for (int i = 0; i < 10; i++){
                    if (abs(guess_num[i][0]-lucky_number) <= abs(guess_num[i][1]-lucky_number)){
                        finalscore[child_id[i][0]]+=10;
                    }
                    else{
                        finalscore[child_id[i][1]]+=10;
                    }
                }
                //send to bash script
                fprintf(writeinfile,"%d\n",host_id);
                for (int i = 0; i < 8; i++){
                    int IDforwrite = allID[i];
                    int writepoint = finalscore[IDforwrite];
                    fprintf(writeinfile,"%d %d\n",IDforwrite,writepoint);
                }
                fflush(writeinfile);
            }
            else{
                fclose(openfile);fclose(writeinfile);
                break;
            }
            fclose(openfile);fclose(writeinfile);  
        }
        //wait
        wait(NULL); wait(NULL);
        //close
        close(fd1[0]);close(fd1[1]);close(fd2[0]);close(fd2[1]);
        close(fd3[0]);close(fd3[1]);close(fd4[0]);close(fd4[1]);
        fclose(readchildfile_left); fclose(readchildfile_right);
        exit(0);
    }
    else if (depth == 1){
        int fd1[2],fd2[2],fd3[2],fd4[2];
        pipe(fd1);pipe(fd2);pipe(fd3);pipe(fd4);
        FILE *readchildfile_left = fdopen(fd3[0],"r");
        FILE *readchildfile_right = fdopen(fd4[0],"r");
        char forcommandline[3][128];
        for (int i = 0; i < 3; i++){
            switch (i){
                case 0:
                sprintf(forcommandline[i],"%d",host_id);
                break;
                case 1:
                sprintf(forcommandline[i],"%d",(depth+1));
                break;
                case 2:
                sprintf(forcommandline[i],"%d",lucky_number);
                break;
            }
        }
        if (fork() == 0){
            dup2(fd1[0],0); 
            dup2(fd3[1],1);
            close(fd1[1]); close(fd3[0]);
            close(fd2[0]); close(fd2[1]);
            close(fd4[0]); close(fd4[1]);
            //execlp
            execlp("./host","./host","-m",forcommandline[0],"-d",forcommandline[1],"-l",forcommandline[2],NULL);

        }
        if (fork() == 0){
            dup2(fd2[0],0); 
            dup2(fd4[1],1);
            close(fd2[1]); close(fd4[0]);
            close(fd1[0]); close(fd1[1]);
            close(fd3[0]); close(fd3[1]);
            //execlp
            execlp("./host","./host","-m",forcommandline[0],"-d",forcommandline[1],"-l",forcommandline[2],NULL);
        }
        while(1){
            int readplayer;
            //record 8 ids in this game
            //fprintf(stderr,"0\n");
            for (int i = 1; i <= 2; i++){
                scanf("%d",&readplayer);
                if (i != 2)
                dprintf(fd1[1],"%d ",readplayer);
                else 
                dprintf(fd1[1],"%d\n",readplayer);
            }
            for (int i = 1; i <= 2; i++){
                scanf("%d",&readplayer);
                if (i != 2)
                dprintf(fd2[1],"%d ",readplayer);
                else 
                dprintf(fd2[1],"%d\n",readplayer);
            }
            //read child
            if (readplayer != -1){
                int child_id[10][2],guess_num[10][2];
                //sleep(10);
                for (int i = 0; i < 10; i++){
                    //fprintf(stderr,"aloha\n");
                    fscanf(readchildfile_left,"%d %d",&child_id[i][0],&guess_num[i][0]);
                    fscanf(readchildfile_right,"%d %d",&child_id[i][1],&guess_num[i][1]);
                    //fprintf(stderr,"%d %d %d %d\n",child_id[i][0],guess_num[i][0],child_id[i][1],guess_num[i][1]);
                }
                //print to stdin
                for (int i = 0; i < 10; i++){
                    if (abs(guess_num[i][0]-lucky_number) <= abs(guess_num[i][1]-lucky_number)){
                        printf("%d %d\n",child_id[i][0],guess_num[i][0]);
                    }
                    else{
                        printf("%d %d\n",child_id[i][1],guess_num[i][1]);
                    }
                }
                fflush(stdout);
            }
            else{
                break;
            } 
                     
        }   
        //wait
        wait(NULL); wait(NULL);    
        //close
        close(fd1[0]);close(fd1[1]);close(fd2[0]);close(fd2[1]);
        close(fd3[0]);close(fd3[1]);close(fd4[0]);close(fd4[1]);
        fclose(readchildfile_left); fclose(readchildfile_right); 
        exit(0);
    }
    else if (depth == 2){
        while(1){
            int readplayer[2];
            //record 8 ids in this game
            scanf("%d %d",&readplayer[0],&readplayer[1]);
            /*debug*/
            //fprintf(stderr,"%d %d\n",readplayer[0],readplayer[1]);
            if (readplayer[0] == -1 && readplayer[1] == -1) break;
            int fd1[2],fd2[2];
            pipe(fd1);pipe(fd2);
            char forcommandline[2][128];
            sprintf(forcommandline[0],"%d",readplayer[0]);
            sprintf(forcommandline[1],"%d",readplayer[1]);
            if (fork() == 0){
                dup2(fd1[1],1); 
                close(fd1[0]); 
                close(fd2[0]); close(fd2[1]);
                //execlp
                execlp("./player","./player","-n",forcommandline[0],NULL);
            }
            if (fork() == 0){
                dup2(fd2[1],1); 
                close(fd2[0]);
                close(fd1[0]); close(fd1[1]);
                //execlp
                execlp("./player","./player","-n",forcommandline[1],NULL);
            }
            //read child
            FILE *readchildfile_left = fdopen(fd1[0],"r");
            FILE *readchildfile_right = fdopen(fd2[0],"r");
            int child_id[10][2],guess_num[10][2];
            for (int i = 0; i < 10; i++){
                fscanf(readchildfile_left,"%d %d",&child_id[i][0],&guess_num[i][0]);
                fscanf(readchildfile_right,"%d %d",&child_id[i][1],&guess_num[i][1]);
                //fprintf(stderr,"%d %d %d %d\n",child_id[i][0],guess_num[i][0],child_id[i][1],guess_num[i][1]);
            }
            //wait
            wait(NULL); wait(NULL);
            //print to stdin
            for (int i = 0; i < 10; i++){
                if (abs(guess_num[i][0]-lucky_number) <= abs(guess_num[i][1]-lucky_number)){
                    //fprintf(stderr,"%d %d\n",child_id[i][0],guess_num[i][0]);
                    printf("%d %d\n",child_id[i][0],guess_num[i][0]);
                }
                else{
                    //fprintf(stderr,"%d %d\n",child_id[i][1],guess_num[i][1]);
                    printf("%d %d\n",child_id[i][1],guess_num[i][1]);
                }
                
            }
            fflush(stdout);
            //close
            close(fd1[0]);close(fd1[1]);close(fd2[0]);close(fd2[1]);
            fclose(readchildfile_left); fclose(readchildfile_right);
        }
        exit(0);
    }   
}