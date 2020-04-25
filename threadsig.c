/*
  Brendan Lisowski
  CIS 3207 - Project 4

  This is the main source code for the program that uses child processes.
  I made two modules to seperate shared variables.
  procgen.c contains the shared variables that the signal generating processes
  need.
  prochandle.c contains the shared variables that the signal handling processes
  and the reporter need.
*/

#include "threadsig.h"
pthread_t tid[8];

int main(int argc, char *argv[]){

  //initialize all the shared variables and locks.
  initgen();
  inithandle();
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);


  //Setting up the parent process to ignore SIGUSR1 and SIGUSR2
  struct sigaction action;
  action.sa_handler = handlesigs;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGUSR1, &action, NULL);
  sigaction(SIGUSR2, &action, NULL);
  //setting handler for SIGINT and SIGTERM
  struct sigaction quitaction;
  quitaction.sa_handler = sigintterm;
  sigemptyset(&quitaction.sa_mask);
  quitaction.sa_flags = 0;
  sigaction(SIGINT, &quitaction, NULL);
  sigaction(SIGTERM, &quitaction, NULL);

  //create child processes
  for(int i = 0; i < 8; i++){
    if (i < 3){
      pthread_create(&tid[i], NULL, siggens, NULL);
    }
    else if (i == 7){
      pthread_create(&tid[i], NULL, sigreporter, NULL);
    }
    else{
      pthread_create(&tid[i], NULL, sighandlers, &i);
    }
  }
  pthread_sigmask(SIG_BLOCK, &mask, NULL);

  //wait for child processes to finish
  for (int i = 0; i < 8; i++){
    pthread_join(tid[i], NULL);
  }

  return 0;
}

void sigintterm(){
  //printing the average reception of signals
  printavgs();
  //destroying locks
  destrgenlocks();
  destrhandlelocks();
  exit(0);
}
