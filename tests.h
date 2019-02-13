#ifndef _TESTS_H_
#define _TESTS_H_

#include "implicit.h"

/*
 * Set to 1 for additional debugging statements.
 */
#define DEBUG 0

/*
 * Default size for the heap is 2MB( 2^21 B).
 */
#define HEAP_SIZE (1 << 21)

/*
 * Default size for array holding pointers returned by malloc.
 */
#define MAX_POINTERS 1000

/*
 * running all unit tests
 */
void unit_tests();

#endif