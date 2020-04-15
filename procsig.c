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


int main(int argc, char *argv[]){

  //initialize all the shared variables and locks.
  initgen();
  inithandle();

  //create child processes
  pid_t pid[8];
  for(int i = 0; i < 8; i++){
    pid[i] = fork();
    if (pid[i] == 0){
      if (i < 3)
      //signal generators
        siggen();
      else if (i == 7){
      //signal reporter
        sigreporter();
      }
      else
      //signal handlers
        sighandle();
    }
  }
  int status;
  //wait for child processes to finish
  for (int i = 0; i < 8; i++){
    waitpid(pid[i], &status, 0);
  }

  printsigs();
  return 0;
}
