typedef float Value;

#include "stdio.h"
#include "string.h"
#include "bp.h"
#include "pool.h"
#include "values.h"

void cmd_bp(int times) {
	Pool pool = pool_new();

	Values input[] = {
		values_new(&pool, 2, 1, 1),
		values_new(&pool, 2, 0, 0),
		values_new(&pool, 2, 1, 0),
		values_new(&pool, 2, 0, 1),
	};
	Values expected[] = {
		values_new(&pool, 1, 1),
		values_new(&pool, 1, 0),
		values_new(&pool, 1, 1),
		values_new(&pool, 1, 0),
	};

	Matrix m = matrix_new_by_rows(&pool, 4, 2, 3, 2, 1);
	Bp bp = bp_new(&pool, &m, 1);
	for (int i = 0; i < times; i++) {
		for (int j = 0; j < 4; j++) {
			bp_train(&bp, input[j], expected[j]);
		}
	}
	for (int j = 0; j < 4; j++) {
		Values output = bp_output(&bp, input[j]);
		printf("%f, %f  =>  %.6f (%f)\n", input[j].values[0], input[j].values[1], output.values[0], expected[j].values[0]);
	}
	pool_close(&pool);
}

int main(int argc, char *argv[]) {
	cmd_bp(20000);
	return 0;
}
