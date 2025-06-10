/****************************************************************************
 * Copyright © 2022 Rose-Hulman Institute of Technology
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

#include "kernel/types.h"
#include "user/user.h"
#include "user/rhtest.h"

#define DEF_ARRAY_LIST_CAPACITY 4

/**
 * @brief An array list structure that can contain a list of integers.
 *
 * @var size          The current size of the array list.
 * @var capacity      The max size of the array list.
 * @var list          The actual list.
 */
struct arraylist {
  int size;
  int capacity;
  int *list;
};

/**
 * @brief Allocate and initialize a new array list.
 *
 * Allocate room for a new array list and set its size and capacity to the
 * default values. Be aware that the array list originally has a capacity of
 * DEF_ARRAY_LIST_CAPACITY, and thus must allocate room for the internal list as
 * well.
 *
 * @return a new array list.
 */
struct arraylist *al_new(void)
{
  /* DONE: Add your code here. */
  struct arraylist *al = malloc(sizeof(*al));
  if (!al)
    return 0;
  al->size = 0;
  al->capacity = DEF_ARRAY_LIST_CAPACITY;
  al->list = malloc(al->capacity * sizeof(int));
  if (!al->list) {
    free(al);
    return 0;
  }
  return al;
}

/**
 * @brief Free an allocated list.
 *
 * Free an allocated array by freeing the internal list first and then freeing
 * the structure itself.
 *
 * @warning The list is invalid after returning from this function.
 */
void al_free(struct arraylist *al)
{
  /* DONE: Add your code here. */
    if (!al){
	return;
    }
  if (al->list) {
     free(al->list);
    }
  free(al);
}

/**
 * @brief Get the element at position pos.
 *
 * Return the element at the given position pos.
 *
 * @param al         The array list.
 * @param pos        The position where to get.
 *
 * @return     Returns the element at position pos, 0xffffffff if invalid.
 */
int al_get_at(struct arraylist *al, int pos)
{
  /* DONE: Add your code here. */
    if (!al || pos < 0 || pos >= al->size){
    return 0xffffffff;
}
  return al->list[pos];
}

/**
 * @brief Resize an array list.
 *
 * Resize the array list by making sure the internal list grows double in size.
 * Note that all the elements in the list must be preserved, i.e., the original
 * content of the list must still be present in there after the resize.
 *
 * @param al         The array list.
 *
 * @warning This would be easy if we had access to the realloc libc function,
 * but we do not, so we must do things the old hard way.
 */
void al_resize(struct arraylist *al)
{
  /* DONE: Add your code here. */
  int newcap = al->capacity * 2;
  int *newlist = malloc(newcap * sizeof(int));
  if (!newlist){
    return;  
}
  memmove(newlist, al->list, al->size * sizeof(int));
  free(al->list);
  al->list = newlist;
  al->capacity = newcap;
}

/**
 * @brief Append a new element to the list.
 *
 * Add a new element at the end of the list. If the list is out of space, this
 * will cause a resize and add the element.
 *
 * @param al            The array list.
 * @param val           The value to append.
 *
 * @warning This function will cause the array list to increase in size. If the
 * array list goes over capacity, it will resize the array list and double the
 * capacity.
 */
void al_append(struct arraylist *al, int val)
{
  /* DONE: Add your code here. */
    if (al->size >= al->capacity){
	 al_resize(al);
    }
  al->list[al->size++] = val;
}

/**
 * @brief Print the elements of an array list.
 *
 * @param al         The array list to print.
 */
void al_print(struct arraylist *al)
{
  int i;

  printf("[");
  for(i = 0; i < al->size; i++) {
    printf("%d, ", al->list[i]);
  }
  printf("]\n");
}

int main(int argc, char **argv)
{
  struct arraylist *al;

  /* test creating a new array list */
  al = al_new();
  rhassert(al != 0);
  rhassert_int_equals(al->size, 0);
  rhassert_int_equals(al->capacity, DEF_ARRAY_LIST_CAPACITY);

  /* test appending two numbers */
  al_append(al, 1);
  al_append(al, 2);
  rhassert_int_equals(al->size, 2);
  rhassert_int_equals(al->capacity, DEF_ARRAY_LIST_CAPACITY);

  /* test getting an integer */
  rhassert_int_equals(al_get_at(al, 0), 1);
  rhassert_int_equals(al_get_at(al, 1), 2);
  rhassert_int_equals(al_get_at(al, 5), 0xffffffff);

  /* add more elements, check on size and capacity */
  al_append(al, 3);
  al_append(al, 4);
  rhassert_int_equals(al->size, 4);
  rhassert_int_equals(al->capacity, DEF_ARRAY_LIST_CAPACITY);

  /* check for automatic resizing */
  al_append(al, 5);
  rhassert_int_equals(al->size, 5);
  rhassert_int_equals(al->capacity, 2 * DEF_ARRAY_LIST_CAPACITY);

  /* check all elements are there after resize */
  rhassert_int_equals(al_get_at(al, 0), 1);
  rhassert_int_equals(al_get_at(al, 1), 2);
  rhassert_int_equals(al_get_at(al, 2), 3);
  rhassert_int_equals(al_get_at(al, 3), 4);
  rhassert_int_equals(al_get_at(al, 4), 5);

  exit(0);
}
