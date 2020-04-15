/*
  This module is for the processes that are generating the signals.
*/

#include "procsig.h"

/*
  Globally defining the shared variables for the signal generating processes.
*/
int *sig1;
int *sig2;
pthread_mutex_t *lock1;
pthread_mutexattr_t attr1;
pthread_mutex_t *lock2;
pthread_mutexattr_t attr2;

/*
  Initializing and mapping the shared variables and mutex's.
*/
void initgen(){
  sig1 = mmap(NULL, _SC_PAGESIZE, PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sig2 = mmap(NULL, _SC_PAGESIZE, PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  lock1 = mmap(NULL, _SC_PAGESIZE, PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  pthread_mutexattr_init(&attr1);
  pthread_mutexattr_setpshared(&attr1, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(lock1, &attr1);
  lock2 = mmap(NULL, _SC_PAGESIZE, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  pthread_mutexattr_init(&attr2);
  pthread_mutexattr_setpshared(&attr2, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(lock2, &attr2);
}

/*
  Function for the signal generating processes.
  I decided to use each processes' pid as the seed for srand to guarantee
  different rand() outcomes.
*/
void siggen(){
  int pid = (int)getpid();
  srand(pid);
  int sig;
  int i = 0;
  while(i < 500000){
    //if the random number is even, send SIGUSR1. else send SIGUSR2
    if ((sig = rand())%2 == 0){
      //signal(SIGUSR1);
      pthread_mutex_lock(lock1);
      (*sig1)++;
      pthread_mutex_unlock(lock1);
    }
    else{
      pthread_mutex_lock(lock2);
      (*sig2)++;
      pthread_mutex_unlock(lock2);
    }
    i++;
    //delay next iteration by .01 to .1 seconds
  }

  exit(0);
}

//printing results for testing purposes
void printsigs(){
  float sigtotal = *sig1 + *sig2;
  float sig1avg = (float)*sig1/sigtotal;
  float sig2avg = (float)*sig2/sigtotal;
  printf("total = %f\nsig1 = %d\nsig1 avg = %f\nsig2 = %d\nsig2avg = %f\n", sigtotal, *sig1, sig1avg, *sig2, sig2avg);
}
