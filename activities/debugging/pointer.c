#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int i;
  int *p = malloc(10 * sizeof(int));

  // fill array
  for (i = 0; i < 10; i++) {
    p[i] = i;
  }

  // print array in reverse order.
  for(i = 9; i >= 0; i--) {
    printf("index: %d, value: %d\n", i, p[i]);
  }

  free(p);

  exit(EXIT_SUCCESS);
}
