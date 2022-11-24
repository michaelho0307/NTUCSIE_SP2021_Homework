#include "threadtools.h"

// Please complete this three functions. You may refer to the macro function defined in "threadtools.h"
// Mountain Climbing
// You are required to solve this function via iterative method, instead of recursion.
void MountainClimbing(int thread_id, int number){
	/* Please fill this code section. */
	ThreadInit(thread_id,number);
	//printf("ckheckckjkl");
	for(Current->i = 0;Current->i <= Current->N;(Current->i)++){
		sleep(1);
		/* function work */
		//printf("Current->i: %d\n",Current->i);
		//printf("Round: %d %d %d %d\n",number,Current->w,Current->x,Current->y);
		if (Current->i == 0){
			Current->w = 1;
		}
		else if (Current->i == 1){
			Current->w = 1; Current->x = 1; Current->y = 1;
		}
		else{
			Current->w = Current->x+Current->y;
			Current->y = Current->x;
			Current->x = Current->w;
		}
		printf("Mountain Climbing: %d\n", Current->w);
		ThreadYield();
	} 
	ThreadExit();
}

// Reduce Integer
// You are required to solve this function via iterative method, instead of recursion.
void ReduceInteger(int thread_id, int number){
	/* Please fill this code section. */
	ThreadInit(thread_id,number);
	Current->w = Current->N;
	Current->x = 0;
	if (Current->w == 1){
		sleep(1);
		printf("Reduce Integer: %d\n", Current->x);
		ThreadYield();
	}
	while (Current->w != 1){
		sleep(1);
		/* function work */
		if (Current->w % 2 == 0){
			(Current->w) /= 2;
		}
		else if (Current->w == 3 || (Current->w % 4) == 1){
			(Current->w)--;
		}
		else{
			(Current->w)++;
		}
		(Current->x)++;
		printf("Reduce Integer: %d\n", Current->x);
		ThreadYield();
	}
	ThreadExit();
}

// Operation Count
// You are required to solve this function via iterative method, instead of recursion.
void OperationCount(int thread_id, int number){
	/* Please fill this code section. */
	ThreadInit(thread_id,number);
	for(Current->i = -1;Current->i < Current->N/2;(Current->i)++){
		sleep(1);
		/* function work */
		if (Current->i == -1){
			if (Current->N % 2 == 0) Current->w = (2*Current->N-1)/2+323;
			else Current->w = (Current->N-1)+323;
			Current->x = 0;
		}
		else{
			Current->x += (Current->w)-(2*(Current->i)+323);
		}
		printf("Operation Count: %d\n", Current->x);
		ThreadYield();
	} 
	ThreadExit();
}