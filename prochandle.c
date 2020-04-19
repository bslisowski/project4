/*
  This module is for the processes that are handling the signals (including
  the reporting process).
*/
#include "procsig.h"

/*
  Globally defining the shared variables.
*/
struct handlestruct{
  int sig1;
  int sig2;
  pthread_mutex_t lock1;
  pthread_mutexattr_t attr1;
  pthread_mutex_t lock2;
  pthread_mutexattr_t attr2;

  pthread_cond_t cond;
  pthread_condattr_t condattr;
  //count for the reporter
  int sigcount;
};

//declaring struct pointer for shared memory
struct handlestruct *handle;
//integer for the shared memory ID
int shared;
//variables for the reporter process
int reportcount;
long sig1avg;
int sig1rc;
int sig2rc;
long sig2avg;
struct timespec sig1time;
struct timespec sig2time;

//Initializing and mapping shared variables
void inithandle(){
  key_t key = ftok("key.bat", 2);
  shared = shmget(key,sizeof(struct handlestruct *),0666|IPC_CREAT);
  handle = (struct handlestruct *)shmat(shared,(void*)0,0);
  handle->sig1 = 0;
  handle->sig2 = 0;
  pthread_mutexattr_init(&(handle->attr1));
  pthread_mutexattr_setpshared(&(handle->attr1), PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(handle->lock1), &(handle->attr1));
  pthread_mutexattr_init(&(handle->attr2));
  pthread_mutexattr_setpshared(&(handle->attr2), PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(handle->lock2), &(handle->attr2));
  pthread_condattr_init(&(handle->condattr));
  pthread_condattr_setpshared(&(handle->condattr), PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&(handle->cond), &(handle->condattr));
  handle->sigcount = 0;
}

void sigfunct1(){
  //write(1, "SIGUSR1\n", 9);
  handle->sig1++;
}

void sigfunct2(){
  //write(1, "SIGUSR2\n", 9);
  handle->sig2++;
}
//function for the signal hangling processes
void sighandle(int i){
  struct sigaction action;
  action.sa_handler = sigfunct1;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGUSR1, &action, NULL);
  action.sa_handler = sigfunct2;
  sigaction(SIGUSR2, &action, NULL);

  if(i%2 == 0){
    signal(SIGUSR1, SIG_IGN);
  }
  else{
    signal(SIGUSR2, SIG_IGN);
  }
  while(1){
    pause();
  }
  exit(0);
}

//function for the signal reporting process
void sigreporter(){
  reportcount = 0;
  sigset_t maskset;
  sigemptyset(&maskset);
  sigaddset(&maskset, SIGUSR1);
  sigaddset(&maskset, SIGUSR2);
  struct sigaction action;
  action.sa_handler = sigreport1;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGUSR1, &action, NULL);
  action.sa_handler = sigreport2;
  sigaction(SIGUSR2, &action, NULL);
  int l = 0;
  int k = 0;
  while(1){
    pause();
    sigprocmask(SIG_BLOCK, &maskset, NULL);
    if (reportcount == 10){
      struct timespec time;
      clock_gettime(CLOCK_REALTIME, &time);
      printf("System time: %lu\n", time.tv_sec);
      printf("\tS1: %d\n\tS2: %d\n", sig1rc, sig2rc);
      printf("\tSIGUSR1: %d\n\tSIGUSR2: %d\n", handle->sig1, handle->sig2);
      int i,j;
      if (sig1rc == 1){
        i = 0;
        j = (sig2avg/(sig2rc-1));
      }
      else if (sig2rc == 1){
        i = (sig1avg/(sig1rc-1));
        j = 0;
      }
      else{
        i = (sig1avg/(sig1rc-1));
        j = (sig2avg/(sig2rc-1));
      }
      printf("\tSIGUSR1 average: %d μs\n\tSIGUSR2 average: %d μs\n", i, j);
      reportcount = 0;
      sig1rc = 0;
      sig2rc = 0;
      sig1avg = 0;
      sig2avg = 0;
      k++;
    }
    sigprocmask(SIG_UNBLOCK, &maskset, NULL);
    printf("l = %d\nk = %d\nreportcount = %d\n", l, k, reportcount);
    l++;
  }
  exit(0);
}
//(stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
void sigreport1(){
  time_t lastsec;
  long lastnsec;
  sig1rc++;
  reportcount++;
  if (reportcount != 1){
    lastsec = sig1time.tv_sec;
    lastnsec = sig1time.tv_nsec;
    clock_gettime(CLOCK_REALTIME, &sig1time);
    sig1avg = (sig1time.tv_sec - lastsec) * 1000000 + (sig1time.tv_nsec - lastnsec)/1000;
  }
  else{
    clock_gettime(CLOCK_REALTIME, &sig1time);
  }
}

void sigreport2(){
  time_t lastsec;
  long lastnsec;
  sig2rc++;
  reportcount++;
  if (reportcount != 1){
    lastsec = sig2time.tv_sec;
    lastnsec = sig2time.tv_nsec;
    clock_gettime(CLOCK_REALTIME, &sig2time);
    sig2avg = (sig2time.tv_sec - lastsec) * 1000000 + (sig2time.tv_nsec - lastnsec)/1000;
  }
  else{
    clock_gettime(CLOCK_REALTIME, &sig2time);
  }
}


/*
  Destroying locks and condition variables.
  Detatching and deallocating the shared memory.
*/
void freehandlemem(){
  pthread_mutex_destroy(&(handle->lock1));
  pthread_mutexattr_destroy(&(handle->attr1));
  pthread_mutex_destroy(&(handle->lock2));
  pthread_mutexattr_destroy(&(handle->attr2));
  pthread_cond_destroy(&(handle->cond));
  pthread_condattr_destroy(&(handle->condattr));
  shmdt(handle);
  shmctl(shared,IPC_RMID,NULL);
}
