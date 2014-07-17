#ifndef LI_H
#define LI_H

#include "ZMath/math/vec2/vec2.h"

// Linear Interpolation
class li {
public:
	vec2 t;
	vec2 y;

	li ();
	li(vec2, vec2);
	float v (float=-1);
	bool complete (float=-1);
};

#endif /* LI_H */
