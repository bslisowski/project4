/*
  This module is for the processes that are generating the signals.
*/

#include "procsig.h"

/*
  Globally defining the shared variables for the signal generating processes.
*/
struct genstruct{
  sig_atomic_t sig1;
  sig_atomic_t sig2;
  pthread_mutex_t lock1;
  pthread_mutexattr_t attr1;
  pthread_mutex_t lock2;
  pthread_mutexattr_t attr2;
};

//struct pointer for shared memory
struct genstruct * volatile gens;
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
  //
  struct sigaction exitaction;
  exitaction.sa_handler = gensexit;
  sigemptyset(&exitaction.sa_mask);
  exitaction.sa_flags = 0;
  sigaction(SIGINT, &exitaction, NULL);
  sigaction(SIGTERM, &exitaction, NULL);

  int pid = (int)getpid();
  srand(pid);
  int sig;
  int i = 0;
  useconds_t waittime;
  //variables to make loop run for n seconds (end)
  time_t start;
  time_t curr;
  float timeelapsed;
  float end = 30;
  time(&start);

  while(timeelapsed < end){
    //if the random number is even, send SIGUSR1. else send SIGUSR2
    if ((sig = rand())%2 == 0){
      kill(0, SIGUSR1);
      pthread_mutex_lock(&(gens->lock1));
      (gens->sig1)++;
      pthread_mutex_unlock(&(gens->lock1));
    }
    else{
      kill(0, SIGUSR2);
      pthread_mutex_lock(&(gens->lock2));
      (gens->sig2)++;
      pthread_mutex_unlock(&(gens->lock2));
    }
    i++;
    //delay next iteration by .01 to .1 seconds
    waittime = (useconds_t) ((rand()%(100000-10001)) + 10000);
    usleep(waittime);
    curr = time(0);
    timeelapsed = difftime(curr, start);
  }
  kill(0, SIGTERM);
  exit(0);
}

/*
  Destroying locks.
  Detatching and deallocating the shared memory.
*/
void freegenmem(){
  printf("SIGUSR1 sent: %d\nSIGUSR2 sent: %d\n", gens->sig1, gens->sig2);
  pthread_mutex_destroy(&(gens->lock1));
  pthread_mutexattr_destroy(&(gens->attr1));
  pthread_mutex_destroy(&(gens->lock2));
  pthread_mutexattr_destroy(&(gens->attr2));
  shmdt(gens);
  shmctl(shared,IPC_RMID,NULL);
}

//Function to detatch shared memory
void gensexit(){
  shmdt(gens);
  exit(0);
}
