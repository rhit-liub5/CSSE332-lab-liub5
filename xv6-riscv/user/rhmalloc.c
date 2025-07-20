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
static metadata_t *free_lists[22];

/**
 * Record the original start of our memory area in case we needed to allocate
 * more stuff to align our target start address.
*/
static void *original_start = 0;

/**
 * For testing purposes, we need to record where our memory starts. Generally
 * this is hidden from the users of the library but we're just using it here to
 * make our tests more meaningful.
 */
static void *heap_mem_start = 0;

/**
 * Check if the freelist has been initialized, if not, call rhmalloc_init()
*/
static int initialized = 0;

/**
 * For testing purposes, exposed the initialization bit.
*/
int is_initialized(void) { return initialized; }

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
  original_start = sbrk(0);

  /* keep allocating useless crap until we hit our target starting address */
  p = sbrk(TARGET_START - original_start);

  /* grow the memory area by MAX_HEAP_SIZE bytes */
  p = sbrk(MAX_HEAP_SIZE);
  if(p == (char *)-1) {
    fprintf(2, "sbrk failed:exiting....\n");
    exit(1);
  }

  /* check if we matched the target start */
  if(p != TARGET_START) {
    fprintf(2, "sbrk failed: cannot get to target start\n");
    exit(1);
  }

  /* check if lower 21 bits are all zeros */
  if((uint64)p & (uint64)0x1FFFFF) {
    fprintf(2, "sbrk failed: cannot get good start of memory...\n");
    exit(1);
  }

  /* update the heap start */
  heap_mem_start = p;

  // mark it as initialized
  initialized = 1;

  // TODO: Add your initialization code here, but do not change anything above
  // this line.
  for(int k = 0; k <= 22; k++){
    free_lists[k] = NULL;
  }

  metadata_t *b = (metadata_t*)heap_mem_start;
  b->size   = 1 << 22; 
  b->in_use = 0;
  b->next   = b->prev = NULL;

  free_lists[22] = b;
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

  /* move back if we did have to allocate more stuff */
  sbrk(-(TARGET_START - original_start));

  heap_mem_start = 0;
  original_start = 0;
  initialized = 0;

  // TODO: Add your destruction code here, but do not change anything above this
  // line.

}

/**
 * Grab the pointer to the buddy region, given a pointer to a memory chunk of a
 * given size.
 * 
 * @param ptr               A pointer to a given memory chunk of a given size.
 * @param exponent          The exponent representing the size. 
 * 
 * @return A pointer to the buddy region, i.e., the adjacent region of the same
 * size with only 1 bit difference.
 * 
*/
void *get_buddy(void *ptr, int exponent)
{
  // DONE: Add your code here.
    uint64 addr = (uint64)ptr;
    uint64 mask = 1ULL << exponent;
    uint64 buddy_addr = addr ^ mask;  
  return (void*)buddy_addr;
}

/**
 * Allocate size bytes and return a pointer to start of the region. 
 * 
 * @return A valid void ptr if there is enough room, 0 on error. 
 */
int findexp(uint32 size){
  int exp = 5;
  while(size+8 > (1u << exp)) {
    exp++;
  }
  return exp;
}

void *rhmalloc(uint32 size)
{
  /* Check if we need to call rhmalloc_init and call it if needed. */
  if(!initialized)
    if(rhmalloc_init()) return 0;

  // TODO: Add your malloc code here.
  // 查找我们需要的大小的指数
  int exp = findexp(size);
  if (exp > 22){
    
    return (void*)0;
  }

  // 我们查找最近的有空闲位置
  int free = exp;
  while (free <= 22 && free_lists[free] == NULL){
    free++;
  }
  if (free > 22){
    return (void*)0;
  }

  // 拿出空闲链表进行重新分配
  metadata_t *a = free_lists[free];
  free_lists[free] = a->next;
  if (a->next){
    a->next->prev = NULL;
  }

  // 拆分盒子
  for(int k = free; k > exp; k--){
    metadata_t *buddy = (metadata_t*)((char*)a + (1 << (k-1)));
    buddy->size = 1 << (k-1);
    buddy->in_use = 0;
    buddy->next = free_lists[k-1];
    if (free_lists[k-1] != NULL) {
      free_lists[k-1]->prev = buddy;
    }
    buddy->prev = NULL;
    free_lists[k-1] = buddy;
    a->size = 1 << (k-1);
  }
  a->in_use = 1;

  return (void*)((char*)a + 8);
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
  // TODO: Add your free code here.
  metadata_t *hdr = (metadata_t*)((char*)ptr - 8);

  hdr->in_use = 0;
  hdr->next = NULL; 
  hdr->prev = NULL;

  int exp = findexp(hdr->size-8);


  while (exp < 22) {
    metadata_t *buddy = (metadata_t*)get_buddy(hdr, exp);
    if (buddy->in_use || buddy->size != hdr->size) {
      break;
    }              
    if (buddy->next) {
      buddy->next->prev = buddy->prev;
    }
    if (buddy->prev) {
      buddy->prev->next = buddy->next;
    } else {
      free_lists[exp] = buddy->next;
    }
    if (buddy < hdr) {
      hdr = buddy;
    } 
    hdr->size <<= 1;  
    exp++;            
  }

  hdr->next = free_lists[exp];
  if (free_lists[exp]) {
    free_lists[exp]->prev = hdr;
  }
  hdr->prev = NULL;
  free_lists[exp] = hdr;  
}



