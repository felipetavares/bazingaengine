#ifndef ZOBJECT_H
#define ZOBJECT_H

#include "Box2D/Box2D.h"
#include "ZMath/math/vec3/vec3.h"
#include "ZGraphics.h"

// All objects in the game engine come from
// this definition

class ZObject {
public:
	bool mapped;

	bool phys,background;
	b2Body *box2dBody;
	b2PolygonShape *shape;

	// Physical location
	vec3 *position;
	vec3 *rotation;
	vec3 *size;
	// Visible props
	ZGraphic *graphic;

	// Object id in the engine
	long int oid;

	ZObject (long int,vec3=vec3(),vec3=vec3());
	virtual ~ZObject ();

	virtual void init ();
	virtual void step ();
	virtual void draw ();
};

class ZBoxObject: public ZObject {
public:
	ZBoxObject (long int,vec3=vec3(),vec3=vec3());


	void init ();
	//void step ();
};

class ZFloorObject: public ZObject {
public:
	ZFloorObject (long int,vec3=vec3(),vec3=vec3());

	void init ();
};


class ShotCallback : public b2RayCastCallback {
public:
	bool c = false;

	ShotCallback();

	float32 ReportFixture(b2Fixture*, const b2vec2&,const b2vec2&, float32);

	b2Fixture* m_fixture;
	b2vec2 m_point;
	b2vec2 m_normal;
	float32 m_fraction = 99999;
};

class ZBulletObject: public ZObject {
public:
	bool suicide = false;

	ZBulletObject (long int, vec3=vec3(), vec3=vec3());
	~ZBulletObject();

	void init();
	void step();
};

#endif /* ZOBJECT_H */
