# C Sparse Array Implementation

This repository contains a C library for a sparse array data structure (`c_sparse_array`) using a sorted array of nodes (`c_sparse_node`). Here's an overview of the components and functionalities provided.

## Introduction

Sparse arrays are useful when you have a large range of indices but only a small subset of them are actively used. This implementation efficiently manages memory and provides operations to insert, remove, find elements, and more.

## Structure

### `c_sparse_array`

- **Properties**:
  - `array`: Array of `c_sparse_node` elements.
  - `capacity`: Current capacity of the array.
  - `limit`: Number of elements currently in the array.
  - `value_free`: Flag indicating whether to free values upon removal.

### `c_sparse_node`

- **Properties**:
  - `key`: Integer key for indexing.
  - `value`: Pointer to the stored value or a special marker for deleted nodes.

## Functions

### Initialization and Memory Management

- `c_sparse_array_new`: Creates a new sparse array.
- `c_sparse_array_init`: Initializes an existing sparse array with a specified capacity.
- `c_sparse_array_destroy`: Frees memory allocated for the sparse array.
- `c_sparse_array_move`: Moves contents from one sparse array to another.
- `c_sparse_array_swap`: Swaps contents between two sparse arrays.

### Insertion, Removal, and Access

- `c_sparse_array_insert`: Inserts a key-value pair into the sparse array.
- `c_sparse_array_remove`: Removes an element with the given key from the sparse array.
- `c_sparse_array_find`: Finds and returns a node with the given key in the sparse array.

### Utility and Maintenance

- `c_sparse_array_size`: Returns the current number of elements in the sparse array.
- `c_sparse_array_clear`: Removes all elements from the sparse array.

### Garbage Collection

- `gc`: Internal function to clean up deleted nodes and compact the array.

### Helper Functions

- `binary_search`: Performs a binary search on the sorted array of nodes.

## Usage

To use this library, include `csparsearray.h` and link against the compiled library. Here's a brief example of how to create and manipulate a sparse array:

```c
#include "csparsearray.h"

int main() {
  int capacity = 100;
	int rand_max = capacity * 2;

	srand((unsigned)time(NULL));

  c_sparse_array carr;
  c_sparse_array_init(&carr, -1, 0);

  for (int i = 0; i < capacity; i++)
  {
    int key = rand() % rand_max;
    c_sparse_array_insert(&carr, key, INT_TO_POINTER(key * 100));
    printf("%d\n", key);
  }

  for (int i = 0, j = carr.limit; i < j; i++)
  {
    if (carr.array[i].value != C_SPARSE_NODE_DELETED)
    {
      printf("%d, %d=%d\n", i, carr.array[i].key, carr.array[i].value);
    }
  }
  printf("====iterate\n");

  for (int i = 0, j = capacity * 2; i < j; i++)
  {
    int key = rand() % rand_max;
    c_sparse_node* result = c_sparse_array_find(&carr, key);

    if (result != NULL)
    {
      printf("contains, %d=%d\n", result->key, result->value);
    }
    else
    {
      printf("====not found, %d\n", key);
    }

    c_sparse_array_remove(&carr, key);
  }

  for (int i = 0, j = carr.limit; i < j; i++)
  {
    if (carr.array[i].value != C_SPARSE_NODE_DELETED)
    {
      printf("%d, %d=%d\n", i, carr.array[i].key, carr.array[i].value);
    }
  }

  printf("====remove\n");
  //printf("====remove:%d\n", c_sparse_array_size(&carr));

  c_sparse_array carr2 = {};
  c_sparse_array_move(&carr2, &carr);
  printf("====move\n");

  c_sparse_array_insert(&carr, 777, INT_TO_POINTER(777));

  for (int i = 0, j = carr.limit; i < j; i++)
  {
    if (carr.array[i].value != C_SPARSE_NODE_DELETED)
    {
      printf("%d, %d=%d\n", i, carr.array[i].key, carr.array[i].value);
    }
  }
  printf("====carr, size:%d\n", c_sparse_array_size(&carr));

  for (int i = 0, j = carr2.limit; i < j; i++)
  {
    if (carr2.array[i].value != C_SPARSE_NODE_DELETED)
    {
      printf("%d, %d=%d\n", i, carr2.array[i].key, carr2.array[i].value);
    }
    else
    {
      printf("deleted, %d\n", i);
    }
  }
  printf("====carr2, size:%d\n", c_sparse_array_size(&carr2));

  c_sparse_array_destroy(&carr);
  c_sparse_array_destroy(&carr2);

  return 0;
}
