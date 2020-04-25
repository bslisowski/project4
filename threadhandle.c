/*
  This module is for the processes that are handling the signals (including
  the reporting process).
*/
#include "threadsig.h"

/*
  Globally defining the shared variables.
*/
struct handlestruct{
  //counters for signals recieved by the handlers
  sig_atomic_t sig1;
  sig_atomic_t sig2;
  pthread_mutex_t sig1lock;
  pthread_mutex_t sig2lock;
};

//declaring a volatile struct pointer for shared memory
struct handlestruct handle;

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
struct reportstruct rep;

//structs for getting the average time between signal receptioin
struct timespec sig1time;
struct timespec sig2time;

//lock to make 2 handlers ignore SIGUSR1 and 2 handlers ignore SIGUSR2
pthread_mutex_t handlelock;
int i;
pthread_t sigid[4];

//Initializing and mapping shared variables
void inithandle(){
  handle.sig1 = 0;
  handle.sig2 = 0;
  pthread_mutex_init(&handlelock, NULL);
  pthread_mutex_init(&handle.sig1lock, NULL);
  pthread_mutex_init(&handle.sig2lock, NULL);
  i = 0;
}

//handler function for the signal handling functions
void handlesigs(int sig){
  //if the thread is the reporter thread
  if (tid[7] == pthread_self()){
    time_t lastsec;
    long lastnsec;
    if (sig == SIGUSR1){
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
    else{
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
    if (rep.reportcount == 10){
      printreport();
    }
  }
  //handler threads
  else if (sig == SIGUSR1){
    pthread_mutex_lock(&handle.sig1lock);
    handle.sig1++;
    pthread_mutex_unlock(&handle.sig1lock);
  }
  else{
    pthread_mutex_lock(&handle.sig2lock);
    handle.sig2++;
    pthread_mutex_unlock(&handle.sig2lock);
  }
}

//function for the signal hangling processes
void * sighandlers(void * arg){
  sigset_t handlemask;
  sigemptyset(&handlemask);
  //this code is locked so 2 threads ignore SIGUSR1 and 2 threads ignore SIGUSR2
  pthread_mutex_lock(&handlelock);
  sigid[i] = pthread_self();
  if(i < 2){
    sigaddset(&handlemask, SIGUSR1);
  }
  else{
    sigaddset(&handlemask, SIGUSR2);
  }
  i++;
  pthread_mutex_unlock(&handlelock);
  pthread_sigmask(SIG_BLOCK, &handlemask, NULL);

  while(1){
    pause();
  }
  exit(0);
}

//function for the signal reporting process
void * sigreporter(void * args){
  rep.reportcount = 0;
  while(1){
    pause();
  }
  exit(0);
}

//function to calculate averages and report
void printreport(){
  rep.numreports++;
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  printf("System time: %lu\n", time.tv_sec);
  printf("\tSIGUSR1: %d\n\tSIGUSR2: %d\n", rep.sig1rc, rep.sig2rc);
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
  pthread_mutex_lock(&handle.sig1lock);
  printf("\tS1 total: %d\n", handle.sig1);
  pthread_mutex_unlock(&handle.sig1lock);

  pthread_mutex_lock(&handle.sig2lock);
  printf("\tS2 total: %d\n", handle.sig2);
  pthread_mutex_unlock(&handle.sig2lock);
  rep.reportcount = 0;
  rep.sig1rc = 0;
  rep.sig2rc = 0;
  rep.sig1avg = 0;
  rep.sig2avg = 0;
}

//printing the total average reception of signals
void printavgs(){
  long i = rep.sig1avgtotal/rep.numreports;
  long j = rep.sig2avgtotal/rep.numreports;
  printf("S1 avg total: %ld μs\nS2 avg total: %ld μs\n", i, j);
  printf("Reports: %d\n", rep.numreports);
}

//destroying the mutex locks
void destrhandlelocks(){
  pthread_mutex_destroy(&handle.sig1lock);
  pthread_mutex_destroy(&handle.sig2lock);
  pthread_mutex_destroy(&handlelock);
}
