#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t  c;
pthread_mutex_t m;
int done = 1;

void *thread(void *arg)
{

  int *num = (int *)arg;

  pthread_mutex_lock(&m);
  printf("%d wants to enter the critical section\n", *num);
  while(done != *num){
    pthread_cond_wait(&c, &m);  
  }
  printf("Thread %d in critical section\n", *num);
  done++;
  pthread_cond_broadcast(&c);
  pthread_mutex_unlock(&m);
  printf("%d is finished with the critical section\n", *num);

  return NULL;
}

int
main(int argc, char **argv)
{
  pthread_t threads[4];
  int i;
  int nums[] = {2, 1, 4, 3};


  for(i = 0; i < 4; ++i) {
    pthread_create(&threads[i], NULL, thread, &nums[i]);

    if(i == 2)
      sleep(3);
  }

  for(i = 0; i < 4; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("Everything finished\n");

  return 0;
}
