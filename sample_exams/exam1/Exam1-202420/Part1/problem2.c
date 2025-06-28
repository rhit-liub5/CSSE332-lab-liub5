#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Problem 2
//
// Name: 


// Default buffer size for use with snprintf
#define BUFSIZE 128
#define NUM_CHILDREN 10

int main(int argc, char **argv) {
  int pids[NUM_CHILDREN];

  for (int i = 0; i < NUM_CHILDREN; i++) {
    int parent_pid = getpid();
    int pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }
    if (pid == 0) {
      int self = getpid();
      if (self % 4 == 0) {
          printf("Child %d is bad, will not execute.\n", self);
          exit(89);
      }
      alarm(5);
      char buf[32];
      snprintf(buf, sizeof(buf), "%d", parent_pid);
      execlp("./pretty.bin", "./pretty.bin", buf,
            "Be brave and never give up!", NULL);
      perror("execl failed");
      exit(1);
    }
    if(pid%4 == 0){
      waitpid(pid, 0, 0);
      i--;
    }else{
      pids[i] = pid;
    }
  }

  for (int i = 0; i < NUM_CHILDREN; i++) {
    int status;
    waitpid(pids[i], &status, 0);
    if (WIFEXITED(status)) {
      if(WEXITSTATUS(status)==99){
        printf("Child %d timed out!\n", pids[i]);
      }else{
        int exit_status = WEXITSTATUS(status);
        printf("Child %d with PID %d exited with status %d\n", i, pids[i], exit_status);
      }
        }else {
            printf("Child %d with PID %d crashed!\n", i,pids[i]);
        }

  printf("Parent %d finished ....\n", getpid());
  exit(0);
  }
}

