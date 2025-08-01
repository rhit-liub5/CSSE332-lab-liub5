#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * CSSE332 Exam 2 Problem 2
 *
 * Author: Borui Liu
 */

// constants
#define THREAD_A_CYCLE 2
#define THREAD_B_CYCLE 3
#define THREAD_C_CYCLE 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cvA = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvB = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvC = PTHREAD_COND_INITIALIZER;

// A=1, B=2， C=3
int nowtype = 0;
int activity = 0;
int waitingA = 0;
int waitingB = 0;
int waitingC = 0;

void *threadA(void *arg)
{
  int tid = *(int*)arg;
  int type = 1;
  printf("Thread A[%d] started...\n", tid);
  waitingA++;
  // TODO: Add your code here...
  pthread_mutex_lock(&mutex);
  while(nowtype!= type && nowtype!=0){
    pthread_cond_wait(&cvA, &mutex);
  }
  nowtype = type;
  waitingA--;
  activity++;
  pthread_mutex_unlock(&mutex);

  // do stuff, simulate critical section
  printf("Thread A[%d] hanging out in critical section.\n", tid);
  sleep(THREAD_A_CYCLE);


  printf("Thread A[%d] has left the critical section.\n", tid);

  pthread_mutex_lock(&mutex);
  activity--;
  if (activity == 0){
    nowtype = 0;
    if (waitingB > 0){
      pthread_cond_broadcast(&cvB);
    }else {
      pthread_cond_broadcast(&cvC);
    }
  }
  pthread_mutex_unlock(&mutex);

  return 0;
}

void *threadB(void *arg)
{
  int tid = *(int*)arg;
  int type = 2;

  printf("Thread B[%d] started...\n", tid);
  waitingB++;
  // TODO: Add your code here...
  pthread_mutex_lock(&mutex);
  while(nowtype!= type && nowtype!=0){
    pthread_cond_wait(&cvB, &mutex);
  }
  nowtype = type;
  waitingB--;
  activity++;
  pthread_mutex_unlock(&mutex);

  // do stuff, simulate critical section
  printf("Thread B[%d] hanging out in critical section.\n", tid);
  sleep(THREAD_B_CYCLE);

  printf("Thread B[%d] has left the critical section.\n", tid);

  pthread_mutex_lock(&mutex);
  activity--;
  if (activity == 0){
    nowtype = 0;
    if (waitingA > 0){
      pthread_cond_broadcast(&cvA);
    }else {
      pthread_cond_broadcast(&cvC);
    }
  }
  pthread_mutex_unlock(&mutex);
  return 0;
}

void *threadC(void *arg)
{
  int tid = *(int*)arg;
  int type = 3;

  printf("Thread C[%d] started...\n", tid);
  waitingC++;
  // TODO: Add your code here...
  pthread_mutex_lock(&mutex);
  while(nowtype!= type && nowtype!=0){
    pthread_cond_wait(&cvC, &mutex);
  }
  nowtype = type;
  waitingC--;
  activity++;
  pthread_mutex_unlock(&mutex);
  
  // do stuff, simulate critical section
  printf("Thread C[%d] hanging out in critical section.\n", tid);
  sleep(THREAD_C_CYCLE);

  printf("Thread C[%d] has left the critical section.\n", tid);

  pthread_mutex_lock(&mutex);
  activity--;
  if (activity == 0){
    nowtype = 0;
    if (waitingA > 0){
      pthread_cond_broadcast(&cvA);
    }else {
      pthread_cond_broadcast(&cvB);
    }
  }
  pthread_mutex_unlock(&mutex);
  return 0;
}

#define create_thread(th, ids, num, fn) \
  for(int i = 0; i < num; i++) { \
    ids[i] = i; \
    pthread_create(&th[i], 0, fn, &ids[i]);  \
  }

#define join_threads(th, num) \
  for(int i = 0; i < num; i++) {\
    pthread_join(th[i], 0); \
  } 

#define create_partial_thread(th, ids, start, end, fn) \
  for(int i = start; i < end; i++) {\
    ids[i] = i; \
    pthread_create(&th[i], 0, fn, &ids[i]); \
  }

void test1(void)
{
  printf("\n======= TEST 1 STARTING =======\n");
  const int NUM_TA = 5;
  const int NUM_TB = 8;
  const int NUM_TC = 10;

  pthread_t tA[NUM_TA];
  int tAid[NUM_TA];

  pthread_t tB[NUM_TB];
  int tBid[NUM_TB];

  pthread_t tC[NUM_TC];
  int tCid[NUM_TC];

  create_thread(tA, tAid, NUM_TA, threadA);
  create_thread(tB, tBid, NUM_TB, threadB);
  create_thread(tC, tCid, NUM_TC, threadC);

  join_threads(tA, NUM_TA);
  join_threads(tB, NUM_TB);
  join_threads(tC, NUM_TC);

  printf("\n======= TEST 1 FINISHED =======\n");
}

void test2(void)
{
  printf("\n======= TEST 2 STARTED =======\n");
  const int NUM_TA = 5;
  const int NUM_TB = 8;
  const int NUM_TC = 10;

  pthread_t tA[NUM_TA];
  int tAid[NUM_TA];

  pthread_t tB[NUM_TB];
  int tBid[NUM_TB];

  pthread_t tC[NUM_TC];
  int tCid[NUM_TC];

  // start with a bunch of B's
  create_partial_thread(tB, tBid, 0, 4, threadB);
  // then a bunch of A's
  create_partial_thread(tA, tAid, 0, 2, threadA);
  // then all of the C's
  create_thread(tC, tCid, NUM_TC, threadC);

  usleep(500);
  // then remaining A's
  create_partial_thread(tA, tAid, 2, NUM_TA, threadA);
  // then remaining B's
  create_partial_thread(tB, tBid, 4, NUM_TB, threadB);

  join_threads(tA, NUM_TA);
  join_threads(tB, NUM_TB);
  join_threads(tC, NUM_TC);

  printf("\n======= TEST 2 FINISHED =======\n");
}

void test3(void)
{
  printf("\n======= TEST 3 STARTED =======\n");

  srand(time(0));
  const int NUM_THREADS = 10;
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  void *(*fn[])(void*) = { threadA, threadB, threadC };

  for(int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i;
    int which = rand() % 3;
    pthread_create(&threads[i], 0, fn[which], &ids[i]);
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], 0);
  }

  printf("\n======= TEST 3 FINISHED =======\n");
}

int
main(int argc, char **argv)
{
  int test = -1;
  if(argc > 1) {
    test = atoi(argv[1]);
    switch(test) {
      case 1:
      test1();
        return 0;
      break;
      case 2:
      test2();
        return 0;
      break;
      case 3:
      test3();
        return 0;
      break;
      default:
      break;
    }
  } 
  test1();
  test2();
  test3();
  return 0;
}
