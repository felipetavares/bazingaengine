#ifndef CAMERA_H
#define CAMERA_H

#include "ZMath/math/vec3/vec3.h"

class ZCamera {
    bool drag;
    vec2 dragPos;
public:
	long int playerOid;
	vec3 *position;
	vec3 *scale;

	void run();
	void debugMovement ();

	ZCamera ();
	~ZCamera ();
};

#endif /* CAMERA_H */
