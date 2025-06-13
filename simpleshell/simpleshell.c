/* Copyright 2016 Rose-Hulman
   But based on idea from http://cnds.eecs.jacobs-university.de/courses/caoslab-2007/
   */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

void setsighandler(int signum, void (*handler)(int)) {
  struct sigaction act;

  act.sa_handler = handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART;
  sigaction(signum, &act, NULL);
}

void handle_sigchld(int ignored)
{
  /* TODO: Insert your code here! */
  // This code will execute whenever a child terminates and send its parent the
  // SIGCHLD signal.
    int rc;
    while ((rc = waitpid(-1, NULL, WNOHANG)) > 0) {

    }

}

void help(char *cmd, char *arg) {
    
    if (isdigit((unsigned char)*cmd)){
	int repeat = *cmd - '0';
	char *newcmd = cmd+1;
	int rcs[9];
	printf ("Will generate '%d' commands '%s'\n", repeat,newcmd);
	for (int i = 0; i<repeat; i++){
	   int rc = fork();
	   if (rc == 0){
		if (arg == NULL){
		    execlp(newcmd, newcmd, NULL);
		    exit(1);
		}else {
		    execlp(newcmd, newcmd, arg, NULL);
		    exit(1);
		}
	   }
	   rcs[i] = rc;
	}
	for (int k = 0; k < repeat; k++) {
	 int status;
	 waitpid(rcs[k], &status, 0);
	 printf("Command %s for child %d done..\n", newcmd, rcs[k]);
     }

     printf("All commands finished...\n");
	

    } else if (*cmd == 'B'|| *(cmd+1) == 'G'){
	int watcher = fork();
	if (watcher == 0){
	 char *newcmd = cmd+2;
	 int rc = fork();
	   if (rc == 0){
		if (arg == NULL){
		    execlp(newcmd, newcmd, NULL);
		}else {
	    	    execlp(newcmd, newcmd, arg, NULL);
		   }
      }
	waitpid(rc,NULL,0);
	printf("Background command finished\n");
	exit(0);
    }
    } else{
	int rc = fork();
	if (rc == 0){
	   if (arg == NULL){
	     execlp(cmd, cmd, NULL);
	     exit(1);
	    }else {
		execlp(cmd, cmd, arg, NULL);
		exit(1);
	    }
	} else if (rc > 0){
	    wait(NULL);
	}
    }

}



int main() {
  char command[82];
  char *parsed_command[2];
  //takes at most two input arguments
  // infinite loop but ^C quits
  while (1) {
    setsighandler(SIGCHLD, handle_sigchld);
    printf("SHELL%% ");
    fgets(command, 82, stdin);
    command[strlen(command) - 1] = '\0';//remove the \n
    int len_1;
    for(len_1 = 0;command[len_1] != '\0';len_1++){
      if(command[len_1] == ' ')
        break;
    }

    parsed_command[0] = command;

    if(len_1 == strlen(command)){
          printf("Command is '%s' with no arguments\n", parsed_command[0]);
	  parsed_command[1] = NULL;

      // leave this here, do not change it
     if(!strcmp(parsed_command[0], "quit") ||
          !strcmp(parsed_command[0], "exit")) {
        exit(EXIT_SUCCESS);
    }
    }else{
	 command[len_1] = '\0';
	 parsed_command[1] = command + len_1 + 1;
	 printf("Command is '%s' with argument '%s'\n", parsed_command[0], parsed_command[1]);

      // leave this here, do not change it
      if(!strcmp(parsed_command[0], "quit") ||
          !strcmp(parsed_command[0], "exit")) {
        exit(EXIT_SUCCESS);
      }
    }
   
    help(parsed_command[0], parsed_command[1]);
  }
}
