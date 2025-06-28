#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// TODO: Replace this with your Rose username
#define EXAM_USER "liub5"

// Feel free to use this as the default size of any buffer.
#define MAX_BUFF_LEN 128

/**
 * compute_stupid_math
 *
 * This function computes a stupid math operation on a string and an integer.
 *
 * YOU SHOULD NOT CHANGE THIS FUNCTION AT ALL.
 *
 * @param str: string
 *   The string to compute the operation on.
 *
 * @param num: int
 *   An integer representing a process id, obtained from getpid()
 *
 * @return: int
 *   Returns an integer between 0 and 4.
 */
int
compute_stupid_math(const char *str, int num)
{
  unsigned long long pw = 1, m = 5;
  const char *c = str;
  unsigned long long result = num;

  while(*c != 0) {
    result = (result + (*c - 'a' + 1) * pw) % m;
    pw = (pw * 31) % m;
    c++;
  }

  return result;
}

// 
// HINT:
// =====
//  To obtain the hexademical representation of an integer as a string, you can
//  use the following:
//
//    char buff[MAX_BUFF_LEN];
//    snprintf(buff, MAX_BUFF_LEN, "0x%02x", rc);
//
//  snprintf will return the actual string length, not include the null
//  terminator.
//

int
main(int argc, char **argv)
{
  // TODO:
  // =====
  //   Add your code for problem 3 here.
  //

  int rc;
  int ptc[2];
  int ctp[2];

  if (pipe(ptc)<0) {
      perror("pipe failed");
      exit(199);
    }
  if(pipe(ctp) < 0) {
    perror("pipe failed");
    exit(199);
  }
  rc = fork();

  //子进程
  if(rc == 0){
    close(ptc[1]);
    close(ctp[0]);
    char pbuff[128];
    int n = read(ptc[0], pbuff, 128);
    pbuff[5] = '\0';
    printf("Child: Hello %s, I will send you a number now!\n", pbuff);
    int rc = compute_stupid_math(pbuff, getpid());

    char massage[128];
    snprintf(massage, 128, "0x%02x", rc);
    for(int i = 0; i < rc; i++) {
      write(ctp[1],massage,128);
      sleep(1);
    }
    exit(0);    
  }

  close(ptc[0]);
  close(ctp[1]);

  char namebuff[128];
  snprintf(namebuff, 128, "%s\n", EXAM_USER);
  write(ptc[1], namebuff, 128);
  close(ptc[1]);

  char massagebuff[128];
  int i = 1;
  int m;
  while ((m = read(ctp[0], massagebuff, 128))>0){
    printf("Parent: %d My child sent me the number %s in response\n", i,massagebuff);
    i++;
  }
  close(ctp[0]);
  wait(NULL);
}
