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

#include "procsig.h"
pid_t pid[8];

int main(int argc, char *argv[]){

  //initialize all the shared variables and locks.
  initgen();
  inithandle();

  //Setting up the parent process to ignore SIGUSR1 and SIGUSR2
  struct sigaction action;
  action.sa_handler = SIG_IGN;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGUSR1, &action, NULL);
  sigaction(SIGUSR2, &action, NULL);

  //create child processes
  for(int i = 0; i < 8; i++){
    pid[i] = fork();
    if (pid[i] == 0){
      if (i < 3){
      //signal generators
        siggen();
      }
      else if (i == 7){
      //signal reporter
        sigreporter();
      }
      else{
      //signal handlers
        sighandlers(i);
      }
    }
  }

  /*
    Setting the handler for SIGINT and SIGTERM to a function that will
    deallocate the shared memory.
  */
  struct sigaction exitaction;
  exitaction.sa_handler = parentexit;
  sigemptyset(&exitaction.sa_mask);
  exitaction.sa_flags = 0;
  sigaction(SIGINT, &exitaction, NULL);
  sigaction(SIGTERM, &exitaction, NULL);

  int status;
  //wait for child processes to finish
  for (int i = 0; i < 8; i++){
    waitpid(pid[i], &status, 0);
  }

  freegenmem();
  freehandlemem();
  return 0;
}

//Function to detatch and deallocate shared memory
void parentexit(){
  int status;
  for (int i = 0; i < 8; i++){
    waitpid(pid[i], &status, 0);
  }
  freegenmem();
  freehandlemem();
  exit(0);
}
