/*
 * csparsearray - Sparse Array Implementation in C Language
 * Copyright (c) 2024 Eungsuk Jeon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __C_SPARSE_ARRAY_H
#define __C_SPARSE_ARRAY_H

#define C_SPARSE_NODE_DELETED	((void*)(long long)(18446744073709551615))

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _tag_c_sparse_node
	{
		int key;
		void* value;
	} c_sparse_node;

	typedef struct _tag_c_sparse_array
	{
		c_sparse_node* array;
		int capacity;
		int limit;
		int value_free;
		int have_garbage;
	} c_sparse_array;

	c_sparse_array* c_sparse_array_new(int capacity, int value_free);
	void c_sparse_array_init(c_sparse_array* parr, int capacity, int value_free);
	void c_sparse_array_destroy(c_sparse_array* parr);
	void c_sparse_array_move(c_sparse_array* dst, c_sparse_array* src);
	void c_sparse_array_swap(c_sparse_array* dst, c_sparse_array* src);

	void c_sparse_array_insert(c_sparse_array* parr, int key, void* value);
	void c_sparse_array_remove(c_sparse_array* parr, int key);
	c_sparse_node* c_sparse_array_find(c_sparse_array* parr, int key);
	int c_sparse_array_size(c_sparse_array* parr);
	void c_sparse_array_clear(c_sparse_array* parr);

#ifdef __cplusplus
}
#endif
#endif
