#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
  const char *user;
  int pizza;

  if(argc < 2) {
    printf("You do not know how to run p2worker.bin!\n");
    exit(128);
  }
  user = argv[1];
  srand(time(0));

  // TODO: check if a file descriptor is passed through.
  printf("p2worker: Hello %s, how are you?\n", user);

  pizza = rand() % 4;
  if(pizza <= 2) {
    sleep(pizza);
    printf("p2worker: Yumm, the pizza is so good, thank you %s!\n", user);
    exit(pizza);
  } else if(pizza == 3) {
    printf("p2worker: Who puts pineapple on a pizza, I refuse to eat!\n");
    while(1);
  } else {
    printf("p2worker: Fatal error happened, will segfault myself\n");
    *((int*)0xdeadbeef) = 3;
  }
}
