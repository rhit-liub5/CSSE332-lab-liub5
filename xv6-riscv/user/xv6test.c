#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/cool_threads.h"

int g = 0;

// static void worker(void *arg) {
//   int me = (int)(uint64)arg;

//   printf("I am worker %d",me);

//   sleep(10);

//   printf("worker %d finish work",me);
// }

// int
// main(int argc, char *argv[])
// {
//   printf("[ct_share] start\n");

//   int t1 = thread_create(worker, (void*)1);
//   int t2 = thread_create(worker, (void*)2);
//   printf("[ct_share] created t1=%d t2=%d\n", t1, t2);

//   sleep(10);

//   printf("[ct_share] main sees g=%d\n", g);

//   exit(0);
// }

static void worker(void *arg) {
  int me = (int)(uint64)arg;
  g += me;
  printf("I am %d current %d\n", me, g);
  for(;;)
  sleep(1000); 
}

int main() {
  ct_create(worker, (void*)10);
  sleep(50);
  ct_create(worker, (void*)20);
  sleep(1000);
  exit(0);
}