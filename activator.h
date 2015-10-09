#ifndef __ANN_ACTIVATOR_H__
#define __ANN_ACTIVATOR_H__

#include "math.h"

Value sigmoid(Value v) {
	return 1 / (1 + exp(-v));
}

Value sigmoid_derivative(Value v) {
	return sigmoid(v) * (1 - sigmoid(v));
}

#endif
