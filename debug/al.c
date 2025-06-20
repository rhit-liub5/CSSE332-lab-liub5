/****************************************************************************
 * Copyright © 2024 Rose-Hulman Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct array_list {
  char name[16];
  int size;
  int *array;
};

/** alloc_al() - Allocate an array list
  *
  * This function accepts a name and size and allocates room for an array list
  * with that name and that specific size, and returns a pointer to the
  * allocated array_list structure.
  *
  * After this function return, it should be safe for the user to use the
  * returned pointer in any way, to put data in the array list, and to modify
  * it.
  *
  * Task:
  * =====
  * The code below contains ONE crucial bug. Use gdb to find the bug and fix
  * it.
  */
struct array_list *alloc_al(const char *name, int size)
{
  struct array_list *al = malloc(sizeof(struct array_list));
  if(!al) {
    perror("PANIC: malloc failed, we didn't learn what to do here");
    exit(EXIT_FAILURE);
  }

  strcpy(al->name, name);
  al->size = size;
  al->array = malloc(size * sizeof(int));
  return al;
}

/**
  * The main function below allocate an array list, fills it with some data,
  * and then prints it. It will then go on to free the array list to return the
  * memory back to the operating system.
  *
  * Task:
  * =====
  * The code in main below contains another crucial bug. Use an appropriate
  * tool to find and fix that bug.
  */
int main(int argc, char **argv) {
  struct array_list *al = alloc_al("list", 16);

  // initialize the array list
  for(int i = 0; i < al->size; i++) {
    al->array[i] = i;
  }

  // print it out
  for(int i = 0; i < al->size; i++) {
    printf("%d ", al->array[i]);
  }
  printf("\n");

  // cleanup because we are nice
  free(al->array);
  free(al);

  exit(EXIT_SUCCESS);
}
