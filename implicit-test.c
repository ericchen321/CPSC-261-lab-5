#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include "implicit.h"
#include "tests.h"

/*
 * Function to initialize the random number generator.
 */
void initialize_rng()
{
    struct timeval t1;
    gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
}

/*
 * Function to pick a rand size for a new block.
 */
unsigned long get_rand_block_size()
{
    unsigned long size = 4;
    while (size < 512 && rand() % 6 != 0)
    {
	size <<= 1;
    }
    while (size < 2048 && rand() % 2 != 0)
    {
	size <<= 1;
    }
    return size + rand() % size;
}

/*
 * Function that performs a large number of heap operations. Returns the
 * average size of a free block.
 */
unsigned long test_heap(search_alg_t search_alg, int op_count)
{
  heap *h = heap_create(HEAP_SIZE, search_alg);
  char *pointer_array[MAX_POINTERS];
  unsigned long fragmentation, size;
  int nb_pointers = 0;
  char *new_pointer;
  int index; 
  
  while (op_count-- > 0) {
    
    /* Randomly chooses between malloc and free. Always uses malloc if there
       is nothing to free. More allocated pointers means higher probability
       free will be chosen.
     */
    if ((nb_pointers == 0) || (rand() % MAX_POINTERS > nb_pointers)) {
      
      size = get_rand_block_size();
#if DEBUG
      printf("Malloc'ing %ld bytes for pointer %d.\n", size, nb_pointers);
#endif
      new_pointer = heap_malloc(h, size);
      if (new_pointer == NULL) {
	printf("Ran out of memory with %d operations left.\n", op_count);
	break;
      }
      pointer_array[nb_pointers++] = new_pointer;
    }
    else {
      
      index = rand() % nb_pointers;
#if DEBUG
      printf("Freeing pointer %d at address %p\n", index, pointer_array[index]);
#endif
      heap_free(h, pointer_array[index]);
      pointer_array[index] = pointer_array[--nb_pointers];
    }
#if DEBUG
    heap_print(h);
#endif
  }
  
  fragmentation = heap_find_avg_free_block_size(h);
  return fragmentation;
}

/*
 * Main function.
 */
int main(int argc, char *argv[])
{
  /* call unit_tests from here
   */
  unit_tests();
  
  initialize_rng();
  
  /*
   * Now run tests on all three types of search algorithm.
   */
  printf("First fit average block size: %lu\n", test_heap(HEAP_FIRSTFIT, 50000));
  //printf("Next fit average block size: %lu\n", test_heap(HEAP_NEXTFIT, 50000));
  printf("Best fit average block size: %lu\n", test_heap(HEAP_BESTFIT, 50000));
}
