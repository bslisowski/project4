/*
  This module is for the processes that are generating the signals.
*/

#include "procsig.h"

/*
  Globally defining the shared variables for the signal generating processes.
*/
struct genstruct{
  int sig1;
  int sig2;
  pthread_mutex_t lock1;
  pthread_mutexattr_t attr1;
  pthread_mutex_t lock2;
  pthread_mutexattr_t attr2;
};

//struct pointer for shared memory
struct genstruct *gens;
//integer for the shared memory ID
int shared;

/*
  Initializing and mapping the shared variables and mutex's.
*/
void initgen(){
  key_t key = ftok("key.bat", 1);
  shared = shmget(key,sizeof(struct genstruct *),0666|IPC_CREAT);
  gens = (struct genstruct *)shmat(shared,(void*)0,0);
  gens->sig1 = 0;
  gens->sig2 = 0;
  pthread_mutexattr_init(&(gens->attr1));
  pthread_mutexattr_setpshared(&(gens->attr1), PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(gens->lock1), &(gens->attr1));
  pthread_mutexattr_init(&(gens->attr2));
  pthread_mutexattr_setpshared(&(gens->attr2), PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(gens->lock2), &(gens->attr2));
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
      pthread_mutex_lock(&(gens->lock1));
      (gens->sig1)++;
      pthread_mutex_unlock(&(gens->lock1));
    }
    else{
      pthread_mutex_lock(&(gens->lock2));
      (gens->sig2)++;
      pthread_mutex_unlock(&(gens->lock2));
    }
    i++;
    //delay next iteration by .01 to .1 seconds
  }

  exit(0);
}

/*
  Destroying locks.
  Detatching and deallocating the shared memory.
*/
void freegenmem(){
  pthread_mutex_destroy(&(gens->lock1));
  pthread_mutexattr_destroy(&(gens->attr1));
  pthread_mutex_destroy(&(gens->lock2));
  pthread_mutexattr_destroy(&(gens->attr2));
  shmdt(gens);
  shmctl(shared,IPC_RMID,NULL);
}


//printing results for testing purposes
void printsigs(){
  float sigtotal = gens->sig1 + gens->sig2;
  float sig1avg = (float)gens->sig1/sigtotal;
  float sig2avg = (float)gens->sig2/sigtotal;
  printf("total = %f\nsig1 = %d\nsig1 avg = %f\nsig2 = %d\nsig2avg = %f\n", sigtotal, gens->sig1, sig1avg, gens->sig2, sig2avg);

}
