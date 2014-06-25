#ifndef ZOBJECT_H
#define ZOBJECT_H

#include "Box2D/Box2D.h"
#include "../ZMath/Vec3.h"
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
	Vec3 *position;
	Vec3 *rotation;
	Vec3 *size;
	// Visible props
	ZGraphic *graphic;

	// Object id in the engine
	long int oid;

	ZObject (long int,Vec3=Vec3(),Vec3=Vec3());
	virtual ~ZObject ();

	virtual void init ();
	virtual void step ();
	virtual void draw ();
};

class ZBoxObject: public ZObject {
public:
	ZBoxObject (long int,Vec3=Vec3(),Vec3=Vec3());


	void init ();
	//void step ();
};

class ZFloorObject: public ZObject {
public:
	ZFloorObject (long int,Vec3=Vec3(),Vec3=Vec3());

	void init ();
};

class ZPlayerObject: public ZObject {
public:
    ZAnimation* anims[4];

	bool reinit = false;

	int playerNum = 0;

	Vec3 dir;
	ZPlayerObject (long int, Vec3=Vec3(), Vec3=Vec3());

	void init();
	void step();
	void shot();
	void draw();
private:
    float getAxis (int);
};

class ShotCallback : public b2RayCastCallback {
public:
	bool c = false;

	ShotCallback();

	float32 ReportFixture(b2Fixture*, const b2Vec2&,const b2Vec2&, float32);

	b2Fixture* m_fixture;
	b2Vec2 m_point;
	b2Vec2 m_normal;
	float32 m_fraction = 99999;
};

class ZBulletObject: public ZObject {
public:
	bool suicide = false;

	ZBulletObject (long int, Vec3=Vec3(), Vec3=Vec3());
	~ZBulletObject();

	void init();
	void step();
};

#endif /* ZOBJECT_H */
