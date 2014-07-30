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

PI::Inventory::Inventory ():
	y(vec2(tnow, tnow), vec2(Engine->videoManager->windowHeight/2,
						   Engine->videoManager->windowHeight/2)),
	angle(vec2(tnow,tnow), vec2(0,0)) {

	currentItem = 0;
	display = false;
	back = Engine->assetsManager->getAsset <ZTextureAsset*>("image.menu.back");
	top = Engine->assetsManager->getAsset <ZTextureAsset*>("image.menu.top");
}

void PI::Inventory::addItem (Item* _item) {
	items.push_back (_item);
}

void PI::Inventory::removeItem (Item* _item) {
	int p = 0;
	for (auto i :items) {
		if (i == _item) {
			items.erase(items.begin()+p);
			if (currentItem >= p && currentItem > 0) {
				currentItem--;
			}
			break;
		}
		p++;
	}
}

void PI::Inventory::setDisplay (bool _display) {
	// Close
	if (display == true && _display == false) {
		y = li(vec2(tnow, tnow+0.3), vec2(y.v(), Engine->videoManager->windowHeight/2));
	}
	else if (display == false && _display == true) {
		y = li(vec2(tnow, tnow+0.3), vec2(y.v(), 0));
	}

	display = _display;
}

void PI::Inventory::draw() {
	//if (!display)
	//	return;

	float windowWidth = Engine->videoManager->windowWidth;
	float windowHeight = Engine->videoManager->windowHeight;

	float startY = 0;
	if (getItem()) {
		startY = -getItem()->getSize().y*currentItem;
	}

	glDisable (GL_DEPTH_TEST);
	glPushMatrix();
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, back->id);
		glColor4f(0.25,0.25,0.25,1);
		float w = windowWidth*2/(float)back->rwidth/4;
		float h = windowHeight*2/(float)back->rheight/4;
		glBegin(GL_QUADS);
			glTexCoord2f (0,0);
			glVertex3f(-windowWidth, y.v(), 0);
			glTexCoord2f (w,0);
			glVertex3f(windowWidth, y.v(), 0);
			glTexCoord2f (w,h);
			glVertex3f(windowWidth, y.v()+windowHeight, 0);
			glTexCoord2f (0,h);
			glVertex3f(-windowWidth, y.v()+windowHeight, 0);
		glEnd();
		w = windowWidth*2/(float)top->rwidth/4;
		glBindTexture(GL_TEXTURE_2D, top->id);
		glBegin(GL_QUADS);
			glTexCoord2f (0,0);
			glVertex3f(-windowWidth, y.v()-8, 0);
			glTexCoord2f (w,0);
			glVertex3f(windowWidth, y.v()-8, 0);
			glTexCoord2f (w,1);
			glVertex3f(windowWidth, y.v()+top->rheight-8, 0);
			glTexCoord2f (0,1)	;
			glVertex3f(-windowWidth, y.v()+top->rheight-8, 0);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glLoadIdentity();

		glTranslatef(0, y.v()+windowHeight/4, 0);

		glTranslatef(items[0]->getSize().x*angle.v(), 0, 0);

		float n = 0;
		for (auto i :items) {
			glPushMatrix();
				glTranslatef (n, 0, 0);
				if (i)
					i->draw(vec3());
			glPopMatrix();
			n+=i->getSize().x;
		}

	glPopMatrix();

	glEnable (GL_DEPTH_TEST);
}

PI::Item* PI::Inventory::getItem () {
	if (currentItem >= 0 &&
		currentItem < items.size())
		return items[currentItem];
}

void PI::Inventory::nextItem() {
	if (!angle.complete())
		return;

	if (currentItem < items.size()-1)
		currentItem++;
	else
		currentItem = 0;
	// New angle
	float na = angle.v()-1;
	angle = li(vec2(tnow, tnow+0.3), vec2(angle.v(), na));
}

void PI::Inventory::prevItem() {
	if (!angle.complete())
		return;

	if (currentItem > 0)
		currentItem--;
	else
		currentItem = items.size()-1;
	float na = angle.v()+1;
	angle = li(vec2(tnow, tnow+0.3), vec2(angle.v(), na));
}

PI::Item* PI::Inventory::pullItem() {
	if (items.size() > 0) {
		auto ret = items[currentItem];

		items.erase (items.begin()+currentItem);

		if (currentItem > 0)
			currentItem--;

		float na = (float)(currentItem)/(float)items.size()*360;
		angle = li(vec2(tnow, tnow+0.3), vec2(angle.v(), na));		

		return ret;
	} else {
		return NULL;
	}
}

ZPlayerObject::ZPlayerObject (long int _oid,
						vec3 _position,
						vec3 _rotation):
	ZObject (_oid, _position, _rotation) {
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

	// Add inventory items
	// A lot of matches :P
	inventory.addItem(new PI::Matches());
	inventory.addItem(new PI::Matches());
	inventory.addItem(new PI::Matches());
	inventory.addItem(new PI::Matches());
	inventory.addItem(new PI::Matches());
	inventory.addItem(new PI::Matches());
	inventory.addItem(new PI::Matches());
	inventory.addItem(new PI::Matches());

	openInventory = false;
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

	graphic->animation->isPlaying = false;

	if (keyboard->isShot(SDLK_i) || joyButton(1)) {
		openInventory = !openInventory;
	}

	if (keyboard->isShot(SDLK_p) || joyButton(2)) {
		ShotCallback callback;

		vec3 d = (*position)+dir*15;

		b2vec2 point1(position->x, position->y);
		b2vec2 point2(d.x,d.y);
		Engine->box2dWorld->RayCast(&callback, point1, point2);
	
		if (callback.c) {
			((ZObject*)callback.m_fixture->GetBody()->GetUserData())->put(inventory.pullItem());
			//interacting = !interacting;
		}
	}

	if (openInventory) {
		inventory.setDisplay(true);
		if (getAxis(0) > 0.1 || keyboard->isShot(SDLK_RIGHT)) {
			inventory.prevItem();
		}
		if (getAxis(0) < -0.1 || keyboard->isShot(SDLK_LEFT)) {
			inventory.nextItem();
		}
		return;
	} else {
		inventory.setDisplay(false);		
	}


	if (keyboard->isShot(SDLK_SPACE) || joyButton(0)) {
		ShotCallback callback;

		vec3 d = (*position)+dir*15;

		b2vec2 point1(position->x, position->y);
		b2vec2 point2(d.x,d.y);
		Engine->box2dWorld->RayCast(&callback, point1, point2);
	
		if (callback.c) {
			((ZObject*)callback.m_fixture->GetBody()->GetUserData())->interact(this);
			//interacting = !interacting;
		}
	}

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
