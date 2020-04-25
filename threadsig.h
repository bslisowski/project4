
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

sigset_t mask;
pthread_t tid[8];

void sigintterm();

void initgen();
void * siggens(void *);
void signalthreads(int);
void destrgenlocks();

void inithandle();
void handlesigs(int);
void * sighandlers(void *);
void * sigreporter(void *);
void printreport();
void printavgs();
void destrhandlelocks();
