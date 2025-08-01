#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
 * CSSE332 Exam 2 Problem 2
 *
 * Author: Borui Liu
 */

#define NUM_REQUESTS 10
#define PREPROCESS_CYCLE_MAX 4
#define DELIVERY_CYCLE_MAX 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cvre = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvwo = PTHREAD_COND_INITIALIZER;

int prepo = 0;
int isdv = 0;
int total = 0;

void *request_fn(void *arg)
{
  int id = *(int*)arg;
  printf("Request [ %d ] arrived...\n", id);

  pthread_mutex_lock(&mutex);
  while(prepo>=2){
    pthread_cond_wait(&cvre,&mutex);
  }
  prepo++;
  pthread_mutex_unlock(&mutex);
  // do the pre processing
  sleep(rand() % PREPROCESS_CYCLE_MAX);

  printf("Request [ %d ] finished preprocessing...\n", id);

  pthread_mutex_lock(&mutex);
  prepo--;
  if(prepo<2){
    pthread_cond_broadcast(&cvre);
  }
  isdv++;
  pthread_cond_broadcast(&cvwo);
  pthread_mutex_unlock(&mutex);

  return 0;
}

void *worker_fn(void *ignore)
{
  printf("Worker thread started...\n");

  while(total<10){
    pthread_mutex_lock(&mutex);
    while(isdv == 0){
      pthread_cond_wait(&cvwo, &mutex);
    }
    isdv--;
    pthread_mutex_unlock(&mutex);
    // simulate delivering the job.
    sleep(rand() % DELIVERY_CYCLE_MAX);
    printf("Worker thread delivered a new job, total deliveries: %d\n",total+1);

    pthread_mutex_lock(&mutex);
    total++;
    pthread_mutex_unlock(&mutex);

  }

  printf("Worker thread delivered all jobs...\n");
  return 0;
}

int
main(int argc, char **argv)
{
  srand(time(0));
  pthread_t worker_th, request_th[NUM_REQUESTS];
  int rids[NUM_REQUESTS];
  int i;

  pthread_create(&worker_th, 0, worker_fn, 0);

  for(i = 0; i < NUM_REQUESTS / 2; i++) {
    rids[i] = i;
    pthread_create(&request_th[i], 0, request_fn, &rids[i]);
  }

  sleep(10);

  for(i = NUM_REQUESTS/2; i < NUM_REQUESTS; i++) {
    rids[i] = i;
    pthread_create(&request_th[i], 0, request_fn, &rids[i]);
  }

  pthread_join(worker_th, 0);
  for(i = 0; i < NUM_REQUESTS; i++) {
    pthread_join(request_th[i], 0);
  }

  printf("\n========== Everything Finished ===========\n");
  return 0;
}
