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
#include "user/rhmalloc.h"
#include <stddef.h>


/**
 * For testing purposes, we need to record where our memory starts. Generally
 * this is hidden from the users of the library but we're just using it here to
 * make our tests more meaningful.
 */
static void *heap_mem_start = 0;

/**
 * Head of the free list. It is actually a pointer to the header of the first
 * free block.
 *
 * @warning In this assignment, "freelist" is somewhat of a misnomer, because
 * this list contains both free and unfree nodes.
 */
static metadata_t *freelist = 0;

/**
 * Return a pointer to the metadata of the head of the free list.
 *
 * @return The freelist pointer.
 */
metadata_t *freelist_head(void) { return freelist; }

/**
 * Return the pointer to the start of the heap memory.
 * 
 * @return The heam_mem_start ptr.
 */
void *heap_start(void) { return heap_mem_start; }

/**
 * Initialize the rh memroy allocator system.
 *
 * @return 0 on success, non-zero number on failure.
 */
uint8 rhmalloc_init(void)
{
  char *p;

  /* Grab the start of the memory where we are allocating. */
  heap_mem_start = sbrk(0);

  /* grow the memory area by MAX_HEAP_SIZE bytes */
  p = sbrk(MAX_HEAP_SIZE);
  if(p == (char *)-1) {
    fprintf(2, "sbrk failed:exiting....\n");
    exit(1);
  }

  /* TODO: Add code here to initialize freelist and its content. */
  freelist = (metadata_t*) heap_mem_start;
  freelist->size   = MAX_HEAP_SIZE - sizeof(metadata_t);
  freelist->in_use = 0;
  freelist->next   = NULL;
  freelist->prev   = NULL;

  return 0;
}

/**
 * Deallocates everything and frees back all the memory to the operating system.
 *
 * This routine is useful to do between tests so that we can reset everything.
 * You should not need to modify this routine though if you use global
 * variables, it might be useful to reset their values here.
 */
void rhfree_all(void)
{
  /* Imagine what would happen on a double free, yikes! */
  sbrk(-MAX_HEAP_SIZE);

  freelist = 0;
  heap_mem_start = 0;
}

/**
 * Allocate size bytes and return a pointer to start of the region. 
 * 
 * @return A valid void ptr if there is enough room, 0 on error. 
 */


void *rhmalloc(uint32 size)
{
  /* Check if we need to call rhmalloc_init and call it if needed. */
  metadata_t *block;
  uint32      Rsize;
  if(!freelist)
    if(rhmalloc_init()) return 0;

  /* TODO: Add you malloc code here. */
  Rsize = ALIGN(size);
  block = freelist;
  while (block != NULL) {
    if (block->in_use == 0 && block->size >= Rsize) {
        break;
    }
    block = block->next;
  }

  if (block == NULL) {
    return 0;
  }

  if (block->size >= Rsize + sizeof(metadata_t) + ALIGNMENT) {
    metadata_t *newblk = (metadata_t*)((char*)block
                           + sizeof(metadata_t)
                           + Rsize);
    newblk->size = block->size - Rsize - sizeof(metadata_t);
    newblk->next = block->next;
    newblk->prev = block;
    newblk->in_use = 0;

    if (block->next){
      block->next->prev = newblk;
    }
    block->next = newblk;
    block->size = Rsize;

  }
  
  block->in_use = 1;

  return (void*)((char*)block + sizeof(metadata_t));
}

/**
 * Free a memory region and return it to the memory allocator.
 *
 * @param ptr The pointer to free.
 *
 * @warning This routine is not responsible for making sure that the free
 * operation will not result in an error. If freeing a pointer that has already
 * been freed, undefined behavior may occur.
 */
void rhfree(void *ptr)
{
  /* TODO: Add your free code here. */
    metadata_t *block;
    metadata_t *next;
    metadata_t *prev;
    block = (metadata_t *)((char *)ptr - sizeof(metadata_t));
    block->in_use = 0;
    next = block->next;

    if (next && !next->in_use) {
        block->size += sizeof(metadata_t) + next->size;
        block->next = next->next;
        if (next->next){
            next->next->prev = block;
        }
    }
    prev = block->prev;
    if (prev && !prev->in_use) {
        prev->size += sizeof(metadata_t) + block->size;
        prev->next = block->next;
        if (block->next){
            block->next->prev = prev;
        }
    } 
}
