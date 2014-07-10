#include "li.h"

li::li (vec2 _t, vec2 _y):
	t(_t), y(_y) {
}

float li::v (float _t) {
	if (_t >= t[1])
		return y[1];
	else if (_t <= t[0])
		return y[0];
	else
		// The real interpolation
		return y[0]+(y[1]-y[0])*(_t-t[0])/(t[1]-t[0]);
}