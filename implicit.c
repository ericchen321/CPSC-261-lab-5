#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#include "implicit.h"

/*
 * Determine whether or not a block is in use.
 */
static inline int block_is_in_use(void *block_start)
{
  return 1 & *((block_size_t *) block_start);
}

/*
 * Return the size of a block.
 */
static inline block_size_t get_block_size(void *block_start)
{
  return -HEADER_SIZE & *((block_size_t *) block_start);
}

/*
 * Return the size of the payload of a block.
 */
static inline block_size_t get_payload_size(void *block_start)
{
  return get_block_size(block_start) - HEADER_SIZE * 2;
}

/*
 * Find the start of the block, given a pointer to the payload.
 */
static inline void *get_block_start(void *payload)
{
  return payload - HEADER_SIZE;
}

/*
 * Find the payload, given a pointer to the start of the block.
 */
static inline void *get_payload(char *block_start)
{
  return block_start + HEADER_SIZE;
}

/*
 * Set the size of a block, and whether or not it is in use. Remember each block
 * has two copies of the header (one at each end).
 */
static inline void set_block_header(void *block_start, block_size_t block_size, int in_use)
{
  block_size_t header_value = block_size | in_use;
  *((block_size_t *) block_start) = header_value;
  *((block_size_t *) (get_payload(block_start) +
		      get_payload_size(block_start))) = header_value;
}


/*
 * Find the start of the next block.
 */
static inline void *get_next_block(void *block_start)
{
  return block_start + get_block_size(block_start);
}

/*
 * Find the start of the previous block.
 */
static inline void *get_previous_block(void *block_start)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return NULL;
}

/*
 * Determine whether or not the given block is at the front of the heap.
 */
static inline int is_first_block(heap *h, void *block_start)
{
  return block_start == h->start;
}

/*
 * Determine whether or not the given address is inside the heap
 * region. Can be used to loop through all blocks:
 *
 * for (blk = h->start; is_within_heap_range(h, blk); blk = get_next_block(blk)) ...
 */
static inline int is_within_heap_range(heap *h, void *addr)
{
  return addr >= h->start && addr < h->start + h->size;
}

/*
 * Coalesce a block with its consecutive block, only if both blocks are free.
 * Return a pointer to the beginning of the coalesced block.
 */
static inline void *coalesce(heap *h, void *first_block_start)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return NULL;
}

/*
 * Determine the size of the block we need to allocate given the size
 * the user requested. Don't forget we need space for the header  and
 * footer, and that the user size may not be aligned.
 */
static inline block_size_t get_size_to_allocate(block_size_t user_size)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return 0;
}

/*
 * Turn a free block into one the user can utilize. Split the block if
 * it's more than twice as large or MAX_UNUSED_BYTES bytes larger than
 * needed.
 */
static inline void *prepare_block_for_use(void *block_start, block_size_t real_size)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return NULL;
}

/*
 * Create a heap that is "size" bytes large, including its header.
 */
heap *heap_create(intptr_t size, search_alg_t search_alg)
{
  /* Allocate space in the process' actual heap */
  void *heap_start = sbrk(size);
  if (heap_start == (void *) -1) return NULL;
  
  /* Use the first part of the allocated space for the heap header */
  heap *h = heap_start;
  heap_start += sizeof(heap);
  size -= sizeof(heap);
  
  /* Ensures the heap_start points to an address that has space for
     the header, while allowing the payload to be aligned to PAYLOAD_ALIGN */
  int delta = PAYLOAD_ALIGN - HEADER_SIZE -
    ((uintptr_t) heap_start) % PAYLOAD_ALIGN;
  if (delta < 0)
    delta += PAYLOAD_ALIGN;
  if (delta > 0) {
    heap_start += delta;
    size -= delta;
  }
  /* Ensures the size points to as many bytes as necessary so that
     only full-sized blocks fit into the heap.
   */
  size -= (size - 2 * HEADER_SIZE) % PAYLOAD_ALIGN;
  
  h->size = size;
  h->start = heap_start;
  h->search_alg = search_alg;
  
  h->next = h->start;
  set_block_header(h->start, size, 0);
  return h;
}
/*
 * Print the structure of the heap to the screen.
 */
void heap_print(heap *h)
{
  /* TO BE COMPLETED BY THE STUDENT. */
}

/*
 * Determine the average size of all free blocks.
 */
block_size_t heap_find_avg_free_block_size(heap *h)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return 0;
}

/*
 * Free a block on the heap h. Beware of the case where the  heap uses
 * a next fit search strategy, and h->next is pointing to a block that
 * is to be coalesced.
 */
void heap_free(heap *h, void *payload)
{
  /* TO BE COMPLETED BY THE STUDENT. */
}

/*
 * Malloc a block on the heap h, using first fit. Return NULL if no block
 * large enough to satisfy the request exits.
 */
static void *malloc_first_fit(heap *h, block_size_t user_size)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return NULL;
}

/*
 * Malloc a block on the heap h, using best fit. Return NULL if no block
 * large enough to satisfy the request exits.
 */
static void *malloc_best_fit(heap *h, block_size_t user_size)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return NULL;
}

/*
 * Malloc a block on the heap h, using next fit. Return NULL if no block
 * large enough to satisfy the request exits.
 */
static void *malloc_next_fit(heap *h, block_size_t user_size)
{
  /* TO BE COMPLETED BY THE STUDENT. */
  return NULL;
}

/*
 * Our implementation of malloc.
 */
void *heap_malloc(heap *h, block_size_t size)
{
  switch (h->search_alg) {
  case HEAP_FIRSTFIT:
    return malloc_first_fit(h, size);
  case HEAP_NEXTFIT:
    return malloc_next_fit(h, size);
  case HEAP_BESTFIT:
    return malloc_best_fit(h, size);
  }
  return NULL;
}

/*
 * wrapper function for get_size_to_allocate
 */
block_size_t wrapper_get_size_to_allocate(block_size_t user_size){
  return get_size_to_allocate(user_size);
}

/*
 * wrapper function for prepare_block_for_use
 */
void *wrapper_prepare_block_for_use(void *block_start, block_size_t real_size){
  prepare_block_for_use(block_start, real_size);
}

/*
 * wrapper function for get_previous_block
 */
void *wrapper_get_previous_block(void *block_start){
  get_previous_block(block_start);
}

/*
 * wrapper function for coalesce
 */
void *wrapper_coalesce(heap *h, void *first_block_start){
  coalesce(h, first_block_start);
}