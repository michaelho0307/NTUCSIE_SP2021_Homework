#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

extern int timeslice, switchmode;

typedef struct TCB_NODE *TCB_ptr;
typedef struct TCB_NODE{
    jmp_buf  Environment;
    int      Thread_id;
    TCB_ptr  Next;
    TCB_ptr  Prev;
    int i, N;
    int w, x, y, z;
} TCB;

extern jmp_buf MAIN, SCHEDULER;
extern TCB_ptr Head;
extern TCB_ptr Current;
extern TCB_ptr Work;
extern sigset_t base_mask, waiting_mask, tstp_mask, alrm_mask;

void sighandler(int signo);
void scheduler();

// Call function in the argument that is passed in
#define ThreadCreate(function, thread_id, number)               \
{                                                               \
	/* Please fill this code section. */						\
    int createstate = setjmp(MAIN);                             \
    if (createstate == 0){                                      \
        function(thread_id,number);                             \
    }                                                           \
}                                                                

// Build up TCB_NODE for each function, insert it into circular linked-list
#define ThreadInit(thread_id, number)                           \
{                                                               \
	/* Please fill this code section. */                        \
                                                                \
    Work = (TCB*)malloc(sizeof(TCB));                           \
    int initjmp = setjmp(Work->Environment);                    \
    if (initjmp == 0){                                          \
        if (thread_id == 1){                                    \
            Head = Work; Current = Work;                        \
            Work->Thread_id = thread_id;                        \
            Work->Next = NULL; Work->Prev = NULL;               \
            Work->N = number;                                   \
            Work->i = 0; Work->w = 0;                           \
            Work->x = 0; Work->y = 0; Work->z = 0;              \
            longjmp(MAIN,10);                                   \
        }			                                            \
        else if (thread_id == 2){                               \
            Current = Work;                                     \
            Head->Next = Current;                               \
            Current->Prev = Head; Current->Next = NULL;         \
            Work->Thread_id = thread_id;                        \
            Work->N = number;                                   \
            Work->i = 0; Work->w = 0;                           \
            Work->x = 0; Work->y = 0; Work->z = 0;              \
            longjmp(MAIN,11);                                   \
        }                                                       \
        else if (thread_id == 3){                               \
            Current->Next = Work; Work->Prev = Current;         \
            Work->Next = Head; Head->Prev = Work;               \
            Current = Work;                                     \
            Work->Thread_id = thread_id;                        \
            Work->N = number;                                   \
            Work->i = 0; Work->w = 0;                           \
            Work->x = 0; Work->y = 0; Work->z = 0;              \
            Current = Head;                                     \
            longjmp(MAIN,12);                                   \
        }                                                       \
        else{                                                   \
            fprintf(stderr,"Wrong Thread ID!\n");               \
        }                                                       \
    }                                                           \
}                                                               

// Call this while a thread is terminated
#define ThreadExit()                                            \
{                                                               \
	/* Please fill this code section. */                        \
    Current->Thread_id = 0;                                     \
    longjmp(SCHEDULER,2);                                       \
}                                                               

// Decided whether to "context switch" based on the switchmode argument passed in main.c

#define ThreadYield()                                           \
{                                                               \
	/* Please fill this code section. */                        \
    int yieldjump = setjmp(Current->Environment);               \
    if (yieldjump == 0){                                        \
        if (switchmode == 0){                                   \
            longjmp(SCHEDULER,1);                               \
        }                                                       \
        else if (switchmode == 1){                              \
            sigset_t curset;                                    \
            sigpending(&curset);                                \
            if (sigismember(&curset,SIGTSTP)){                  \
                sigprocmask(SIG_UNBLOCK,&tstp_mask,NULL);       \
            }                                                   \
            if (sigismember(&curset,SIGALRM)){                  \
                sigprocmask(SIG_UNBLOCK,&alrm_mask,NULL);       \
            }                                                   \
        }                                                       \
    }                                                           \
}           
