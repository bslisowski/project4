/*
  This module is for the processes that are handling the signals (including
  the reporting process).
*/
#include "procsig.h"

/*
  Globally defining the shared variables.
*/
int *sig1;
int *sig2;
pthread_mutex_t *lock1;
pthread_mutexattr_t attr1;
pthread_mutex_t *lock2;
pthread_mutexattr_t attr2;

//Initializing and mapping shared variables
void inithandle(){
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


//function for the signal hangling processes
void sighandle(){

  exit(0);
}

//function for the signal reporting process
void sigreporter(){

  exit(0);
}
