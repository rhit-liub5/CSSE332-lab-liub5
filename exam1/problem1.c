#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// TODO: Replace this with your Rose username
#define EXAM_USER "liub5"

#define NUM_CHILDREN 5

/**
 * child_fn
 *
 * This is the function that the child should execute. It is implemented
 * elsewhere and you should only need to call it.
 *
 * DO NOT CHANGE THIS DECLARATION
 *
 * Refer to the instructions on ways to call this function.
 */
extern int child_fn(const char *);

/**
 * rc_to_str
 *
 * This function translates a return code from the child_fn function into a
 * string that can printed on the console.
 *
 * DO NOT CHANGE THIS DECLARATION
 *
 * @rc int
 *  The return code from the child_fn function.
 *
 * @return const char*
 *  Return a string representation of the return code of the child.
 */
const char* rc_to_str(int rc);

int
main(int argc, char **argv)
{

  // TODO:
  // =====
  //   Add your problem 1 code here.
  int child[NUM_CHILDREN];
  for (int i = 0; i < NUM_CHILDREN; i++) {
    int rc = fork();
    if (rc < 0) {
        perror("fork");
        exit(1);
    }
    // 子进程逻辑
    if (rc == 0) {

    int cl = child_fn(EXAM_USER);
    if(cl == NULL) {
      printf("we have problem on child_fun\n");
      exit(1);
    }
    exit(cl);
  }
  // 父进程逻辑
  child[i] = rc;
}
  //回收子进程
  for (int i = 0; i < NUM_CHILDREN; i++) {
    int status;
    int back = waitpid(child[i], &status, 0);
    if (WIFEXITED(status)) {
      int code = WEXITSTATUS(status);
      const char *text = rc_to_str(code);
      printf("Child %d returned %s.\n", child[i], text);
    }else if (WIFSIGNALED(status)) {
      printf("Child %d sadly terminated early.\n", child[i]);
    }
  }
}


//
// Implementation of the rc_to_str function.
//
// Feel free to read this function but DO NOT CHANGE IT.
//
const char* rc_to_str(int rc)
{
  switch(rc) {
    case 0:
      return "happily";
      break;
    case 1:
      return "sadly";
      break;
    case 2:
      return "neutral";
      break;
    case 3:
      return "surprised";
      break;
    default:
      return "unknown";
  }
}
