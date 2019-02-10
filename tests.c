#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include "tests.h"
#include "implicit.h"

/*
 * initialize 2 heaps with given searching algorithm:
 * h_0: a 64B heap with a single 24B free block
 * h_1: a 256B heap with blocks: 16B(u)-32B(f)-64B(f)-32B(f)-16B(u)-32B(u)-24B(f)
 */
void initialize_heaps(heap** h_0, heap** h_1, search_alg_t search_alg){
  void* block_start;
  *h_0 = heap_create(64, search_alg);
  *h_1 = heap_create(256, search_alg);

  if(h_0 == NULL || h_1 == NULL){
    printf("ERROR: allocating space for testing heaps failed\n");
    return;
  }

  block_start = (*h_0)->start;
  wrapper_set_block_header(block_start, 24, 0);
  if(!wrapper_is_within_heap_range(*h_0, block_start + 23)){
    printf("ERROR: dividing blocks for h_0 failed");
    return;
  }

  block_start = (*h_1)->start;
  wrapper_set_block_header(block_start, 16, 1); 
  block_start += 16;
  wrapper_set_block_header(block_start, 32, 0);
  block_start += 32;
  wrapper_set_block_header(block_start, 64, 0);
  block_start += 64;
  wrapper_set_block_header(block_start, 32, 0);
  block_start += 32;
  wrapper_set_block_header(block_start, 16, 1);
  block_start += 16;
  wrapper_set_block_header(block_start, 32, 1);
  block_start += 32;
  wrapper_set_block_header(block_start, 24, 0);

  if(!wrapper_is_within_heap_range(*h_1, block_start + 23)){
    printf("ERROR: dividing blocks for h_1 failed\n");
    return;
  }
}

/*
 * running all unit tests
 */
void unit_tests(){
  heap* h_0; heap* h_1;

  // tests: heap_print
  initialize_heaps(&h_0, &h_1, HEAP_FIRSTFIT);
  heap_print(h_0);
  heap_print(h_1);

  // tests: heap_find_avg_free_block_size
  initialize_heaps(&h_0, &h_1, HEAP_FIRSTFIT);
  if(heap_find_avg_free_block_size(h_0)==24){}
  else{
    printf("find average free block size of h_0 failed\n");
  }
  if(heap_find_avg_free_block_size(h_1)==38){}
  else{
    printf("find average free block size of h_1 failed\n");
  }
}