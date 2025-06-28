#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef MAX_PID_LEN
#define MAX_PID_LEN 256
#endif

int compute_stupid_hash(const char *str)
{
  unsigned long long pw = 1, m = 5;
  const char *c = str;
  unsigned long long result = 0;

  while(*c != 0) {
    result = (result + (*c - 'a' + 1) * pw) % m;
    pw = (pw * 31) % m;
    c++;
  }

  return result;
}

int child_fn(const char *user)
{
  static char pid[MAX_PID_LEN];
  int len, hash;
  char *dst;
  unsigned long long dead = 0xdeadbeef;

  // get the pid in string format
  len = snprintf(pid, MAX_PID_LEN,"%d", getpid()); 
  if(len < 0) {
    perror("snprintf:");
    exit(255);
  }

  // allocate concat string
  dst = calloc(strlen(user) + strlen(pid) + 1, sizeof(char));
  strncpy(dst, user, strlen(user));
  strcat(dst, pid);

  printf("Child %d obtained string %s\n", getpid(), dst);
  hash = compute_stupid_hash(dst);
  printf("Child %d computed hash 0x%02x\n", getpid(), hash);

  // check if should crash
  if(hash == 4) {
    // crash with a segmentation fault
    *((int*)dead) = 3;
  }

  free(dst);
  return hash;
}
