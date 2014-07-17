#include "../ZEngine/ZEngine.h"
#include "ZEditor.h"

ZEditor::ZEditor () {
	drag = false;
	subject = NULL;
	gridSize = new vec3(1,1,1);
	gridColor = new vec3 (1,0,1);
	grid = false;
}

ZEditor::~ZEditor () {
	delete gridSize;
	delete gridColor;
}

void ZEditor::drawHUD () {
	if (subject) {
		glPushMatrix();
			glScalef (0.5,0.5,0.5);
			vec3 top = vec3(-Engine->videoManager->windowWidth/2/0.5,-Engine->videoManager->windowHeight/2/0.5,50);
			string oPos = "Pos  " + subject->position->str();
			string oSiz = "Size " + subject->size->str();
			string ogPos = "gaphPos  " + subject->graphic->position->str();
			string ogSiz = "graphSize " + subject->graphic->size->str();
			string oCol  = "Col ";

			if (subject->phys)
				oCol += "true";
			else
				oCol += "false";

			Engine->textManager->drawString (top, oPos, 32);
			top.y += 40;
			Engine->textManager->drawString (top, oSiz, 32);
			top.y += 40;
			Engine->textManager->drawString (top, ogPos, 32);
			top.y += 40;
			Engine->textManager->drawString (top, ogSiz, 32);
			top.y += 40;
			Engine->textManager->drawString (top, oCol, 32);

		glPopMatrix();
	}
}

void ZEditor::run () {
    if (Engine->inputManager->getMice().size() > 0) {
		vec3 *mp = new vec3();

		mp->x = (Engine->camera->position->x-Engine->videoManager->windowWidth/2/Engine->camera->scale->x)+Engine->inputManager->getMice()[0]->pos.x/Engine->camera->scale->x;
		mp->y = (Engine->camera->position->y-Engine->videoManager->windowHeight/2/Engine->camera->scale->y)+Engine->inputManager->getMice()[0]->pos.y/Engine->camera->scale->y;

		auto bcall = new b2qGetFirstObject();

		b2AABB mouseAABB;

		float sX = 2.5/Engine->camera->scale->x;
		float sY = 2.5/Engine->camera->scale->y;

		mouseAABB.lowerBound.Set (mp->x-sX,mp->y-sY);
		mouseAABB.upperBound.Set (mp->x+sX,mp->y+sY);

		Engine->box2dWorld->QueryAABB (bcall, mouseAABB);

		if (!Engine->inputManager->getMice()[0]->left && drag) {
			drag = false;
			float ang = subject->box2dBody->GetAngle();

			float gridedX,gridedY;

			gridedX = int((mp->x+dragPos.x)/gridSize->x) *gridSize->x;
			gridedY = int((mp->y+dragPos.y)/gridSize->y) *gridSize->y;

			gridedX += subject->size->x/2;
			gridedY += subject->size->y/2;

			subject->box2dBody->SetTransform (b2vec2((float)gridedX,(float)gridedY),ang);
		}

		if (drag) {
			float ang = subject->box2dBody->GetAngle();

			float gridedX,gridedY;

			gridedX = mp->x+dragPos.x;
			gridedY = mp->y+dragPos.y;

			gridedX += subject->size->x/2;
			gridedY += subject->size->y/2;

			subject->box2dBody->SetTransform (b2vec2((float)gridedX,(float)gridedY),ang);
		}

		if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_TAB)) {
			grid = !grid;
			if (grid) {
				gridSize->x = 32;
				gridSize->y = 32;
				gridSize->z = 32;
			} else {
				gridSize->x = 1;
				gridSize->y = 1;
				gridSize->z = 1;
			}
		}

		if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_UP)) {
			gridSize->x *= 2;
			gridSize->y *= 2;
		}

		if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_DOWN)) {
			if (gridSize->x > 1 && gridSize->y > 1) {
				gridSize->x *= 0.5;
				gridSize->y *= 0.5;
			}
		}

		if (bcall->object) {
			if (!drag)
				subject = bcall->object;

	    	if (Engine->inputManager->getMice()[0]->leftIsShot()) {
    			drag = true;
    			subject = bcall->object;
    			dragPos.x = bcall->object->position->x - mp->x - bcall->object->size->x/2;
    			dragPos.y = bcall->object->position->y - mp->y - bcall->object->size->y/2;
    			//bcall->object->graphic->size->x *= 2;
    			//bcall->object->graphic->size->y *= 2;
			} else if (Engine->inputManager->getMice()[0]->rightIsShot()) {
    			//bcall->object->graphic->size->x *= 0.5;
    			//bcall->object->graphic->size->y *= 0.5;
	   			long int tAid;
	   			if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_LSHIFT])
	   				tAid = findPrevTextureAsset(bcall->object->graphic->texture->getAid());
	   			else
	   				tAid = findNextTextureAsset(bcall->object->graphic->texture->getAid());
	   			ZTextureAsset* asset = Engine->assetsManager->getAsset <ZTextureAsset*> (tAid);
	   			bcall->object->graphic->texture = asset;
	   			bcall->object->init();
	   		} else if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_LEFT)) {
	   			bcall->object->rotation->z -= 10;
				bcall->object->box2dBody->SetTransform (bcall->object->box2dBody->GetPosition(),bcall->object->rotation->z/57.2957795);
	   		} else if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_RIGHT)) {
	   			bcall->object->rotation->z += 10;
				bcall->object->box2dBody->SetTransform (bcall->object->box2dBody->GetPosition(),bcall->object->rotation->z/57.2957795);
	   		} else if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_s]) {
				if (abs(Engine->inputManager->getMice()[0]->pos.z) >= 1) {
		   			if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_x]) {
			   			if (!(bcall->object->graphic->size->x <= gridSize->x && Engine->inputManager->getMice()[0]->pos.z < 0))
	   						bcall->object->graphic->size->x += Engine->inputManager->getMice()[0]->pos.z*gridSize->x;
						else
							Engine->inputManager->getMice()[0]->pos.z = 0;
		   			} else if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_y]) {
			   			if (!(bcall->object->graphic->size->y <= gridSize->y && Engine->inputManager->getMice()[0]->pos.z < 0))
		   					bcall->object->graphic->size->y += Engine->inputManager->getMice()[0]->pos.z*gridSize->y;
						else
							Engine->inputManager->getMice()[0]->pos.z = 0;
					} else {
			   			if (!(bcall->object->graphic->size->x <= gridSize->x && Engine->inputManager->getMice()[0]->pos.z < 0) &&
			   				!(bcall->object->graphic->size->y <= gridSize->y && Engine->inputManager->getMice()[0]->pos.z < 0)) {
		   					bcall->object->graphic->size->x += Engine->inputManager->getMice()[0]->pos.z*gridSize->x;
		   					bcall->object->graphic->size->y += Engine->inputManager->getMice()[0]->pos.z*gridSize->y;
		   				}
						else
							Engine->inputManager->getMice()[0]->pos.z = 0;
		   			}
					Engine->inputManager->getMice()[0]->pos.z = 0;
				}
	   			bcall->object->init();
	   		} else if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_d]) {
				if (abs(Engine->inputManager->getMice()[0]->pos.z) >= 1) {
		   			if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_x]) {
			   			if (!(bcall->object->size->x <= gridSize->x && Engine->inputManager->getMice()[0]->pos.z < 0))
	   						bcall->object->size->x += Engine->inputManager->getMice()[0]->pos.z*gridSize->x;
						else
							Engine->inputManager->getMice()[0]->pos.z = 0;
		   			} else if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_y]) {
			   			if (!(bcall->object->size->y <= gridSize->y && Engine->inputManager->getMice()[0]->pos.z < 0))
		   					bcall->object->size->y += Engine->inputManager->getMice()[0]->pos.z*gridSize->y;
						else
							Engine->inputManager->getMice()[0]->pos.z = 0;
					} else {
			   			if (!(bcall->object->size->x <= gridSize->x && Engine->inputManager->getMice()[0]->pos.z < 0) &&
			   				!(bcall->object->size->y <= gridSize->y && Engine->inputManager->getMice()[0]->pos.z < 0)) {
		   					bcall->object->size->x += Engine->inputManager->getMice()[0]->pos.z*gridSize->x;
		   					bcall->object->size->y += Engine->inputManager->getMice()[0]->pos.z*gridSize->y;
		   				}
						else
							Engine->inputManager->getMice()[0]->pos.z = 0;
		   			}
					Engine->inputManager->getMice()[0]->pos.z = 0;
				}
	   			bcall->object->init();
	   		} else if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_p]) {
				if (abs(Engine->inputManager->getMice()[0]->pos.z) >= 1) {
		   			if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_x]) {
	   						bcall->object->graphic->position->x += Engine->inputManager->getMice()[0]->pos.z*gridSize->x;
		   			} else if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_y]) {
		   					bcall->object->graphic->position->y += Engine->inputManager->getMice()[0]->pos.z*gridSize->y;
					} else {
		   					bcall->object->graphic->position->x += Engine->inputManager->getMice()[0]->pos.z*gridSize->x;
		   					bcall->object->graphic->position->y += Engine->inputManager->getMice()[0]->pos.z*gridSize->y;
		   			}
					Engine->inputManager->getMice()[0]->pos.z = 0;
	   			}

	   			bcall->object->init();
	   		} else if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_g)) {
				ZObject* object = NULL;

				object = Engine->createObject <ZBoxObject> ();

				object->position->x = bcall->object->position->x + gridSize->x;
				object->position->y = bcall->object->position->y + gridSize->y;
				object->position->z = bcall->object->position->z;

				object->graphic->position->x = bcall->object->graphic->position->x;
				object->graphic->position->y = bcall->object->graphic->position->y;
				object->graphic->position->z = bcall->object->graphic->position->z;

				object->graphic->texture = bcall->object->graphic->texture;

				object->size->x = bcall->object->size->x;
				object->size->y = bcall->object->size->y;

				object->graphic->size->x = bcall->object->graphic->size->x;
				object->graphic->size->y = bcall->object->graphic->size->y;

				object->phys = bcall->object->phys;
				object->background = bcall->object->background;

				Engine->addObject (object);
	   		} else if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_r)) {
				Engine->removeObject (bcall->object->oid);
	   		} else if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_c)) {
	   			bcall->object->phys = !bcall->object->phys;
	   			bcall->object->init();
	   		} else if (Engine->inputManager->getKeyboards()[0]->isShot(SDLK_o)) {
	   			bcall->object->background = !bcall->object->background;
	   		}
	   		else if (Engine->inputManager->getKeyboards()[0]->keys[SDLK_z]) {
				if (abs(Engine->inputManager->getMice()[0]->pos.z) >= 1) {
					bcall->object->graphic->position->z += Engine->inputManager->getMice()[0]->pos.z*0.01;
					Engine->inputManager->getMice()[0]->pos.z = 0;
		   			bcall->object->init();
		   		}
	   		}
		}

		delete bcall;
		delete mp;
	}
}

void ZEditor::createObject (unsigned int _aid=0) {
	ZObject* object = NULL;

	object = Engine->createObject <ZBoxObject> ();

	object->position->x = 0;
	object->position->y = 0;

	object->graphic->position->x = 0;
	object->graphic->position->y = 0;

	if (_aid == 0)
		object->graphic->texture = Engine->assetsManager->getAsset <ZTextureAsset*> (findNextTextureAsset(_aid));
	else
		object->graphic->texture = Engine->assetsManager->getAsset <ZTextureAsset*> (_aid);

	object->size->x = object->graphic->texture->width;
	object->size->y = object->graphic->texture->height;

	object->graphic->size->x = object->graphic->texture->width;
	object->graphic->size->y = object->graphic->texture->height;

    object->phys = true;

	Engine->addObject (object);
}

long int ZEditor::findNextTextureAsset (long int _prev) {
	long int aSize = Engine->assetsManager->getAssetsNum();
	long int q=0; // Counter

	long int init = _prev++;

	while (q < aSize) {
		if (_prev >= aSize)
			_prev = 0;

		ZAsset* a = Engine->assetsManager->getAsset <ZAsset*> (_prev++);
		if (a->type == ZAsset::Type::Texture)
			return _prev-1;

		q++;
	}

	return init;
}


long int ZEditor::findPrevTextureAsset (long int _prev) {
	long int aSize = Engine->assetsManager->getAssetsNum();
	long int q=0; // Counter

	long int init = _prev--;

	while (q < aSize) {
		if (_prev <= 0)
			_prev = aSize-1;

		ZAsset* a = Engine->assetsManager->getAsset <ZAsset*> (_prev--);
		if (a->type == ZAsset::Type::Texture)
			return _prev+1;

		q++;
	}

	return init;
}
