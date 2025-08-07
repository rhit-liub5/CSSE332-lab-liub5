#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void mythread(void *arg) {
  printf("mythread running with arg=%p\n", arg);
}

int main() {
  thread_create(mythread, (void*)0x1234);
  thread_join(1);
  exit(0);
}
