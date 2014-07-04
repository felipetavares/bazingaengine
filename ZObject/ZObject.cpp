#include "ZObject.h"
#include "../ZEngine/ZEngine.h"
#include <cmath>

// You can only create objects if you know it's
// oid (object id)

ZObject::ZObject (long int _oid,
		  Vec3 _position,
		  Vec3 _rotation) {
	oid = _oid;

	position = new Vec3 (_position);
	rotation = new Vec3 (_rotation);
	size  	 = new Vec3 ();

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
						Vec3 _position,
						Vec3 _rotation):
	ZObject (_oid, _position, _rotation)
{

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

ZBulletObject::ZBulletObject (long int _oid,
						Vec3 _position,
						Vec3 _rotation):
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
						Vec3 _position,
						Vec3 _rotation):
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

PO::Matches::Matches ():
    PO::Object(Engine->assetsManager->getAsset<ZTextureAsset*>(ZFilePath(".:Assets:Images:bush:jurema01.png"))) {

}

void PO::Matches::action (ZPlayerObject* _player, vector <Object*> _interactions) {

}

string PO::Matches::getName() {
    return "Matches";
}

PO::Object::Object (ZTextureAsset *_texture) {
	texture = _texture;
}

void PO::Object::draw (Vec3 _pos) {
	glPushMatrix();
		glTranslatef (_pos.x, _pos.y, _pos.z);

		glBindTexture (GL_TEXTURE_2D, Object::texture->id);
		glBegin(GL_QUADS);
			glTexCoord2f (0,0); glVertex3f(0, 0, 0);
			glTexCoord2f (texture->width/texture->rwidth,0); glVertex3f(texture->width, 0, 0);
			glTexCoord2f (texture->width/texture->rwidth,texture->height/texture->rheight); glVertex3f(texture->width, texture->height, 0);
			glTexCoord2f (0,texture->height/texture->rheight); glVertex3f(0, texture->height, 0);
		glEnd();

	glPopMatrix();
}

ZPlayerObject::ZPlayerObject (long int _oid,
						Vec3 _position,
						Vec3 _rotation):
	ZObject (_oid, _position, _rotation)
{
}

void ZPlayerObject::init () {
	if (box2dBody != NULL) {
		Engine->box2dWorld->DestroyBody (box2dBody);
		box2dBody = NULL;
	}

	b2BodyDef *def = new b2BodyDef();
	def->type = b2_dynamicBody;
	def->position.Set (position->x,position->y);
	def->userData = this;
	def->allowSleep = true;
	def->fixedRotation = true;
	def->linearDamping = 10;

	if (shape != NULL)
		delete shape;

	shape = new b2PolygonShape();

	shape->SetAsBox (size->x/2,size->y/2);

	box2dBody = Engine->box2dWorld->CreateBody (def);

	b2FixtureDef *fixtureDef = new b2FixtureDef();
	fixtureDef->shape = shape;
	fixtureDef->density = 0.1f;
	fixtureDef->friction = 1.0f;
	fixtureDef->filter.groupIndex = 3;

	box2dBody->CreateFixture (fixtureDef);

	delete fixtureDef;
	delete def;

	mapped = false;

    inventory.push_back(new PO::Matches());

    auto callback = [] (ZAnimation* _anim) {
        if (_anim->position == 0) {
            //Engine->assetsManager->getAsset <ZAudioAsset*> (ZFilePath (".:Assets:Audio:footsteps:sand.ogg"))->play();
        }
    };

    anims[0] = 	new ZAnimation(Engine->assetsManager->getAsset <ZAnimationAsset*> (ZFilePath (".:Assets:Animations:player-up.anim")));
    anims[0]->setCallback (callback);
    anims[1] = 	new ZAnimation(Engine->assetsManager->getAsset <ZAnimationAsset*> (ZFilePath (".:Assets:Animations:player-down.anim")));
    anims[0]->setCallback (callback);
    anims[3] = anims[2] = 	new ZAnimation(Engine->assetsManager->getAsset <ZAnimationAsset*> (ZFilePath (".:Assets:Animations:player-left.anim")));
    anims[2]->setCallback (callback);
    //anims[3] = 	new ZAnimation(Engine->assetsManager->getAsset <ZAnimationAsset*> (ZFilePath (".:Assets:Animations:player-right.anim")));

    graphic->animation = anims[0];
}


void ZPlayerObject::step () {
	if (reinit) {
		init();
		reinit = false;
	}

	ZObject::step();

	if (Engine->editMode)
		return;

    auto keyboard = Engine->inputManager->getKeyboards()[0];

	//if (graphic->animation != NULL)
	//	graphic->animation->isPlaying = false;

	///dir = Vec3();

    graphic->animation->isPlaying = false;

    if (getAxis(1) < -0.1 || keyboard->keys[SDLK_UP]) {
        box2dBody->ApplyLinearImpulse (b2Vec2 (0,-80), box2dBody->GetWorldPoint(b2Vec2(0,0)));
        graphic->animation = anims[0];
        graphic->animation->isPlaying = true;
        dir.y = -1;
        dir.x = 0;
    }
    if (getAxis(1) > 0.1 || keyboard->keys[SDLK_DOWN]) {
        box2dBody->ApplyLinearImpulse (b2Vec2 (0,80), box2dBody->GetWorldPoint(b2Vec2(0,0)));
        graphic->animation = anims[1];
        graphic->animation->isPlaying = true;
        dir.y = +1;
        dir.x = 0;
    }
    if (getAxis(0) < -0.1 || keyboard->keys[SDLK_LEFT]) {
        box2dBody->ApplyLinearImpulse (b2Vec2 (-80,0), box2dBody->GetWorldPoint(b2Vec2(0,0)));
        graphic->animation = anims[2];
        graphic->animation->isPlaying = true;
        graphic->animation->flipH = false;
        dir.x = -1;
        dir.y = 0;
    }
    if (getAxis(0) > 0.1 || keyboard->keys[SDLK_RIGHT]) {
        box2dBody->ApplyLinearImpulse (b2Vec2 (80,0), box2dBody->GetWorldPoint(b2Vec2(0,0)));
        graphic->animation = anims[3];
        graphic->animation->isPlaying = true;
        graphic->animation->flipH = true;
        dir.x = +1;
        dir.y = 0;
    }
}

void ZPlayerObject::drawInventory () {
	glPushMatrix();

	Vec3 p = {0,0,0};

	for (auto o :inventory) {
		o->draw(p);
		p.y += 10;
	}

	glPopMatrix();
}

void ZPlayerObject::draw () {
	drawInventory();
}

float ZPlayerObject::getAxis(int _axis) {
    auto joys = Engine->inputManager->getJoysticks();

    if (joys.size() > playerNum) {
        return joys[playerNum]->axis[_axis].value;
	}
	return 0;
}

void ZPlayerObject::shot () {
	if (dir.x == 0 && dir.y == 0)
		return;

	ShotCallback callback;
	b2Vec2 point1(position->x+dir.x*2, position->y+dir.y*2);
	b2Vec2 point2(position->x+dir.x*320, position->y+dir.y*320);
	//Engine->box2dWorld->RayCast(&callback, point1, point2);

	if (!callback.c)
		return;

	ZObject* bullet = Engine->createObject <ZBulletObject> ();

	bullet->position->x = callback.m_point.x;
	bullet->position->y = callback.m_point.y;

	bullet->graphic->position->x = 0;
	bullet->graphic->position->y = 0;

	bullet->graphic->texture = Engine->assetsManager->getAsset <ZTextureAsset*> (ZFilePath (".:Assets:bullet.png"));

	bullet->size->x = bullet->graphic->texture->width;
	bullet->size->y = bullet->graphic->texture->height;

	bullet->graphic->size->x = bullet->graphic->texture->width;
	bullet->graphic->size->y = bullet->graphic->texture->height;

	Engine->addObject (bullet);
}


ShotCallback::ShotCallback () {
	m_fixture = NULL;
}

float32 ShotCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
					const b2Vec2& normal, float32 fraction) {
	const b2Filter& filter = fixture->GetFilterData();

	if (filter.groupIndex != 0)
		if (fraction < m_fraction) {
			c = true;
			m_fixture = fixture;
			m_point = point;
			m_normal = normal;
			m_fraction = fraction;
		}

	return 1;
}
