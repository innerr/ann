#ifndef __ANN_VALUES_H__
#define __ANN_VALUES_H__

#include "pool.h"
#include "stdarg.h"

typedef struct {
	int size;
	Value *values;
} Values;

Values values_new(Pool *pool, int size, ...) {
	Value *data = pool_alloc(pool, sizeof(Value) * size);
	int idx = 0;
	va_list args;
	va_start(args, size);
	for (int i = 0; i < size; i++) {
		int value = va_arg(args, int);
		data[i] = (Value)value;
	}
	va_end(args);
	Values values = {size, data};
	return values;
}

#endif

