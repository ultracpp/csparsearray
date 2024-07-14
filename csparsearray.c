#include "csparsearray.h"
#include "clib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define C_SPARSE_ARRAY_INIT_CAPACITY	8
#define C_SPARSE_ARRAY_ELEM_LEN	sizeof(c_sparse_node)

static void gc(c_sparse_array* parr)
{
	int n = parr->limit;
	int o = 0;

	for (int i = 0; i < n; i++)
	{
		void* val = parr->array[i].value;

		if (val != C_SPARSE_NODE_DELETED)
		{
			if (i != o)
			{
				parr->array[o].key = parr->array[i].key;
				parr->array[o].value = val;

				parr->array[i].value = C_SPARSE_NODE_DELETED;
			}
			o++;
		}
	}

	parr->have_garbage = 0;
	parr->limit = o;
}

c_sparse_array* c_sparse_array_new(int capacity, int value_free)
{
	int len = sizeof(c_sparse_array);
	c_sparse_array* parr = (c_sparse_array*)malloc(len);
	memset(parr, 0xff, len);

	c_sparse_array_init(parr, capacity, value_free);
	return parr;
}

void c_sparse_array_init(c_sparse_array* parr, int capacity, int value_free)
{
	parr->capacity = capacity;
	parr->value_free = value_free;

	if (parr->capacity < 1)
	{
		parr->capacity = C_SPARSE_ARRAY_INIT_CAPACITY;
	}

	int len = C_SPARSE_ARRAY_ELEM_LEN * parr->capacity;
	parr->array = (c_sparse_node*)malloc(len);

	for (int i = 0; i < parr->capacity; i++)
	{
		parr->array[i].key = -1;
		parr->array[i].value = C_SPARSE_NODE_DELETED;
	}

	parr->limit = 0;
	parr->have_garbage = 0;
}

void c_sparse_array_destroy(c_sparse_array* parr)
{
	c_sparse_array_clear(parr);

	if (parr->array != NULL)
	{
		free(parr->array);
		parr->array = NULL;
	}
}

void c_sparse_array_move(c_sparse_array* dst, c_sparse_array* src)
{
	gc(src);

	dst->array = src->array;
	dst->capacity = src->capacity;
	dst->limit = src->limit;
	dst->value_free = src->value_free;

	c_sparse_array_init(src, C_SPARSE_ARRAY_INIT_CAPACITY, src->value_free);
}

void c_sparse_array_swap(c_sparse_array* dst, c_sparse_array* src)
{
	gc(src);
	gc(dst);

	c_sparse_array tmp = *dst;
	*dst = *src;
	*src = tmp;
}

static int binary_search(c_sparse_node* array, int size, int value)
{
	int lo = 0;
	int hi = size - 1;

	int mid = 2147483647;
	int midVal = 2147483647;

	while (lo <= hi)
	{
		mid = ((unsigned int)lo + (unsigned int)hi) >> 1;
		midVal = array[mid].key;

		if (midVal < value)
		{
			lo = mid + 1;
		} else if (midVal > value)
		{
			hi = mid - 1;
		} else
		{
			return mid;
		}
	}
	return ~lo;
}

void c_sparse_array_insert(c_sparse_array* parr, int key, void* value)
{
	int index = binary_search(parr->array, parr->limit, key);

	if (index < 0)
	{
		index = ~index;

		if (index < parr->limit && parr->array[index].value == C_SPARSE_NODE_DELETED)
		{
			parr->array[index].key = key;
			parr->array[index].value = value;
			return;
		}

		if (parr->have_garbage && parr->limit >= parr->capacity)
		{
			gc(parr);

			index = ~binary_search(parr->array, parr->limit, key);
		}

		if (parr->limit + 1 <= parr->capacity)
		{
			memmove(&parr->array[index + 1], &parr->array[index], (parr->limit - index) * C_SPARSE_ARRAY_ELEM_LEN);

			parr->array[index].key = key;
			parr->array[index].value = value;

			parr->limit++;
			return;
		}

		int n = parr->capacity;
		int newCapacity = n << 1;

		int len = C_SPARSE_ARRAY_ELEM_LEN * newCapacity;
		c_sparse_node* newArray = (c_sparse_node*)malloc(len);
		memset(newArray, 0xff, len);

		memmove(newArray, parr->array, index * C_SPARSE_ARRAY_ELEM_LEN);

		newArray[index].key = key;
		newArray[index].value = value;

		memmove(&newArray[index + 1], &parr->array[index], (parr->limit - index) * C_SPARSE_ARRAY_ELEM_LEN);

		free(parr->array);

		parr->array = newArray;
		parr->capacity = newCapacity;

		parr->limit++;
		return;
	}
	else
	{
		if (parr->value_free)
		{
			free(parr->array[index].value);
		}

		parr->array[index].value = value;
	}
}

void c_sparse_array_remove(c_sparse_array* parr, int key)
{
	int index = binary_search(parr->array, parr->limit, key);

	if (index >= 0)
	{
		if (parr->array[index].value != C_SPARSE_NODE_DELETED)
		{
			if (parr->value_free && parr->array[index].value != C_SPARSE_NODE_DELETED)
			{
				free(parr->array[index].value);
			}

			parr->array[index].value = C_SPARSE_NODE_DELETED;

			parr->have_garbage = 1;
		}
	}
}

c_sparse_node* c_sparse_array_find(c_sparse_array* parr, int key)
{
	int index = binary_search(parr->array, parr->limit, key);

	if (index < 0 || parr->array[index].value == C_SPARSE_NODE_DELETED)
	{
		return NULL;
	}
	else
	{
		return &parr->array[index];
	}
}

int c_sparse_array_size(c_sparse_array* parr)
{
	if (parr->have_garbage)
	{
		gc(parr);
	}

	return parr->limit;
}

void c_sparse_array_clear(c_sparse_array* parr)
{
	int n = parr->limit;

	for (int i = 0; i < n; i++)
	{
		if (parr->value_free && parr->array[i].value != C_SPARSE_NODE_DELETED)
		{
			free(parr->array[i].value);
		}

		parr->array[i].value = C_SPARSE_NODE_DELETED;
	}

	parr->limit = 0;
	parr->have_garbage = 0;
}
