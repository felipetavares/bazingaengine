#include "ZEngine/ZEngine.h"
#include "ZObject/Objects/ZPlayerObject.h"
#include "ZUtil/ZUtil.h"

PI::Matches::Matches ():
	PI::Item(Engine->assetsManager->getAsset<ZTextureAsset*>("image.matches")) {

}

void PI::Matches::action (ZPlayerObject* _player, vector <PI::Item*> _interactions) {

}

string PI::Matches::getName() {
	return "Fósforos";
}

PI::Item::Item (ZTextureAsset *_texture) {
	texture = _texture;
}

void PI::Item::draw (vec3 _pos) {
	texture->drawCentered(_pos);

	string name = getName();
	Engine->textManager->setColor(vec3(0,0,0));
	Engine->textManager->drawStringCentered (_pos+vec3(0,texture->height/2+12,0),name,16);
}

vec2 PI::Item::getSize () {
	string name = getName();
	// 20 = 16 + 4
	return vec2 (Engine->textManager->measureString (name,16), texture->height+20);
}

PI::Category::Category (string _iconName) {
	icon = Engine->assetsManager->getAsset<ZTextureAsset*> (_iconName);
}

ZTextureAsset* PI::Category::getIcon () {
	return icon;
}

void PI::Category::draw () {
	glPushMatrix();
	glLoadIdentity();
	glDisable (GL_DEPTH_TEST);

	//glTranslatef (0, translation.v()*items[0]->texture->height, 0);

	for (int i=0;i<items.size();i++) {
		auto icon = items[i]->texture;
		icon->draw(vec3(64, i*icon->height, 0));
	}

	glEnable (GL_DEPTH_TEST);
	glPopMatrix();
}

void PI::Category::addItem (Item* _item) {
	items.push_back(_item);
}

PI::Inventory::Inventory ():
	translation(vec2(0,0), vec2(0,0)) {
	position = 0;
}

void PI::Inventory::addCategory (Category _category) {
	categories.push_back (_category);
}

void PI::Inventory::draw () {
	glPushMatrix();
	glLoadIdentity();
	glDisable (GL_DEPTH_TEST);

	glTranslatef (0, translation.v()*categories[0].getIcon()->height, 0);

	for (int i=0;i<categories.size();i++) {
		auto icon = categories[i].getIcon();
		icon->draw(vec3(0, i*icon->height, 0));
	}

	glEnable (GL_DEPTH_TEST);
	glPopMatrix();

	if (position < categories.size())
		categories[position].draw();
}

void PI::Inventory::up () {
	if (!translation.complete())
		return;

	if (position < categories.size()-1)
		position++;
	else
		position = 0;

	translation = li(vec2(tnow, tnow+0.3), vec2(translation.v(), -position));
}

void PI::Inventory::down () {
	if (!translation.complete())
		return;

	if (position > 0)
		position--;
	else
		position = categories.size()-1;

	translation = li(vec2(tnow, tnow+0.3), vec2(translation.v(), -position));
}

ZPlayerObject::ZPlayerObject (long int _oid,
						vec3 _position,
						vec3 _rotation):
	ZObject (_oid, _position, _rotation) {
	openInventory = false;

	for (int i=0;i<10;i++) {
		auto cat = PI::Category("image.matches");
		auto n = rand()%10;

		for (int j=0;j<n;j++)
			cat.addItem (new PI::Matches());

		inventory.addCategory(cat);
	}
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

	auto callback = [] (ZAnimation* _anim) {
	if (_anim->position == 0) {
		//Engine->assetsManager->getAsset <ZAudioAsset*> ("audio.effect.sand")->play();
	}
	};

	anims[0] = 	new ZAnimation(Engine->assetsManager->getAsset <ZAnimationAsset*> ("animation.player.up"));
	anims[0]->setCallback (callback);
	anims[1] = 	new ZAnimation(Engine->assetsManager->getAsset <ZAnimationAsset*> ("animation.player.down"));
	anims[0]->setCallback (callback);
	anims[3] = anims[2] = 	new ZAnimation(Engine->assetsManager->getAsset <ZAnimationAsset*> ("animation.player.left"));
	anims[2]->setCallback (callback);

	graphic->animation = anims[0];
}

	/*
	if (keyboard->isShot(SDLK_SPACE) || joyButton(0)) {
	}
	*/

void ZPlayerObject::step () {
	if (reinit) {
		init();
		reinit = false;
	}

	ZObject::step();

	if (Engine->editMode)
		return;

	auto keyboard = Engine->inputManager->getKeyboards()[0];

	graphic->animation->isPlaying = false;

	if (openInventory) {
		if (getAxis(1) < -0.1 || keyboard->keys[SDLK_UP]) {
			inventory.down();
		}
		if (getAxis(1) > 0.1 || keyboard->keys[SDLK_DOWN]) {
			inventory.up();
		}
		//if (getAxis(0) < -0.1 || keyboard->isShot(SDLK_LEFT)) {
			//inventory.left();
		//}
		//if (getAxis(0) > 0.1 || keyboard->isShot(SDLK_RIGHT)) {
			//inventory.right();
		//}
	}

	if (keyboard->isShot(SDLK_i) || joyButton (0)) {
		openInventory = !openInventory;
	}

	if (keyboard->isShot(SDLK_SPACE)) {
		ShotCallback callback;

		vec3 d = (*position)+dir*15;

		b2vec2 point1(position->x, position->y);
		b2vec2 point2(d.x,d.y);
		Engine->box2dWorld->RayCast(&callback, point1, point2);

		if (callback.c) {
			((ZObject*)callback.m_fixture->GetBody()->GetUserData())->interact(this);
		}
	}

	if (openInventory)
		return;

	if (getAxis(1) < -0.1 || keyboard->keys[SDLK_UP]) {
		box2dBody->ApplyLinearImpulse (b2vec2 (0,-80), box2dBody->GetWorldPoint(b2vec2(0,0)));
		graphic->animation = anims[0];
		graphic->animation->isPlaying = true;
		dir.y = -1;
		dir.x = 0;
	}
	if (getAxis(1) > 0.1 || keyboard->keys[SDLK_DOWN]) {
		box2dBody->ApplyLinearImpulse (b2vec2 (0,80), box2dBody->GetWorldPoint(b2vec2(0,0)));
		graphic->animation = anims[1];
		graphic->animation->isPlaying = true;
		dir.y = +1;
		dir.x = 0;
	}
	if (getAxis(0) < -0.1 || keyboard->keys[SDLK_LEFT]) {
		box2dBody->ApplyLinearImpulse (b2vec2 (-80,0), box2dBody->GetWorldPoint(b2vec2(0,0)));
		graphic->animation = anims[2];
		graphic->animation->isPlaying = true;
		graphic->animation->flipH = false;
		dir.x = -1;
		dir.y = 0;
	}
	if (getAxis(0) > 0.1 || keyboard->keys[SDLK_RIGHT]) {
		box2dBody->ApplyLinearImpulse (b2vec2 (80,0), box2dBody->GetWorldPoint(b2vec2(0,0)));
		graphic->animation = anims[3];
		graphic->animation->isPlaying = true;
		graphic->animation->flipH = true;
		dir.x = +1;
		dir.y = 0;
	}
}

void ZPlayerObject::draw () {
	inventory.draw();
}

float ZPlayerObject::getAxis(int _axis) {
	auto joys = Engine->inputManager->getJoysticks();

	if (joys.size() > playerNum) {
		return joys[playerNum]->axis[_axis].value;
	}
	return 0;
}

bool ZPlayerObject::joyButton (int _bt) {
	auto joys = Engine->inputManager->getJoysticks();

	if (joys.size() > playerNum) {
		if (_bt < joys[playerNum]->buttons.size()) {
			return joys[playerNum]->buttons[_bt].isShot();
		}
	}

	return false;
}

void ZPlayerObject::shot () {
	if (dir.x == 0 && dir.y == 0)
		return;

	ShotCallback callback;
	b2vec2 point1(position->x+dir.x*2, position->y+dir.y*2);
	b2vec2 point2(position->x+dir.x*320, position->y+dir.y*320);
	//Engine->box2dWorld->RayCast(&callback, point1, point2);

	if (!callback.c)
		return;

	ZObject* bullet = Engine->createObject <ZBulletObject> ();

	bullet->position->x = callback.m_point.x;
	bullet->position->y = callback.m_point.y;

	bullet->graphic->position->x = 0;
	bullet->graphic->position->y = 0;

	bullet->graphic->texture = Engine->assetsManager->getAsset <ZTextureAsset*> (ZFilePath ("image.door.00"));

	bullet->size->x = bullet->graphic->texture->width;
	bullet->size->y = bullet->graphic->texture->height;

	bullet->graphic->size->x = bullet->graphic->texture->width;
	bullet->graphic->size->y = bullet->graphic->texture->height;

	Engine->addObject (bullet);
}


ShotCallback::ShotCallback () {
	m_fixture = NULL;
}

float32 ShotCallback::ReportFixture(b2Fixture* fixture, const b2vec2& point,
					const b2vec2& normal, float32 fraction) {
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

InteractCallback::InteractCallback () {
	m_fixture = NULL;
}

float32 InteractCallback::ReportFixture(b2Fixture* fixture, const b2vec2& point,
					const b2vec2& normal, float32 fraction) {
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
