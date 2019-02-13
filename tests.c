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
 * h_2: a 1024B heap with blocks: 48B(u)-512B(f)-424B(f)
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

/* case: given block's size is less than requested size
 */
void test_prepare_block_for_use_case_0(heap **h_0, heap **h_1, heap **h_2){
  void* blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk); // 32B free blk
  block_size_t size = 256;
  if(wrapper_prepare_block_for_use(blk, size) == NULL){}
  else{
    printf("prepare block when requested size is greater than given free block test failed\n");
  }
}

/* case: given block's size is more than twice of requested size
 */
void test_prepare_block_for_use_case_1(heap **h_0, heap **h_1, heap **h_2){
  void* blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk); // 64B free block
  void* next_blk = wrapper_get_next_block(blk);
  block_size_t size = 16;
  blk = wrapper_prepare_block_for_use(blk, size);
  if(blk != NULL
      && wrapper_block_is_in_use(blk)
      && wrapper_get_block_size(blk) == 16
      && wrapper_get_block_size(wrapper_get_next_block(blk)) == 48
      && !wrapper_block_is_in_use(wrapper_get_next_block(blk))
      && wrapper_get_block_size(next_blk) == 32){}
  else{
    printf("prepare block when given block is twice of requested size failed\n");
  }
}

/* case: given block's size >= requested size + MAX_UNUSED_BYTES
 */
void test_prepare_block_for_use_case_2(heap **h_0, heap **h_1, heap **h_2){
  void* blk = (*h_2)->start;
  blk = wrapper_get_next_block(blk); // 512B free block
  void* next_blk = wrapper_get_next_block(blk);
  block_size_t size = 264;
  blk = wrapper_prepare_block_for_use(blk, size);
  if(blk != NULL
      && wrapper_block_is_in_use(blk)
      && wrapper_get_block_size(blk) == 264
      && wrapper_get_block_size(wrapper_get_next_block(blk)) == 248
      && !wrapper_block_is_in_use(wrapper_get_next_block(blk))
      && wrapper_get_block_size(next_blk) == 424){}
  else{
    printf("prepare block when given block's size >= requested size + MAX_UNUSED_BYTES failed\n");
  }
}

/* case: given block's size is enough for requested size, and does not require splitting
 */
void test_prepare_block_for_use_case_3(heap **h_0, heap **h_1, heap **h_2){
  void* blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk); // 64B free block
  block_size_t size = 48;
  blk = wrapper_prepare_block_for_use(blk, size);
  if(blk != NULL
      && wrapper_block_is_in_use(blk)
      && wrapper_get_block_size(blk) == 64
      && wrapper_get_block_size(wrapper_get_next_block(blk)) == 32){}
  else{
    printf("prepare block when block size and requrested size implies no splitting failed\n");
  }
}

/* case: try get prev block of a block in the middle, expects prev block
 *       the prev block is free (though should not be point of behavior change)
 */
void test_get_previous_block_case_0(heap **h_0, heap **h_1, heap **h_2){
  void* blk;
  blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_previous_block(blk);
  if(blk != NULL && wrapper_get_block_size(blk) == 64){}
  else{
    printf("get prev block of 3rd block from h_1 failed\n");
  }
}

/* case: try get prev block of a block in the middle, expects prev block
 *       the prev block is occupied (though should not be point of behavior change)
 */
void test_get_previous_block_case_1(heap **h_0, heap **h_1, heap **h_2){
  void* blk;
  blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_previous_block(blk);
  if(blk != NULL && wrapper_get_block_size(blk) == 16){}
  else{
    printf("get prev block of 1st block from h_1 failed\n");
  }
}

/* case: cannot coalesce because given block is not free
 */
void test_coalesce_case_0(heap **h_0, heap **h_1, heap **h_2){
  void* blk;
  blk = (*h_1)->start;
  blk = wrapper_coalesce(*h_1, blk);
  if(blk != NULL
      && wrapper_get_block_size(blk) == 16
      && wrapper_get_block_size(wrapper_get_next_block(blk)) == 32){}
  else{
    printf("call coalesce when given block is occupied test failed\n");
  }
}

/* case: cannot coalesce because following block exists but is not free
 */
void test_coalesce_case_1(heap **h_0, heap **h_1, heap **h_2){
  void* blk;
  blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk); // blk is 3rd one in h_1

  blk = wrapper_coalesce(*h_1, blk);
  if(blk != NULL
      && wrapper_get_block_size(blk) == 32
      && wrapper_get_block_size(wrapper_get_next_block(blk)) == 16){}
  else{
    printf("call coalesce when following block is occupied test failed\n");
  }
}

/* case: cannot coalesce because following block does not exist
 */
void test_coalesce_case_2(heap **h_0, heap **h_1, heap **h_2){
  void* blk;
  blk = (*h_2)->start;
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk);

  blk = wrapper_coalesce(*h_2, blk);
  if(blk != NULL
      && wrapper_get_block_size(blk) == 424){}
  else{
    printf("call coalesce when following block does not exist test failed\n");
  }
}

/* case: both blocks are free, can coalesce
 */
void test_coalesce_case_3(heap **h_0, heap **h_1, heap **h_2){
  void* blk;
  blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk); // blk is 1st one in h_1

  blk = wrapper_coalesce(*h_1, blk);
  if(blk != NULL
      && wrapper_get_block_size(blk) == 96
      && wrapper_get_block_size(wrapper_get_next_block(blk)) == 32){}
  else{
    printf("call coalesce when both blocks are free failed\n");
  }
}

/* case: first fit, from h_1 requests 55B, expects returning 
 *       ptr to payload of 2nd block (64B, f)
 *       set as used, no splitting
 */
void test_malloc_first_fit_case_0(heap** h_0, heap** h_1, heap** h_2){
  void* payload = NULL;
  void* blk = NULL;
  payload = heap_malloc(*h_1, 55);
  if(payload != NULL)
    blk = wrapper_get_block_start(payload);
  if(payload != NULL
      && blk != NULL
      && wrapper_get_block_size(blk) == 64
      && wrapper_block_is_in_use(blk)){}
  else{
    printf("first fit, from h_0 requests 62B failed\n");
  }
}

/* case: first fit, from h_2 requests 1024B, expects returning 
 *       NULL ptr
 */
void test_malloc_first_fit_case_1(heap** h_0, heap** h_1, heap** h_2){
  void* payload = heap_malloc(*h_2, 1024);
  if(payload == NULL){}
  else{
    printf("first fit, from h_2 requests 1024B test failed\n");
  }
}

/* case: first fit, from h_2 requests 120B, expects returning
 *       ptr to payload of 1st block (512B, f), which should be
 *       split to 128B(u) and 384B(f)
 */
void test_malloc_first_fit_case_2(heap** h_0, heap** h_1, heap** h_2){
  void* payload = heap_malloc(*h_2, 120);
  void* blk = NULL;
  if(payload != NULL)
    blk = wrapper_get_block_start(payload);
  if(payload != NULL
      && blk != NULL
      && wrapper_block_is_in_use(blk)
      && wrapper_get_block_size(blk) == 128
      && wrapper_get_block_size(wrapper_get_next_block(blk))==384){}
  else{
    printf("first fit, from h_2 requests 128B failed\n");
  }
}

/* case: first fit, from h_0 requests 0B, expects returning
 *       NULL ptr
 */
void test_malloc_first_fit_case_3(heap** h_0, heap** h_1, heap** h_2){
  void* payload = heap_malloc(*h_0, 0);
  if(payload == NULL){}
  else{
    printf("first fit, from h_0 requests 0B test failed\n");
  }
}

/* case: best fit, from h_1 requests 0B, expects returning NULL
 *       ptr
 */
void test_malloc_best_fit_case_0(heap** h_0, heap** h_1, heap** h_2){
  void* payload = heap_malloc(*h_1, 0);
  if(payload == NULL){}
  else{
    printf("best fit, from h_1 requests 0B test failed\n");
  }
}

/* case: best fit, change 4th block to free first. Then
 *       from h_1 requests 5B, expects returning ptr to
 *       payload of 4th block (16B, u), no splitting
 */
void test_malloc_best_fit_case_1(heap** h_0, heap** h_1, heap** h_2){
  void* blk = (*h_1)->start;
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk);
  blk = wrapper_get_next_block(blk);
  wrapper_set_block_header(blk, wrapper_get_block_size(blk), 0); // set 4th block to free

  void* payload =heap_malloc(*h_1, 5);
  if(payload != NULL
      && wrapper_get_block_start(payload) == blk
      && wrapper_block_is_in_use(blk)){}
  else{
    printf("best fit, from h_1 requests 5B failed\n");
  }
}

/* case: best fit, from h_2 requests 120B, expects 2nd block split
 *       to 128B(u) and 296B(f), returns ptr to payload to 
 *       2nd block
 */
void test_malloc_best_fit_case_2(heap** h_0, heap** h_1, heap** h_2){
  void* payload = heap_malloc(*h_2, 120);
  void* blk = NULL;
  if(payload != NULL)
    blk = wrapper_get_block_start(payload);
  if(payload != NULL
      && blk != NULL
      && wrapper_block_is_in_use(blk)
      && wrapper_get_block_size(blk) == 128
      && !wrapper_block_is_in_use(wrapper_get_next_block(blk))
      && wrapper_get_block_size(wrapper_get_next_block(blk)) == 296){}
  else{
    printf("best fit, from h_2 requests 120B failed\n");
    if(payload == NULL)
      printf("got null return\n");
  }
}

/* case: best fit, from h_2 requests 2048B, expects NULL ptr
 */
void test_malloc_best_fit_case_3(heap** h_0, heap** h_1, heap** h_2){
  void* payload = heap_malloc(*h_2, 2048);
  if(payload == NULL){}
  else{
    printf("best fit, from h_2 requests 2048B test failed\n");
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

  // tests: prepare_block_for_use
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_prepare_block_for_use_case_0(&h_0, &h_1, &h_2); // less
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_prepare_block_for_use_case_1(&h_0, &h_1, &h_2); // more than twice
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_prepare_block_for_use_case_2(&h_0, &h_1, &h_2); // at least MAX_UNUSED_BYTES longer
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_prepare_block_for_use_case_3(&h_0, &h_1, &h_2); // allocate without splitting

  // tests: get_previous_block
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_get_previous_block_case_0(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_get_previous_block_case_1(&h_0, &h_1, &h_2);

  // tests: coalesce
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_coalesce_case_0(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_coalesce_case_1(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_coalesce_case_2(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_coalesce_case_3(&h_0, &h_1, &h_2);

  // tests: malloc_first_fit
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_malloc_first_fit_case_0(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_malloc_first_fit_case_1(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_malloc_first_fit_case_2(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_FIRSTFIT);
  test_malloc_first_fit_case_3(&h_0, &h_1, &h_2);

  // tests: malloc_best_fit
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_BESTFIT);
  test_malloc_best_fit_case_0(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_BESTFIT);
  test_malloc_best_fit_case_1(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_BESTFIT);
  test_malloc_best_fit_case_2(&h_0, &h_1, &h_2);
  initialize_heaps(&h_0, &h_1, &h_2, HEAP_BESTFIT);
  test_malloc_best_fit_case_3(&h_0, &h_1, &h_2);
}