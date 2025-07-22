/* Copyright 2016 Rose-Hulman Institute of Technology

   Here is some code that factors in a super dumb way.  We won't be
   attempting to improve the algorithm in this case (though that would be
   the correct thing to do).

   Modify the code so that it starts the specified number of threads and
   splits the computation among them.  You can be sure the max allowed 
   number of threads is 50.  Be sure your threads actually run in parallel.

   Your threads should each just print the factors they find, they don't
   need to communicate the factors to the original thread.

   ALSO - be sure to compile this code with -pthread or just used the 
   Makefile provided.

*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

unsigned long long target;
long range;
long left;

void *factor_function(void *arg) {
  int times = *((int *)arg);
  int realtimes = times - 1;
  long st = range * realtimes;
  long end = st + range;
  if (st == 0){
    st = 2;
  }
  if (end + left == target/2) {
    end = end + left + 1;
  }
  for (unsigned long long i = st; i < end; i = i + 1) {
    /* You'll want to keep this testing line in.  Otherwise it goes so
       fast it can be hard to detect your code is running in
       parallel. Also test with a large number (i.e. > 3000) */
    printf("thread %d testing %llu\n", times,i);
    if (target % i == 0) {
      printf("%llu is a factor\n", i);
    }
  }
  return NULL;
}

int main(void) {
  /* you can ignore the linter warning about this */
  int numThreads;

  printf("Give a number to factor.\n");
  scanf("%llu", &target);

  printf("How man threads should I create?\n");
  scanf("%d", &numThreads);
  range = target / (numThreads*2);
  left = target % numThreads;
  if (numThreads > 50 || numThreads < 1) {
    printf("Bad number of threads!\n");
    return 0;
  }
  pthread_t tids[numThreads];

  for (int k = 0; k < numThreads; k++) {
    pthread_create(&tids[k], NULL, factor_function, &k);
  }

  for (int k = 0; k < numThreads; k++) {
    pthread_join(tids[k], NULL);
  }
  return 0;
}


