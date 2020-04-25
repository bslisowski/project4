/*
  This module is for the processes that are handling the signals (including
  the reporting process).
*/
#include "procsig.h"

/*
  Globally defining the shared variables.
*/
struct handlestruct{
  sig_atomic_t sig1;
  sig_atomic_t sig2;
  pthread_mutex_t sig1lock;
  pthread_mutexattr_t sig1attr;
  pthread_mutex_t sig2lock;
  pthread_mutexattr_t sig2attr;
};
//declaring a volatile struct pointer for shared memory
struct handlestruct * volatile handle;
//integer for the shared memory ID
int shared;

//variables for the reporter process
struct reportstruct{
  //counter to tell the reporter when 10 signals have been received
  sig_atomic_t reportcount;
  //average time between reception of signals (μs)
  long sig1avg;
  long sig2avg;
  //counters for the signals received
  sig_atomic_t sig1rc;
  sig_atomic_t sig2rc;
  //average time between reception of signals for all reports (μs)
  long sig1avgtotal;
  long sig2avgtotal;
  //number of times the reporter has received 10 signals
  sig_atomic_t numreports;
};
struct reportstruct volatile rep;
//variable for a mask
sigset_t maskset;
//structs for getting the average time between signal receptioin
struct timespec sig1time;
struct timespec sig2time;

int isreporter = 0;

//Initializing and mapping shared variables
void inithandle(){
  key_t key = ftok("key.bat", 2);
  shared = shmget(key,sizeof(struct handlestruct *),0666|IPC_CREAT);
  handle = (struct handlestruct *)shmat(shared,(void*)0,0);
  pthread_mutexattr_init(&(handle->sig1attr));
  pthread_mutexattr_setpshared(&(handle->sig1attr), PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(handle->sig1lock), &(handle->sig1attr));
  pthread_mutexattr_init(&(handle->sig2attr));
  pthread_mutexattr_setpshared(&(handle->sig2attr), PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(handle->sig2lock), &(handle->sig2attr));
  handle->sig1 = 0;
  handle->sig2 = 0;
}

//handler function for the signal handling functions
void sigfunct(int sig){
  //write(1, "SIGUSR1\n", 9);
  if (sig == SIGUSR1){
    pthread_mutex_lock(&(handle->sig1lock));
    handle->sig1++;
    pthread_mutex_unlock(&(handle->sig1lock));
  }
  else{
    pthread_mutex_lock(&(handle->sig2lock));
    handle->sig2++;
    pthread_mutex_unlock(&(handle->sig2lock));
  }
}

//function for the signal hangling processes
void sighandlers(int i){
  //struct for the signal the process will handle
  struct sigaction action;
  action.sa_handler = sigfunct;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaddset(&action.sa_mask, SIGUSR1);
  sigaddset(&action.sa_mask, SIGUSR2);

  //struct for the signal the process will ignore
  struct sigaction ignaction;
  ignaction.sa_handler = SIG_IGN;
  sigemptyset(&ignaction.sa_mask);
  ignaction.sa_flags = 0;

  //setting handling function for SIGINT and SIGTERM
  struct sigaction exitaction;
  exitaction.sa_handler = handlersexit;
  sigemptyset(&exitaction.sa_mask);
  exitaction.sa_flags = 0;
  sigaction(SIGINT, &exitaction, NULL);
  sigaction(SIGTERM, &exitaction, NULL);

  //Setting two processes to handle SIGUSR1 and two to hand SIGUSR2
  if(i%2 == 0){
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &ignaction, NULL);
  }
  else{
    sigaction(SIGUSR1, &ignaction, NULL);
    sigaction(SIGUSR2, &action, NULL);
  }
  while(1){
    pause();
  }
  exit(0);
}

//function for the signal reporting process
void sigreporter(){
  isreporter = 1;
  rep.reportcount = 0;
  //struct for the signal the process will handle
  struct sigaction action;
  action.sa_handler = sigreporthandle;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaddset(&action.sa_mask, SIGUSR1);
  sigaddset(&action.sa_mask, SIGUSR2);
  sigaction(SIGUSR1, &action, NULL);
  sigaction(SIGUSR2, &action, NULL);
  //another mask to block signals when printing
  sigaddset(&maskset, SIGUSR1);
  sigaddset(&maskset, SIGUSR2);
  //setting handling function for SIGINT and SIGTERM
  struct sigaction exitaction;
  exitaction.sa_handler = handlersexit;
  sigemptyset(&exitaction.sa_mask);
  exitaction.sa_flags = 0;
  sigaction(SIGINT, &exitaction, NULL);
  sigaction(SIGTERM, &exitaction, NULL);

  while(1){
    pause();
  }
  exit(0);
}

/*
  Function for the reporting process. Need to figure out how to do the non-asynch-safe
  functions outside of the handler. Tried using sigprocmask() to block after the
  function returns but it eventually gets interrupted by a signal.
*/
void sigreporthandle(int sig){
  //for the current system time
  time_t lastsec;
  long lastnsec;
  //handling SIGUSR1
  if (sig == SIGUSR1){
    //incrementing the signal count and adding to average time
    rep.sig1rc++;
    rep.reportcount++;
    if (rep.reportcount != 1){
      lastsec = sig1time.tv_sec;
      lastnsec = sig1time.tv_nsec;
      clock_gettime(CLOCK_REALTIME, &sig1time);
      rep.sig1avg = (sig1time.tv_sec - lastsec) * 1000000 + (sig1time.tv_nsec - lastnsec)/1000;
    }
    else{
      clock_gettime(CLOCK_REALTIME, &sig1time);
    }
  }
  //handling SIGUSR2
  else{
    //incrementing the signal count and adding to average time
    rep.sig2rc++;
    rep.reportcount++;
    if (rep.reportcount != 1){
      lastsec = sig2time.tv_sec;
      lastnsec = sig2time.tv_nsec;
      clock_gettime(CLOCK_REALTIME, &sig2time);
      rep.sig2avg = (sig2time.tv_sec - lastsec) * 1000000 + (sig2time.tv_nsec - lastnsec)/1000;
    }
    else{
      clock_gettime(CLOCK_REALTIME, &sig2time);
    }
  }
  //printing
  if (rep.reportcount == 10){
    rep.numreports++;
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    printf("System time: %lu\n", time.tv_sec);
    printf("\tSIGUSR1: %d\n\tSIGUSR2: %d\n", rep.sig1rc, rep.sig2rc);
    /*
      Calculating the average time between signal reception. Since there are
      only 9 intervals, the if/else statements prevent dividing by zero when
      one of the signals is only recieved once.
    */
    int i,j;
    if (rep.sig1rc == 1){
      i = 0;
      j = (rep.sig2avg/(rep.sig2rc-1));
    }
    else if (rep.sig2rc == 1){
      i = (rep.sig1avg/(rep.sig1rc-1));
      j = 0;
    }
    else{
      i = (rep.sig1avg/(rep.sig1rc-1));
      j = (rep.sig2avg/(rep.sig2rc-1));
    }
    rep.sig1avgtotal += i;
    rep.sig2avgtotal += j;
    printf("\tS1 avg time: %d μs\n\tS2 avg time: %d μs\n", i, j);
    pthread_mutex_lock(&(handle->sig1lock));
    printf("\tS1 total: %d\n", handle->sig1);
    pthread_mutex_unlock(&(handle->sig1lock));
    pthread_mutex_lock(&(handle->sig2lock));
    printf("\tS2 total: %d\n", handle->sig2);
    pthread_mutex_unlock(&(handle->sig2lock));
    rep.reportcount = 0;
    rep.sig1rc = 0;
    rep.sig2rc = 0;
    rep.sig1avg = 0;
    rep.sig2avg = 0;
  }
}

/*
  Destroying locks
  Detatching and deallocating the shared memory.
*/
void freehandlemem(){
  pthread_mutexattr_destroy(&(handle->sig1attr));
  pthread_mutex_destroy(&(handle->sig1lock));
  pthread_mutexattr_destroy(&(handle->sig2attr));
  pthread_mutex_destroy(&(handle->sig2lock));
  shmdt(handle);
  shmctl(shared,IPC_RMID,NULL);
}

//Function to detatch shared memory
void handlersexit(){
  //printing the total average reception of signals
  if (isreporter){
    long i = rep.sig1avgtotal/rep.numreports;
    long j = rep.sig2avgtotal/rep.numreports;
    printf("S1 avg total: %ld μs\nS2 avg total: %ld μs\n", i, j);
    printf("Reports: %d\n", rep.numreports);
  }
  shmdt(handle);
  exit(0);
}
