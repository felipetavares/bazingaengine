#include "ZObject.h"
#include "../ZEngine/ZEngine.h"
#include <cmath>

// You can only create objects if you know it's
// oid (object id)

ZObject::ZObject (long int _oid,
		  vec3 _position,
		  vec3 _rotation) {
	oid = _oid;

	position = new vec3 (_position);
	rotation = new vec3 (_rotation);
	size  	 = new vec3 ();

	graphic = new ZGraphic();

	box2dBody = NULL;
	shape 	  = NULL;
	phys 	  = false;
	background = false;
}

ZObject::~ZObject () {
	delete position;
	delete rotation;
	delete size;
	delete graphic;
}

void ZObject::step () {
	if (box2dBody) {
		position->x = box2dBody->GetPosition().x;
		position->y = box2dBody->GetPosition().y;

		graphic->position->z = (position->y+size->y/2)/100000;

		rotation->z = box2dBody->GetAngle()*57.2957795;
	}
}

void ZObject::draw () {

}

void ZObject::init () {
}

ZBoxObject::ZBoxObject (long int _oid,
						vec3 _position,
						vec3 _rotation):
	ZObject (_oid, _position, _rotation)
{
	vector <ZTextureAsset*> icons = {
		Engine->assetsManager->getAsset <ZTextureAsset*> ("image.matches")
	};

	selector.setIcons(icons);
}

void ZBoxObject::init () {
	if (box2dBody != NULL) {
		Engine->box2dWorld->DestroyBody (box2dBody);
		box2dBody = NULL;
	}

	b2BodyDef *def = new b2BodyDef();
	def->type = b2_staticBody;
	def->position.Set (position->x,position->y);
	def->angle = rotation->z*M_PI/180;
	def->userData = this;
	def->allowSleep = true;
	def->fixedRotation = true;

	if (shape != NULL)
		delete shape;

	shape = new b2PolygonShape();

	shape->SetAsBox (size->x/2,size->y/2);

	box2dBody = Engine->box2dWorld->CreateBody (def);

	b2FixtureDef *fixtureDef = new b2FixtureDef();
	fixtureDef->shape = shape;
	fixtureDef->density = 0.0f;
	fixtureDef->friction = 0.3f;
	fixtureDef->filter.groupIndex = (int)phys;

	box2dBody->CreateFixture (fixtureDef);

	delete fixtureDef;
	delete def;

	mapped = true;
}

void ZBoxObject::draw () {
	selector.draw();
}

ZBulletObject::ZBulletObject (long int _oid,
						vec3 _position,
						vec3 _rotation):
	ZObject (_oid, _position, _rotation)
{

}

void ZBulletObject::init () {
	if (box2dBody != NULL) {
		Engine->box2dWorld->DestroyBody (box2dBody);
		box2dBody = NULL;
	}

	b2BodyDef *def = new b2BodyDef();
	def->type = b2_dynamicBody;
	def->position.Set (position->x,position->y);
	def->angle = rotation->z*M_PI/180;
	def->userData = this;
	def->allowSleep = true;
	def->fixedRotation = true;

	if (shape != NULL)
		delete shape;

	shape = new b2PolygonShape();

	shape->SetAsBox (size->x/2,size->y/2);

	box2dBody = Engine->box2dWorld->CreateBody (def);

	b2FixtureDef *fixtureDef = new b2FixtureDef();
	fixtureDef->shape = shape;
	fixtureDef->density = 0.1f;
	fixtureDef->friction = 1.0f;
	fixtureDef->filter.groupIndex = 2;

	box2dBody->CreateFixture (fixtureDef);

	delete fixtureDef;
	delete def;

	mapped = false;
}

ZBulletObject::~ZBulletObject () {
	if (box2dBody != NULL) {
		Engine->box2dWorld->DestroyBody (box2dBody);
		box2dBody = NULL;
	}

	if (shape != NULL)
		delete shape;

	Engine->removeObject (oid);
}

void ZBulletObject::step () {
	if (suicide)
		delete this;
}

ZFloorObject::ZFloorObject (long int _oid,
						vec3 _position,
						vec3 _rotation):
	ZObject (_oid, _position, _rotation)
{

}

void ZFloorObject::init () {
	b2BodyDef *def = new b2BodyDef();
	def->position.Set (0,500);
	def->userData = this;
	def->allowSleep = true;

	shape = new b2PolygonShape();

	shape->SetAsBox (500,5);

	box2dBody = Engine->box2dWorld->CreateBody (def);

	box2dBody->CreateFixture (shape, 0);

	delete def;
}
