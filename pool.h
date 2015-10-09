#ifndef __ANN_POOL_H__
#define __ANN_POOL_H__

#include "stdlib.h"
#include "string.h"
#include "assert.h"

typedef struct {
	int size;
	void *data;
} Block;

typedef struct {
	int capacity;
	int size;
	Block *array;
} Pool;

Pool pool_new() {
	Pool self = {32, 0, (Block*)malloc(sizeof(Block) * 32)};
	return self;
}

void pool_close(Pool *self) {
	for (int i = self->size - 1; i >= 0; i--) {
		free(self->array[i].data);
	}
}

void* pool_alloc(Pool *self, int size) {
	assert(size != 0);
	if (self->size >= self->capacity) {
		Block *array = (Block*)malloc(sizeof(Block) * self->capacity * 2);
		memcpy(array, self->array, sizeof(Block) * self->capacity);
		free(self->array);
		self->array = array;
		self->capacity *= 2;
	}

	void *data = malloc(size);
	Block block = {size, data};
	*(self->array + self->size) = block;
	self->size += 1;
	return data;
}

void* pool_resize(Pool *self, void *origin, int size) {
	Block block;

	int idx = -1;
	for (int i = 0; i < self->size; i++) {
		block = self->array[i];
		if (block.data == origin) {
			assert(block.size <= size);
			idx = i;
			break;
		}
	}
	assert(idx >= 0);

	void *data = malloc(size);
	memcpy(data, block.data, block.size);
	free(block.data);
	block.size = size;
	block.data = data;
	*(self->array + idx) = block;
	return data;
}

#endif
