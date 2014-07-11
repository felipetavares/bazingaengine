#ifndef LI_H
#define LI_H

#include "ZMath/math/vec2/vec2.h"
#include "ZEngine/ZEngine.h"

// Linear Interpolation
class li {
public:
	vec2 t;
	vec2 y;

	li(vec2, vec2);
	float v (float=Engine->gameTime->currentTime);
	bool complete (float=Engine->gameTime->currentTime);
};

#endif /* LI_H */