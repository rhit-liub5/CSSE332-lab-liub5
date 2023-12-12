#include "kernel/types.h"
#include "user/user.h"

void meminfo(int size)
{
  char *p;
  uint64 va = 0;
  uint64 i;

  p = malloc(size);

  printf(" Virtual Address   \t\t  Physical Address\n");
  printf("------------------ \t\t -------------------\n");
  for(i = (uint64)p; i < (uint64)(p + size); i += 4096) {
    if(getmaping(i, (uint64)&va) < 0) {
      printf("[ERROR] Cannot call the getmaping system call\n");
      goto leave;
    }
    printf("%p \t\t %p\n", (char*)i, (char*)va);
  }

  printf("\n");
  getmaping(i, (uint64)&va);
  printf("Checking for invalid address %p -->  %p\n", (char*)i, (char*)va);

leave:
  free(p);
  return;
}

int main(int argc, char **argv)
{
  int size, rc;

  if(argc != 2) {
    printf("[usage] %s <num bytes to allocate>\n", argv[0]);
    exit(1);
  }
  size = atoi(argv[1]);

  if((rc = fork()) == 0) {
    printf("############### CHILD (%d) ###############\n", getpid());
    meminfo(size);
    printf("############### CHILD (%d) ###############\n\n", getpid());
    exit(0);
  }
  // make sure we don't print over each other
  wait(0);

  printf("############### PARENT (%d) ###############\n", getpid());
  meminfo(size);
  printf("############### PARENT (%d) ###############\n\n", getpid());

  exit(0);
}
