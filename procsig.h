
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void initgen();
void inithandle();
void freegenmem();
void freehandlemem();
void siggen();
void initsigs();
void sighandlers(int i);
void sigreporter();
void parentexit();
void handlersexit();
void gensexit();
void sigfunct(int);
void sigreporthandle(int);
