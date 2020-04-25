/*
  This module is for the processes that are generating the signals.
*/

#include "threadsig.h"

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
struct genstruct gens;

/*
  Initializing and mapping the shared variables and mutex's.
*/
void initgen(){
  gens.sig1 = 0;
  gens.sig2 = 0;
  pthread_mutexattr_init(&gens.attr1);
  pthread_mutex_init(&gens.lock1, &gens.attr1);
  pthread_mutexattr_init(&gens.attr2);
  pthread_mutex_init(&gens.lock2, &gens.attr2);
}

/*
  Function for the signal generating processes.
  I decided to use each processes' pid as the seed for srand to guarantee
  different rand() outcomes.
*/
void * siggens(void * args){
  pthread_sigmask(SIG_BLOCK, &mask, NULL);
  int pid = (int)getpid();
  srand(pid);
  int sig;
  int i = 0;
  useconds_t waittime;
  waittime = (useconds_t) ((rand()%(100000-10001)) + 10000);
  usleep(waittime);
  //variables to make loop run for n seconds (end)
  time_t start;
  time_t curr;
  float timeelapsed;
  float end = 30;
  time(&start);

  while(timeelapsed < end){
    //if the random number is even, send SIGUSR1. else send SIGUSR2
    if ((sig = rand())%2 == 0){
      pthread_mutex_lock(&gens.lock1);
      signalthreads(SIGUSR1);
      gens.sig1++;
      pthread_mutex_unlock(&gens.lock1);
    }
    else{
      pthread_mutex_lock(&gens.lock2);
      signalthreads(SIGUSR2);
      gens.sig2++;
      pthread_mutex_unlock(&gens.lock2);
    }
    i++;
    //delay next iteration by .01 to .1 seconds
    waittime = (useconds_t) ((rand()%(100000-10001)) + 10000);
    usleep(waittime);
    curr = time(0);
    timeelapsed = difftime(curr, start);
  }
  sleep(1);
  kill(0, SIGTERM);
  pthread_exit(0);
}

//function to send signals to handler threads and reporter thread
void signalthreads(int sig){
  for (int i = 3; i < 8; i++){
    pthread_kill(tid[i], sig);
  }
}

//Destroying locks.
void destrgenlocks(){
  printf("SIGUSR1 sent: %d\nSIGUSR2 sent: %d\n", gens.sig1, gens.sig2);
  pthread_mutex_destroy(&gens.lock1);
  pthread_mutexattr_destroy(&gens.attr1);
  pthread_mutex_destroy(&gens.lock2);
  pthread_mutexattr_destroy(&gens.attr2);
}
