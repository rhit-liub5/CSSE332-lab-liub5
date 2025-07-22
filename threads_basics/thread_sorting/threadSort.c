#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

// max number of values for each thread to sort
#define MAX_VALS_PER_THREAD 1000
// max threads allowed
#define MAX_N_SIZE 100


/* other global variable instantiations can go here */
int total_nums;
int* data_array;
int line;
int count = 0;
long Ftime[100];
long Btime[100];
long Mtime[100];
/* Uses a brute force method of sorting the input list. */
void BruteForceSort(int inputList[], int inputLength) {
  int i, j, temp;
  for (i = 0; i < inputLength; i++) {
    for (j = i+1; j < inputLength; j++) {
      if (inputList[j] < inputList[i]) {
        temp = inputList[j];
        inputList[j] = inputList[i];
        inputList[i] = temp;
      }
    }
  }
}

/* Uses the bubble sort method of sorting the input list. */
void BubbleSort(int inputList[], int inputLength) {
  char sorted = 0;
  int i, temp;
  while (!sorted) {
    sorted = 1;
    for (i = 1; i < inputLength; i++) {
      if (inputList[i] < inputList[i-1]) {
        sorted = 0;
        temp = inputList[i-1];
        inputList[i-1] = inputList[i];
        inputList[i] = temp;
      }
    }
  }
}

/* Merges two arrays.  Instead of having two arrays as input, it
 * merges positions in the overall array by re-ording data.  This 
 * saves space. */
void Merge(int *array, int left, int mid, int right) {
  int tempArray[MAX_VALS_PER_THREAD];
  int pos = 0, lpos = left, rpos = mid;
  while (lpos <= mid && rpos <= right) {
    if (array[lpos] < array[rpos]) {
      tempArray[pos++] = array[lpos++];
    } else {
      tempArray[pos++] = array[rpos++];
    }
  }
  while (lpos < mid)  tempArray[pos++] = array[lpos++];
  while (rpos <= right)tempArray[pos++] = array[rpos++];
  int iter;
  for (iter = 0; iter < pos; iter++) {
    array[iter+left] = tempArray[iter];
  }
  return;
}

/* Divides an array into halfs to merge together in order. */
void MergeSort(int array[], int inputLength) {
  int mid = inputLength/2;
  if (inputLength > 1) {
    MergeSort(array, mid);
    MergeSort(array + mid, inputLength - mid);
    // merge's last input is an inclusive index
    // printf("calling merge 0->%d, 1->%d\n mid %d\n",array[0], array[1], mid); 
    Merge(array, 0, mid, inputLength - 1);
  }
}

// you might want some globals, put them here

// here's a global I used you might find useful
char descriptions[] = {'F','B','M'};

// I wrote a function called thread dispatch which parses the thread
// parameters and calls the correct sorting function
//
// you can do it a different way but I think this is easiest
void* thread_dispatch(void* data) {
  char letter = *(char*)data;
  struct timeval startt, stopt;       
  long usecs_passed;
  int start = count*line;
  int end = (count+1)*line - 1;   
  int* sub = data_array + count*line;
  int id = count;
  if (letter == 'F'){
    count++;
    printf("Sorting indexes %d-%d with brute force\n", start, end);
    sleep(1);
    gettimeofday(&startt, NULL);    
    BruteForceSort(sub, line);
    gettimeofday(&stopt, NULL);
    usecs_passed = (stopt.tv_sec - startt.tv_sec) * 1000000 + (stopt.tv_usec - startt.tv_usec);
    Ftime[id/3] = usecs_passed;
    printf("Sorting indexes %d-%d with brute force done in %ld usecs\n", start, end, usecs_passed);
  } else if (letter == 'B') {
    sub = data_array + count*line;
    count++;
    printf("Sorting indexes %d-%d with bubble\n", start, end);
    sleep(1);
    gettimeofday(&startt, NULL);
    BubbleSort(sub, line);
    gettimeofday(&stopt, NULL);
    usecs_passed = (stopt.tv_sec - startt.tv_sec) * 1000000 + (stopt.tv_usec - startt.tv_usec);
    Btime[id/3] = usecs_passed;
    printf("Sorting indexes %d-%d with bubble done in %ld usecs\n", start, end, usecs_passed);
  } else if (letter == 'M') {
    sub = data_array + count*line;
    count++;  
    printf("Sorting indexes %d-%d with merge\n", start, end);
    sleep(1);
    gettimeofday(&startt, NULL);
    MergeSort(sub, line);
    gettimeofday(&stopt, NULL);
    usecs_passed = (stopt.tv_sec - startt.tv_sec) * 1000000 + (stopt.tv_usec - startt.tv_usec);
    Mtime[id/3] = usecs_passed;
    printf("Sorting indexes %d-%d with merge done in %ld usecs\n", start, end, usecs_passed);
  }
  return NULL;
}

double average(const long arr[], int len) {
    if (len <= 0) return 0.0;
    long sum = 0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return (double)sum / len;
}

long find_min(const long arr[], int len) {
    if (len <= 0) return 0;
    long m = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] < m) m = arr[i];
    }
    return m;
}

long find_max(const long arr[], int len) {
    if (len <= 0) return 0;
    long m = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] > m) m = arr[i];
    }
    return m;
}

int main(int argc, char** argv) {

  if(argc < 3) {
    printf("not enough arguments!\n");
    exit(1);
  }

  // I'm reading the value n (number of threads) for you off the
  // command line
  int n = atoi(argv[1]);
  if(n <= 0 || n > MAX_N_SIZE || n % 3 != 0) {
    printf("bad n value (number of threads) %d.  Must be a multiple of 3.\n", n);
    exit(1);
  }

  // I'm reading the number of values you want per thread
  // off the command line
  int vals_per_thread = atoi(argv[2]);
  if(vals_per_thread <= 0 || vals_per_thread > MAX_VALS_PER_THREAD) {
    printf("bad vals_per_thread value %d\n", vals_per_thread);
    exit(1);
  }

  total_nums = n * vals_per_thread;
  data_array = malloc(sizeof(int) * total_nums);
  line = vals_per_thread;

  if(data_array == NULL) {
    perror("malloc failure");
    exit(1);
  }

  // initialize the test data for sort
  for(int i = 0; i < total_nums; i++) {
    // big reverse sorted list
    data_array[i] = total_nums - i;
    // the test would be more traditional if we used random
    // values, but this makes it easier for you to visually
    // inspect and ensure you're sorting everything
  }

  // create your threads here
  pthread_t tids[n];

  for (int i = 0; i < n; i++) {
    pthread_create(&tids[i], NULL, thread_dispatch, &descriptions[i % 3]);
  }
  // wait for them to finish
  for (int i = 0; i < n; i++) {
      pthread_join(tids[i], NULL);  
  }
  // print out the algorithm summary statistics
  long Favg = average(Ftime, n/3);
  long Bavg = average(Btime, n/3);
  long Mavg = average(Mtime, n/3);
  long Fmin = find_min(Ftime, n/3);
  long Bmin = find_min(Btime, n/3);
  long Mmin = find_min(Mtime, n/3);
  long Fmax = find_max(Ftime, n/3);
  long Bmax = find_max(Btime, n/3);
  long Mmax = find_max(Mtime, n/3);
  printf("brute force avg %ld min %ld max %ld\n", 
        Favg, Fmin, Fmax);
  printf("bubble avg %ld min %ld max %ld\n", 
        Bavg, Bmin, Bmax);
  printf("merge avg %ld min %ld max %ld\n", 
        Mavg, Mmin, Mmax);  
  // print out the result array so you can see the sorting is working
  // you might want to comment this out if you're testing with large data sets
  printf("Result array:\n");
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < vals_per_thread; j++) {
      printf("%d ", data_array[i*vals_per_thread + j]);

    }
    printf("\n");
  }
  free(data_array); // we should free what we malloc
}
