#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include "tests.h"
#include "implicit.h"

/*
 * initialize 3 heaps with given searching algorithm:
 * h_0: a 64B heap with a single 24B free block
 * h_1: a 256B heap with blocks: 16B(u)-32B(f)-64B(f)-32B(f)-16B(u)-32B(u)-24B(f)
 * h_2: a 1024B heap with blocks: 48B(u)-512B(f)-428B(f)
 */
void initialize_heaps(heap** h_0, heap** h_1, heap** h_2, search_alg_t search_alg){
  void* block_start;
  *h_0 = heap_create(64, search_alg);
  *h_1 = heap_create(256, search_alg);
  *h_2 = heap_create(1024, search_alg);

  if(*h_0 == NULL || *h_1 == NULL || *h_2 == NULL){
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

  block_start = (*h_2)->start;
  wrapper_set_block_header(block_start, 48, 1);
  block_start += 48;
  wrapper_set_block_header(block_start, 512, 0);
  block_start += 512;
  wrapper_set_block_header(block_start, 424, 0);
  if(!wrapper_is_within_heap_range(*h_2, block_start + 423)){
    printf("ERROR: dividing blocks for h_2 failed\n");
    return;
  }
}

/*
 * running all unit tests
 */
void unit_tests(){
  heap* h_0; heap* h_1; heap* h_2;

  // tests: heap_print
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  heap_print(h_0);
  heap_print(h_1);

  // tests: heap_find_avg_free_block_size
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  if(heap_find_avg_free_block_size(h_0)==24){}
  else{
    printf("find average free block size of h_0 failed\n");
  }
  if(heap_find_avg_free_block_size(h_1)==38){}
  else{
    printf("find average free block size of h_1 failed\n");
  }

  // tests: get_size_to_allocate
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  if(wrapper_get_size_to_allocate(1)==16){}
  else{
    printf("get min block size when user size is 1 failed\n");
  }
  if(wrapper_get_size_to_allocate(7)==16){}
  else{
    printf("get min block size when user size is 7 failed\n");
  }
  if(wrapper_get_size_to_allocate(8)==16){}
  else{
    printf("get min block size when user size is 8 failed\n");
  }
  if(wrapper_get_size_to_allocate(19)==32){}
  else{
    printf("get min block size when user size is 19 failed\n");
  }
  if(wrapper_get_size_to_allocate(24)==32){}
  else{
    printf("get min block size when user size is 24 failed\n");
  }


}