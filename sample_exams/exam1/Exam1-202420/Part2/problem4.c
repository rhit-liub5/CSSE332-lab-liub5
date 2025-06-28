#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Problem 4
//
// Name: 


// IMPORTANT: buffer size for use with ALL reads and writes
#define BUFSIZE 1000

int main(int argc, char **argv) {
  int rc;
  int fd[2];

  if (pipe(fd)<0) {
      perror("pipe failed");
      exit(1);
    }

  rc = fork();

  if(rc == 0){
    char readend[128], writeend[128];
    snprintf(readend, 128, "%d", fd[0]);
    snprintf(writeend, 128, "%d", fd[1]);
    execlp("./buffalosay.bin", "./buffalosay.bin", readend, writeend, NULL);
    perror("Error executing buffalosay.bin");
    exit(199);
  }

  close(fd[0]);

  char buffer[1000];
  memset(buffer, 0, 1000);
  snprintf(buffer, 1000, "%s", "secret handshake");

  write(fd[1], buffer, 1000);

  write(fd[1], argv[1], strlen(argv[1]));

  close(fd[1]);
  // Parent process
  int status;
  wait(&status);
  if(WIFEXITED(status)){
    printf("buffalosay exited with exit code %d\n", WEXITSTATUS(status));
  }


  printf("Parent %d finished ....\n", getpid());
  exit(0);
}



