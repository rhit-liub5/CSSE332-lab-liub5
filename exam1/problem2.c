#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

// TODO: Replace this with your Rose username
#define EXAM_USER "liub5"

#define GRACE_PERIOD 5

// TODO:
// =====
//  Add any global variables or function definitions here.

static int worker_pid;                 
static int timed_out;

void handle(int sig) {
    if (worker_pid > 0) {
        kill(worker_pid, SIGKILL);
    }
    timed_out = 1;
}

int main(int argc, char **argv) {
    int parent_pid = getpid();

    signal(SIGALRM, handle);

    worker_pid = fork();

    if (worker_pid < 0) {
        perror("fork");
        exit(1);
    }
    if (worker_pid == 0) {
        execlp("./p2worker.bin", "./p2worker.bin", EXAM_USER, (char*)NULL);
        perror("execl failed");
        exit(1);
    }

    alarm(5);

    int status;
    waitpid(worker_pid, &status, 0);

    if (timed_out) {
        printf("p2worker timed out because they do not like pineapple on pizza!\n");
    } else if (WIFEXITED(status)) {
        int es = WEXITSTATUS(status);
        printf("p2worker ate the pizza and exited with exit status %d\n", es);
    }
  }
