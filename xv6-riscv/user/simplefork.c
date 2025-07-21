//
// tests for step 2 of copy-on-write fork lab
//

#include "kernel/memlayout.h"
#include "kernel/types.h"
#include "user/user.h"

// allocate more than half of physical memory,
// then fork. this will fail in the default
// kernel, which does not support copy-on-write.
void simpletest() {
  uint64 phys_size = PHYSTOP - KERNBASE;
  int sz = (phys_size / 3) * 2;

  printf("simple: ");

  char *p = sbrk(sz);
  if(p == (char *)0xffffffffffffffffL) {
    printf("sbrk(%d) failed\n", sz);
    exit(-1);
  }

  for(char *q = p; q < p + sz; q += 4096) {
    *(int *)q = getpid();
  }

  int pid = fork();
  if(pid < 0) {
    printf("fork() failed\n");
    exit(-1);
  }

  if(pid == 0)
    exit(0);

  // only difference is that we do no call wait since wait calls copyout and
  // that breaks things for us.
  // UNCOMMENT THIS FOR STEP 3
  wait(0);

  if(sbrk(-sz) == (char *)0xffffffffffffffffL) {
    printf("sbrk(-%d) failed\n", sz);
    exit(-1);
  }

  printf("ok\n");
}

int
main(int argc, char **argv)
{
  simpletest();

  // UNCOMMENT THIS FOR STEP 3
  simpletest();

  exit(0);
}
