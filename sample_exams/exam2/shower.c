
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdbool.h>

#define SHOWER_TIME 2

/*

  So these threads do their best thinking in the shower.

  Threads want to use the shower but only one thread can use the
  shower at once.  Once in the shower a thread showers for at least
  SHOWER_TIME.  Then the thread checks to see if any other threads
  want to use the shower, if yes the thread finishes in the shower and
  exits.

  If no other threads want to use the shower, the thread will stay in
  the shower and begin the process again.  This means that a thread
  will never leave the shower unless another thread arrives.

  Hint: this problem can actually be solved with just mutexes, but a
  cv solution is also good.

  Example output:

thread 1 wants to use the shower
thread 1 is using shower
thread 1 decides to stay in the shower a little longer
thread 3 wants to use the shower
thread 2 wants to use the shower
thread 4 wants to use the shower
thread 1 is finished with shower
thread 3 is using shower
thread 3 is finished with shower
thread 2 is using shower
thread 2 is finished with shower
thread 4 is using shower
thread 4 decides to stay in the shower a little longer
thread 4 decides to stay in the shower a little longer
<would run forever but main quits>

 */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int waiting = 0;
int wash = 0;

void *thread(void *arg)
{
  int num = *(int*)arg;
  printf("thread %d wants to use the shower\n", num);
  // I reccommend you leave the above print statement outside of
  // any lock, otherwise a certian kind of bug will be less obvious
  pthread_mutex_lock(&mutex);
  waiting++;
  while(wash > 0) {
    printf("we have %d waiting threads\n", waiting);
    pthread_cond_wait(&cv, &mutex);
  }
  waiting--;
  pthread_mutex_unlock(&mutex);
  printf("thread %d is using shower\n", num);
  wash++;

  while(1) {
    
    sleep(SHOWER_TIME);
    // if(your check to see if another thread is waiting) {
      //    printf("thread %d is finished with shower\n", num);
      //    other code
      //    break;
      // }
      
      if(waiting == 0){
        printf("thread %d decides to stay in the shower a little longer\n", num);
      }else{
        pthread_mutex_lock(&mutex);
        printf("thread %d is finished with shower\n", num);
        wash--;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&mutex);
        break;
    }
  }
}



int main()
{

  pthread_t tid[100];
  int i = 0;
  int ids[] = {1, 2, 3, 4};


  pthread_create(&tid[i++],NULL, thread, (void*) &ids[0]);
  sleep(3);
  pthread_create(&tid[i++],NULL, thread, (void*) &ids[1]);
  pthread_create(&tid[i++],NULL, thread, (void*) &ids[2]);
  pthread_create(&tid[i++],NULL, thread, (void*) &ids[3]);
  sleep(10);

  //note that we don't wait for the last thread, otherwise this
  //program would never be done
  for(int j = 0; j < i - 1; j++) {
    pthread_join(tid[j],NULL);
  }


  return 0;
}
