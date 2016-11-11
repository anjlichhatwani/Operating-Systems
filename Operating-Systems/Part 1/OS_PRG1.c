#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
 
#define NUM_THREADS 2

int SharedVariable = 0; 
pthread_mutex_t lock_x;

bool isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        return false;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
} 

void SimpleThread(int which) {
	int num, val;
	for(num = 0; num < 20; num++) {
		pthread_mutex_lock(&lock_x);
		if (random() > RAND_MAX / 2)
		//printf("*** thread %d is going to sleep\n",which);
		usleep(500);
		pthread_mutex_unlock(&lock_x);
		val = SharedVariable;
		printf("*** thread %d sees value %d\n", which, val);
		SharedVariable = val + 1;
		//printf("*** thread %d changed value from %d to %d\n",which,val, val+1);
	}
	val = SharedVariable;
	printf("Thread %d sees final value %d\n", which, val);
} 

/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  int tid;
  double stuff;
} thread_data_t;
 
/* thread function */
void *thr_func(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
 
  //printf("hello from thr_func, thread id: %d\n", data->tid);
  
  SimpleThread(data->tid);
 
  pthread_exit(NULL);
}
 
int main(int argc, char *argv[]) {
	bool isNum = false;
	char *endptr;
	int i, rc, x;

	// Take the input and check if it is a number.
	if( argc == 2 ) {
		printf("The argument supplied is %s\n", argv[1]);
	}
	else {
		printf("One argument expected.\n");
		return EXIT_SUCCESS;
	}
	
	isNum = isNumber(argv[1]);
	
	if(isNum){
	   printf("The input value is a positive number\n");
	} else {
		printf("The input value is not a positive number\n");
		return EXIT_SUCCESS;
	}
   
	x = strtol(argv[1], &endptr, 10);
	
	// Creating the thread components.
	pthread_t thr[x];
	
	/* create a thread_data_t argument array */
	thread_data_t thr_data[x];
	
	/* initialize pthread mutex protecting "shared_x" */
	pthread_mutex_init(&lock_x, NULL);
 
	/* create threads */
	for (i = 0; i < x; ++i) {
    thr_data[i].tid = i;
    if ((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
		fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
		return EXIT_FAILURE;
		}
	}
	/* block until all threads complete */
	for (i = 0; i < x; ++i) {
		pthread_join(thr[i], NULL);
	}
 
	return EXIT_SUCCESS;
}
