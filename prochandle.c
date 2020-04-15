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


//function for the signal hangling processes
void sighandle(){

  exit(0);
}

//function for the signal reporting process
void sigreporter(){

  exit(0);
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
