#ifndef CAMERA_H
#define CAMERA_H

#include "../ZMath/Vec3.h"

class ZCamera {
    bool drag;
    Vec2 dragPos;
public:
	long int playerOid;
	Vec3 *position;
	Vec3 *scale;

	void run();
	void debugMovement ();

	ZCamera ();
	~ZCamera ();
};

#endif /* CAMERA_H */
