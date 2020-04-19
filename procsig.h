
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
void sighandle(int i);
void sigreporter();

void printsigs();


void sigfunct1();
void sigfunct2();
void sigreport1();
void sigreport2();
