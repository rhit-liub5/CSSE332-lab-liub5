#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Problem 1
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
      char buf[32];
      snprintf(buf, sizeof(buf), "%d", parent_pid);
      execl("./pretty.bin", "./pretty.bin", buf,
            "Be brave and never give up!", NULL);
      perror("execl failed");
      exit(1);
    }
    pids[i] = pid;
  }

  for (int i = 0; i < NUM_CHILDREN; i++) {
    int status;
    waitpid(pids[i], &status, 0);
    if (WIFEXITED(status)) {
      int exit_status = WEXITSTATUS(status);
      printf("Child %d with PID %d exited with status %d\n", i, pids[i], exit_status);
    } else {
      printf("Child %d with PID %d crashed!\n", i,pids[i]);
    }

  }


  printf("Parent %d finished ....\n", getpid());
  exit(0);
}



