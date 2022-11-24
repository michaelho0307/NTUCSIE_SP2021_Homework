#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)
/*int fileno(FILE *stream);*/

typedef struct {
    char hostname[512];  // server's hostname
    unsigned short port;  // port to listen
    int listen_fd;  // fd to wait for a new connection
} server;

typedef struct {
    char host[512];  // client's host
    int conn_fd;  // fd to talk with client
    char buf[512];  // data sent by/to client
    size_t buf_len;  // bytes used by buf
    // you don't need to change this.
    int id;
    int wait_for_write;  // used by handle_read to know if the header is read or not.
} request;

server svr;  // server
request* requestP = NULL;  // point to a list of requests
int maxfd;  // size of open file descriptor table, size of request list

const char* accept_read_header = "ACCEPT_FROM_READ";
const char* accept_write_header = "ACCEPT_FROM_WRITE";

static void init_server(unsigned short port);
// initailize a server, exit for error

static void init_request(request* reqP);
// initailize a request instance

static void free_request(request* reqP);
// free resources used by a request instance

typedef struct {
    int id;          //902001-902020
    int AZ;          
    int BNT;         
    int Moderna;     
}registerRecord;

int handle_read(request* reqP) {
    int r;
    char buf[512];

    // Read in request from client
    r = read(reqP->conn_fd, buf, sizeof(buf));
    if (r < 0) return -1;
    if (r == 0) return 0;
    char* p1 = strstr(buf, "\015\012");
    int newline_len = 2;
    if (p1 == NULL) {
       p1 = strstr(buf, "\012");
        if (p1 == NULL) {
            ERR_EXIT("this really should not happen...");
        }
    }
    size_t len = p1 - buf + 1;
    memmove(reqP->buf, buf, len);
    reqP->buf[len - 1] = '\0';
    reqP->buf_len = len-1;
    return 1;
}

int main(int argc, char** argv) {

    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in cliaddr;  // used by accept()
    int clilen;

    int conn_fd;  // fd for a new connection with client
    int file_fd;  // fd for file that we open for reading
    char buf[512];
    int buf_len;
    int sameprocesslock[100]={0};
    
    // Initialize server
    init_server((unsigned short) atoi(argv[1]));

    // Loop for handling connections
    fprintf(stderr, "\nstarting on %.80s, port %d, fd %d, maxconn %d...\n", svr.hostname, svr.port, svr.listen_fd, maxfd);
    int register_fd = open("registerRecord",O_RDWR);
    /*init fd_set*/    
    fd_set master,copy;
    FD_ZERO(&master);
    FD_SET(svr.listen_fd,&master);
    int current_fd = svr.listen_fd;
    maxfd = 4;
    while (1) {
        // TODO: Add IO multiplexing
        current_fd = svr.listen_fd;
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = 200;
        memcpy(&copy,&master,sizeof(master));
        if (select(maxfd+1,&copy,NULL,NULL,NULL) > 0){
            if (FD_ISSET(current_fd,&copy)){
                // Check new connection
                clilen = sizeof(cliaddr);
                conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
                if (conn_fd < 0) {
                    if (errno == EINTR || errno == EAGAIN) continue;  // try again
                    if (errno == ENFILE) {
                        (void) fprintf(stderr, "out of file descriptor table ... (maxconn %d)\n", maxfd);
                        continue;
                    }
                    ERR_EXIT("accept");
                }
                requestP[conn_fd].wait_for_write = 0;
                requestP[conn_fd].conn_fd = conn_fd;
                strcpy(requestP[conn_fd].host, inet_ntoa(cliaddr.sin_addr));
                fprintf(stderr, "getting a new request... fd %d from %s\n", conn_fd, requestP[conn_fd].host);
                dprintf(requestP[conn_fd].conn_fd,"Please enter your id (to check your preference order):\n");
                if (requestP[conn_fd].conn_fd > maxfd) maxfd = requestP[conn_fd].conn_fd;
                FD_SET(requestP[conn_fd].conn_fd,&master); //SET FD
                memcpy(&copy,&master,sizeof(master));
            }
            else{
                for (int i = 5; i <= maxfd; i++){
                    if (FD_ISSET(i,&copy)){
                         //for write server
                        
                        /*requestP[i].buf_len = 0;*/
                        int ret = handle_read(&requestP[i]); // parse data from client to requestP[conn_fd].buf
                        fprintf(stderr, "ret = %d\n", ret);
                        if (ret < 0) {
                            fprintf(stderr, "bad request from %s\n", requestP[i].host);
                            continue;
                        }
                    }
                }
            }            
        }
        /*else{
            perror("hahaha");
        }*/
        // TODO: handle requests from clients
#ifdef READ_SERVER
        for (int i = 5; i <= maxfd; i++){
            if (requestP[i].conn_fd && requestP[i].buf_len){
                int student_id;
                char* student_id_len=requestP[i].buf;/* = requestP[i].buf;*/ //if 超過 buffer？？？ dont worry
                char vaccine[3][10];int vaccine_rank[3];/*AZ BNT Moderna*/
                registerRecord Myrecord;
                if (strlen(student_id_len) == 6){
                    student_id = atoi(student_id_len);
                    if (student_id >= 902001 && student_id <= 902020){ /*補充lock*/
                        struct flock readflock;
                        readflock.l_start = (student_id-902001)*16;
                        readflock.l_len = 16;
                        readflock.l_type = F_RDLCK;
                        readflock.l_whence = SEEK_SET;
                        int check_read = 1;
                        check_read = fcntl(register_fd,F_SETLK,&readflock); /*unlock？？*/
                        if (check_read >= 0){
                            /*read record*/
                            lseek(register_fd,sizeof(registerRecord)*(student_id-902001),SEEK_SET);
                            read(register_fd,&Myrecord,sizeof(registerRecord));
                            /*read record*/
                            int check_valid[3]={0};
                            vaccine_rank[0] = Myrecord.AZ; vaccine_rank[1] = Myrecord.BNT; vaccine_rank[2] = Myrecord.Moderna;
                            for (int j = 0; j < 3; j++){
                                if (vaccine_rank[0] == j+1) {strcpy(vaccine[j],"AZ");check_valid[j] = 1;}
                                else if (vaccine_rank[1] == j+1) {strcpy(vaccine[j],"BNT");check_valid[j] = 1;}
                                else if (vaccine_rank[2] == j+1) {strcpy(vaccine[j],"Moderna");check_valid[j] = 1;}    
                            }
                            int checkcheck = 0;
                            for (int j = 0; j < 3; j++){
                                if (check_valid[j] == 0){
                                    dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                                    readflock.l_type = F_UNLCK;
                                    fcntl(register_fd,F_SETLK,&readflock);
                                    FD_CLR(requestP[i].conn_fd,&master);
                                    close(requestP[i].conn_fd);
                                    free_request(&requestP[i]);
                                    checkcheck = 1;
                                    break;
                                }
                            }
                            if (checkcheck == 1) continue;
                            dprintf(requestP[i].conn_fd,"Your preference order is %s > %s > %s.\n",vaccine[0],vaccine[1],vaccine[2]);
                            /*fprintf(stderr, "%s", requestP[i].buf);
                            sprintf(buf,"%s : %s",accept_read_header,requestP[i].buf);
                            write(requestP[i].conn_fd, buf, strlen(buf));*/
                            readflock.l_type = F_UNLCK;
                            fcntl(register_fd,F_SETLK,&readflock);
                            FD_CLR(requestP[i].conn_fd,&master);    
                            close(requestP[i].conn_fd);
                            free_request(&requestP[i]);
                            continue;
                        }
                        else{
                            dprintf(requestP[i].conn_fd,"Locked.\n");
                            FD_CLR(requestP[i].conn_fd,&master);
                            close(requestP[i].conn_fd);
                            free_request(&requestP[i]);
                            continue;
                        }
                    }
                    else{
                        dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                        FD_CLR(requestP[i].conn_fd,&master);
                        close(requestP[i].conn_fd);
                        free_request(&requestP[i]);
                        continue;
                    }
                }
                else{
                    dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                    FD_CLR(requestP[i].conn_fd,&master);
                    close(requestP[i].conn_fd);
                    free_request(&requestP[i]);
                }
            }
        }
/*注意i j*/
/*index connfd 改成 i*/
#elif defined WRITE_SERVER
        for (int i = 5; i <= maxfd; i++){
            if (requestP[i].conn_fd && requestP[i].buf_len && requestP[i].wait_for_write == 0){
                struct flock writeflock;
                //處理一下
                int student_id;
                registerRecord Myrecord;
                char vaccine[3][10];
                char *student_id_len=requestP[i].buf; /*= requestP[i].buf;*/ //if 超過 buffer？？？ dont worry
                int vaccine_rank[3];/*AZ BNT Moderna*/
                if (strlen(student_id_len) == 6){
                    student_id = atoi(student_id_len);
                    if (student_id >= 902001 && student_id <= 902020){ /*補充lock*/
                        /*上lock*/
                        if (sameprocesslock[student_id-902001] == 1){
                            dprintf(requestP[i].conn_fd,"Locked.\n");
                            FD_CLR(requestP[i].conn_fd,&master);
                            close(requestP[i].conn_fd);
                            free_request(&requestP[i]);
                            continue;
                        }
                        else{
                            sameprocesslock[student_id-902001] = 1;
                            writeflock.l_start = (student_id-902001)*16;
                            writeflock.l_len = 16;
                            writeflock.l_type = F_WRLCK;
                            writeflock.l_whence = SEEK_SET;
                            int check_write = 1;
                            check_write = fcntl(register_fd,F_SETLK,&writeflock);
                            if (check_write >= 0){
                                /*seek position and write*/
                                lseek(register_fd,sizeof(registerRecord)*(student_id-902001),SEEK_SET);
                                read(register_fd,&Myrecord,sizeof(registerRecord));
                                int check_valid[3]={0};
                                vaccine_rank[0] = Myrecord.AZ; vaccine_rank[1] = Myrecord.BNT; vaccine_rank[2] = Myrecord.Moderna;
                                for (int j = 0; j < 3; j++){
                                    if (vaccine_rank[0] == j+1) {strcpy(vaccine[j],"AZ");check_valid[j] = 1;}
                                    else if (vaccine_rank[1] == j+1) {strcpy(vaccine[j],"BNT");check_valid[j] = 1;}
                                    else if (vaccine_rank[2] == j+1) {strcpy(vaccine[j],"Moderna");check_valid[j] = 1;}    
                                }
                                for (int j = 0; j < 3; j++){
                                    if (check_valid[j] == 0){
                                        dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                                        FD_CLR(requestP[i].conn_fd,&master);
                                        close(requestP[i].conn_fd);
                                        free_request(&requestP[i]);
                                        writeflock.l_start = (student_id-902001)*16;
                                        writeflock.l_len = 16;
                                        writeflock.l_type = F_UNLCK;
                                        writeflock.l_whence = SEEK_SET;
                                        fcntl(register_fd,F_SETLK,&writeflock);
                                        sameprocesslock[student_id-902001] = 0;
                                        break;
                                    }
                                    if (j == 2){
                                        dprintf(requestP[i].conn_fd,"Your preference order is %s > %s > %s.\n",vaccine[0],vaccine[1],vaccine[2]);
                                        dprintf(requestP[i].conn_fd,"Please input your preference order respectively(AZ,BNT,Moderna):\n");
                                        requestP[i].wait_for_write = student_id;
                                    }
                                }
                                /*int ret = handle_read(&requestP[conn_fd]);*/
                            }
                            else{
                                dprintf(requestP[i].conn_fd,"Locked.\n");
                                FD_CLR(requestP[i].conn_fd,&master);
                                close(requestP[i].conn_fd);
                                free_request(&requestP[i]);
                                sameprocesslock[student_id-902001] = 0;
                                continue;
                            }
                        }
                    }
                    else{
                        dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                        FD_CLR(requestP[i].conn_fd,&master);
                        close(requestP[i].conn_fd);
                        free_request(&requestP[i]);
                        //sameprocesslock[student_id-902001] = 0;
                        continue;
                    }
                }
                else{
                    dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                    FD_CLR(requestP[i].conn_fd,&master);
                    close(requestP[i].conn_fd);
                    free_request(&requestP[i]);
                    //sameprocesslock[student_id-902001] = 0;
                    continue;
                }
                requestP[i].buf_len=0;
            }
            else if (requestP[i].conn_fd && requestP[i].buf_len && requestP[i].wait_for_write != 0){
                struct flock writeflock;
                int student_id = requestP[i].wait_for_write;
                requestP[i].wait_for_write = 0;
                registerRecord newrecord;
                char vaccine[3][10];
                /*int ret = handle_read(&requestP[conn_fd]);*/  // parse data from client to requestP[conn_fd].buf
                sameprocesslock[student_id-902001] = 0;
                char *read_len = requestP[i].buf; /*read_len = requestP[i].buf;*/
                if (strlen(read_len) == 5 && read_len[1]==' ' && read_len[3] == ' '){
                    int check_valid[3]={0};
                    int checkmate = 0;
                    for (int j = 0; j < 3 ; j++){
                        if (read_len[2*j] < 49 || read_len[2*j] > 51){
                            dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                            FD_CLR(requestP[i].conn_fd,&master);
                            close(requestP[i].conn_fd);
                            free_request(&requestP[i]);
                            writeflock.l_start = (student_id-902001)*16;
                            writeflock.l_len = 16;
                            writeflock.l_type = F_UNLCK;
                            writeflock.l_whence = SEEK_SET;
                            fcntl(register_fd,F_SETLK,&writeflock);
                            sameprocesslock[student_id-902001] = 0;
                            checkmate = 1;
                            break;
                        }
                        if (j == 0) newrecord.AZ = read_len[2*j]-48;
                        else if (j == 1) newrecord.BNT = read_len[2*j]-48;
                        else if (j == 2) newrecord.Moderna = read_len[2*j]-48;
                        check_valid[read_len[2*j]-48-1] = 1;
                        if (j == 0) strcpy(vaccine[read_len[2*j]-48-1],"AZ");
                        else if (j == 1) strcpy(vaccine[read_len[2*j]-48-1],"BNT");
                        else if (j == 2) strcpy(vaccine[read_len[2*j]-48-1],"Moderna");
                    }
                    if (checkmate == 1) continue;
                    int checkcheckhaha = 0;
                    for (int j = 0; j < 3; j++){
                        if (check_valid[j] == 0){
                            dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                            FD_CLR(requestP[i].conn_fd,&master);
                            close(requestP[i].conn_fd);
                            free_request(&requestP[i]);
                            writeflock.l_start = (student_id-902001)*16;
                            writeflock.l_len = 16;
                            writeflock.l_type = F_UNLCK;
                            writeflock.l_whence = SEEK_SET;
                            fcntl(register_fd,F_SETLK,&writeflock);
                            sameprocesslock[student_id-902001] = 0;
                            checkcheckhaha = 1;
                            break;
                        }
                    }
                    if (checkcheckhaha == 1)continue;
                    newrecord.id = student_id;
                    dprintf(requestP[i].conn_fd,"Preference order for %d modified successed, new preference order is %s > %s > %s.\n",newrecord.id,vaccine[0],vaccine[1],vaccine[2]);
                    lseek(register_fd,sizeof(registerRecord)*(student_id-902001),SEEK_SET);
                    write(register_fd,&newrecord,sizeof(registerRecord));
                    /*fprintf(stderr, "%s", requestP[conn_fd].buf);
                    sprintf(buf,"%s : %s",accept_write_header,requestP[conn_fd].buf);
                    write(requestP[conn_fd].conn_fd, buf, strlen(buf));*/
                    
                    FD_CLR(requestP[i].conn_fd,&master);
                    close(requestP[i].conn_fd);
                    free_request(&requestP[i]);
                    writeflock.l_start = (student_id-902001)*16;
                    writeflock.l_len = 16;
                    writeflock.l_type = F_UNLCK;
                    writeflock.l_whence = SEEK_SET;
                    fcntl(register_fd,F_SETLK,&writeflock);
                    sameprocesslock[student_id-902001] = 0;
                }
                else{
                    dprintf(requestP[i].conn_fd,"[Error] Operation failed. Please try again.\n");
                    FD_CLR(requestP[i].conn_fd,&master);
                    close(requestP[i].conn_fd);
                    free_request(&requestP[i]);
                    
                    writeflock.l_start = (student_id-902001)*16;
                    writeflock.l_len = 16;
                    writeflock.l_type = F_UNLCK;
                    writeflock.l_whence = SEEK_SET;
                    fcntl(register_fd,F_SETLK,&writeflock);
                    sameprocesslock[student_id-902001] = 0;
                }
                
            }
        }
#endif
    }
    close(register_fd); //關掉要讀的檔案
    free(requestP);
    return 0;
}

// ======================================================================================================
// You don't need to know how the following codes are working
#include <fcntl.h>

static void init_request(request* reqP) {
    reqP->conn_fd = -1;
    reqP->buf_len = 0;
    reqP->id = 0;
}

static void free_request(request* reqP) {
    /*if (reqP->filename != NULL) {
        free(reqP->filename);
        reqP->filename = NULL;
    }*/
    init_request(reqP);
}

static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;
    int tmp;

    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    tmp = 1;
    if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp)) < 0) {
        ERR_EXIT("setsockopt");
    }
    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        ERR_EXIT("bind");
    }
    if (listen(svr.listen_fd, 1024) < 0) {
        ERR_EXIT("listen");
    }

    // Get file descripter table size and initialize request table
    maxfd = getdtablesize();
    requestP = (request*) malloc(sizeof(request) * maxfd);
    if (requestP == NULL) {
        ERR_EXIT("out of memory allocating all requests");
    }
    for (int i = 0; i < maxfd; i++) {
        init_request(&requestP[i]);
    }
    requestP[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(requestP[svr.listen_fd].host, svr.hostname);

    return;
}