#ifndef __ANN_ROWS_H__
#define __ANN_ROWS_H__

#include "stdlib.h"
#include "assert.h"
#include "stdarg.h"
#include "pool.h"

//  Class Matrix describe struct like this:
//
//  (1) Overview:
//  Row0:  Node0[]                          Node1[]
//  Row1:  Node3[Node0-Node3, Node1-Node3]  Node4[Node0-Node4, Node1-Node4]
//  Row2:  Node5[Node3-Node5, Node4-Node5]
//  Row3:  Node6[Node5-Node6]
//
//  (2) Node Array:
//  Node0, Node1, Node2, Node3, Node4, Node5, Node6
//
//  (3) Connection Array:
//  Node0-Node3, Node1-Node3, Node0-Node4, Node1-Node4, Node3-Node5, Node4-Node5, Node5-Node6

typedef struct {
	int size;        // node count of this row
	int pos;         // index of the first node in this row
	int sum;         // total node count include this row and previous matrix

	struct {
		int size;    // connection count of nodes in this row
		int pos;     // index of the first connection in this row
		int sum;     // total connection count include this row and previous matrix
	} conns;         // infos about connections of this row
} Row;

typedef struct {
	Pool *pool;      // memory pool
	int capacity;    // rows' size allocated
	int size;        // rows' size
	Row *rows;       // rows
	Row *first;      // first row
	Row *last;       // last row

	struct {
		int sum;     // total count of connections
	} conns;         // summary infos of node connections

	struct {
		int sum;     // total count of nodes
	} nodes;         // summary infos of node array
} Matrix;

Matrix matrix_new(Pool *pool) {
	Row *data = (Row*)pool_alloc(pool, sizeof(Row) * 32);
	Matrix matrix = {pool, 32, 0, data, data, data, {0}, {0}};
	return matrix;
}

void matrix_append(Matrix *self, int size) {
	assert(size != 0);
	if (self->size >= self->capacity) {
		self->rows = pool_resize(self->pool, self->rows, sizeof(Row) * self->capacity * 2);
		self->first = self->rows;
	}

	int prev_size = 0;
	int pos = 0;
	int conns_pos = 0;
	int sum_conns = 0;

	if (self->size != 0) {
		Row *prev = self->rows + self->size - 1;
		prev_size = prev->size;
		pos = prev->pos + prev_size;
		conns_pos = prev->conns.sum;
		sum_conns = prev->conns.sum + prev->size * size;
	}

	Row row = {size, pos, size + pos, {prev_size, conns_pos, sum_conns}};
	*(self->rows + self->size) = row;
	self->size += 1;
	self->conns.sum += sum_conns;
	self->nodes.sum = size + pos;
	self->last = self->first + self->size - 1;
}

Matrix matrix_new_by_rows(Pool *pool, int size, ...) {
	Matrix matrix = matrix_new(pool);
	va_list args;
	va_start(args, size);
	for (int i = 0; i < size; i++) {
		int arg = va_arg(args, int);
		if (arg == 0)
			break;
		matrix_append(&matrix, arg);
	}
	va_end(args);
	return matrix;
}

//  Helper functions:
//    conn: return index of a connection in connection array.
//    in:   return index of a connection's in-node (the one in the higher row) in node array.
//    node: return index of a node in node array.
//
//  Helper functions' args:
//    row:  index of row
//    col:  index of col in the row
//    conn: index of connection of node
//
int in(Matrix *self, int row, int col, int conn) {
	return self->rows[row - 1].pos + conn;
}
int node(Matrix *self, int row, int col) {
	return self->rows[row].pos + col;
}
int conn(Matrix *self, int row, int col, int conn) {
	return self->rows[row].conns.pos + self->rows[row].conns.size * col + conn;
}

#endif
