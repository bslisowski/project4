#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void initgen();
void inithandle();

void siggen();
void sighandle();
void sigreporter();

void printsigs();
