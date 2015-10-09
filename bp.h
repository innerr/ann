#ifndef __ANN_BP_H__
#define __ANN_BP_H__

#include "matrix.h"
#include "activator.h"
#include "values.h"

typedef struct {
	Matrix *matrix;
	Value *nodes;
	Value *values;
	Value *deltas;
	Value learning_rate;
} Bp;

Bp bp_new(Pool *pool, Matrix *matrix, Value learning_rate) {
	assert(2 <= matrix->size);
	Bp self = {
		matrix,
		(Value*)malloc(sizeof(Value) * matrix->conns.sum),
		(Value*)pool_alloc(pool, sizeof(Value) * matrix->nodes.sum),
		(Value*)pool_alloc(pool, sizeof(Value) * matrix->nodes.sum),
		learning_rate,
	};
	for (int i = 0; i < matrix->conns.sum; i++) {
		self.nodes[i] = 1;
	}
	return self;
}

Values bp_output(Bp *self, Values input) {
	Matrix *m = self->matrix;

	assert(input.size <= m->rows[0].size);
	for (int i = 0; i < m->nodes.sum; i++) {
		*(self->values + i) = i < input.size ? input.values[i] : 0;
	}

	for (int i = 1; i < m->size; i++) {
		for (int j = 0; j < m->rows[i].size; j++) {
			for (int k = 0; k < m->rows[i].conns.size; k++) {
				self->values[node(m, i, j)] += self->values[in(m, i, j, k)] * self->nodes[conn(m, i, j, k)];
			}
			self->values[node(m, i, j)] = sigmoid(self->values[node(m, i, j)]);
		}
	}

	Values output = {m->rows[m->size - 1].size, self->values + m->last->pos};
	return output;
}

void bp_print_nodes(Bp *self) {
	Matrix *m = self->matrix;
	for (int i = 0; i < m->size; i++) {
		for (int j = 0; j < m->rows[i].size; j++) {
			printf("{");
			for (int k = 0; k < m->rows[i].conns.size; k++) {
				printf(" %f ", self->nodes[conn(m, i, j, k)]);
			}
			printf("} ");
		}
		printf("\n");
	}
}

void bp_print_cal(Bp *self) {
	for (int i = 0; i < self->matrix->nodes.sum; i++) {
		printf("delta %d: %10f\n", i, self->deltas[i]);
	}
	for (int i = 0; i < self->matrix->nodes.sum; i++) {
		printf("value %d: %10f\n", i, self->values[i]);
	}
}

void bp_train(Bp *self, Values input, Values expected) {
	Matrix *m = self->matrix;

	Values output = bp_output(self, input);
	assert(expected.size <= output.size);

	// init
	for (int i = 0; i < m->last->pos; i++) {
		self->deltas[i] = 0;
	}
	for (int i = 0; i < expected.size; i++) {
		self->deltas[m->last->pos + i] = expected.values[i] - output.values[i];
	}

	// calculate deltas
	for (int i = m->size - 1; i >= 1; i--) {
		for (int j = m->rows[i].size - 1; j >= 0; j--) {
			for (int k = m->rows[i].conns.size - 1; k >= 0; k--) {
				self->deltas[in(m, i, j, k)] += self->deltas[node(m, i, j)] * self->nodes[conn(m, i, j, k)];
			}
		}
	}

	// ajust weights
	Value sigma = 0;
	for (int i = 1; i < m->size; i++) {
		for (int j = 0; j < m->rows[i].size; j++) {
			for (int k = 0; k < m->rows[i].conns.size; k++) {
				sigma = 0;
				for (int l = 0; l < m->rows[i].conns.size; l++) {
					sigma += self->values[in(m, i, j, l)] * self->nodes[conn(m, i, j, l)];
				}
				sigma = sigmoid_derivative(sigma);
				self->nodes[conn(m, i, j, k)] += self->learning_rate *
					self->deltas[node(m, i, j)] * self->values[in(m, i, j, k)] * sigma;
			}
		}
	}
}

#endif
