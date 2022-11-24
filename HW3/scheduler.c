#include "threadtools.h"

/*
1) You should state the signal you received by: printf('TSTP signal caught!\n') or printf('ALRM signal caught!\n')
2) If you receive SIGALRM, you should reset alarm() by timeslice argument passed in ./main
3) You should longjmp(SCHEDULER,1) once you're done.
*/
void sighandler(int signo){
	/* Please fill this code section. */
	if (signo == 20){ //TSTP
		printf("TSTP signal caught!\n");
		longjmp(SCHEDULER,1);
	}
	else if (signo == 14){ //ALRM
		printf("ALRM signal caught!\n");
		alarm(timeslice);
		longjmp(SCHEDULER,1);
	}
}

/*
1) You are stronly adviced to make 
	setjmp(SCHEDULER) = 1 for ThreadYield() case
	setjmp(SCHEDULER) = 2 for ThreadExit() case
2) Please point the Current TCB_ptr to correct TCB_NODE
3) Please maintain the circular linked-list here
*/
void scheduler(){
	/* Please fill this code section. */
	//printf("%d %d %d\n",Head->N,Head->Next->N,Head->Next->Next->N);
	int scheduling = setjmp(SCHEDULER);
	if (scheduling == 0){
		longjmp(Head->Environment,10);
	}
	else if (scheduling == 1){
		for (int i = 0; i < 3; i++){
			Current = Current->Next;
			if (Current->Thread_id != 0){
				//printf("Current->Thread_id: %d\n",Current->Thread_id);
				longjmp(Current->Environment,10);		
			}
		}
	}
	else if (scheduling == 2){
		for (int i = 0; i < 3; i++){
			Current = Current->Next;
			if (Current->Thread_id != 0){
				//printf("Current->Thread_id: %d\n",Current->Thread_id);
				longjmp(Current->Environment,10);
			}
		}
		longjmp(MAIN,10);
	}
	//printf("Hello workd\n");
	
}
