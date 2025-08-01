#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulator.h"

/*
 * CSSE332 Exam 2 Problem 1
 *
 * Author: Borui Liu
 */

#define NUM_THREADS 8

void *thread_func(void *arg) {
    int id = *(int *)arg;
    
    char name[32];
    snprintf(name, sizeof(name), "processor_%d", id + 1);
    
    char file[64];
    snprintf(file, sizeof(file), "files/test_instr%d.txt", id + 1);
    
    fprintf(stderr, "Thread %d: Simulating processor %s on file %s.\n", id, name, file);
    
    int count = simulate_cpu(id, name, (id + 1) * 100, file);
    
    fprintf(stderr, "Thread %d: Done.\n", id);

    int *ret = malloc(sizeof(int));
    if (!ret) {
        perror("malloc");
        pthread_exit(NULL);
    }
    *ret = count;
    return ret;
}

int
main(int argc, char **argv) {
  // =====
  // TODO:
  //  Add your code for problem 1 here...
  // =====
  //
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];
  
  for (int i = 0; i < NUM_THREADS; i++) {
      ids[i] = i;
      pthread_create(&threads[i], NULL, thread_func, &ids[i]);
  }

  long total = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    void *res;
    pthread_join(threads[i], &res);
    if (res) {
      total += *(int *)res;
      free(res);
    }
  }



  printf("=== The total number of instructions executed was %ld ===\n", total);
  return 0;
}
