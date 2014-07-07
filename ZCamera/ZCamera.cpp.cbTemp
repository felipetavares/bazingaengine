#include "../ZEngine/ZEngine.h"
#include "ZCamera.h"

ZCamera::ZCamera () {
	position = new Vec3();
	scale	 = new Vec3(3,3,1);
    drag = false;
    dragPos = Vec2(0,0);
}

ZCamera::~ZCamera () {
	delete position;
	delete scale;
}

void ZCamera::run () {
	ZObject* player = Engine->getObject (playerOid);
	if (player) {
		if (!Engine->editMode) {
            position->x = player->position->x;
            position->y = player->position->y;
		}
	}
}

void ZCamera::debugMovement () {
	// Joystick movement
    if (Engine->inputManager->getJoysticks().size() > 0) {
    	position->x += Engine->inputManager->getJoysticks()[0]->axis[0].value*100*Engine->realTime->deltaTime;
    	position->y += Engine->inputManager->getJoysticks()[0]->axis[1].value*100*Engine->realTime->deltaTime;

    	if (!(scale->x < 0.5 && Engine->inputManager->getJoysticks()[0]->axis[3].value > 0))
			scale->x -= Engine->inputManager->getJoysticks()[0]->axis[3].value*10*Engine->realTime->deltaTime;
    	if (!(scale->y < 0.5 && Engine->inputManager->getJoysticks()[0]->axis[3].value > 0))
			scale->y -= Engine->inputManager->getJoysticks()[0]->axis[3].value*10*Engine->realTime->deltaTime;
		//angle += Engine->inputManager->getJoysticks()[0]->axis[2].value;
	}

    // Mouse movement
    if (Engine->inputManager->getMice().size() > 0 &&
        Engine->inputManager->getKeyboards().size() > 0) {
    	auto mouse = Engine->inputManager->getMice()[0];
    	auto keyboard = Engine->inputManager->getKeyboards()[0];

        if (!drag && mouse->left && keyboard->keys[SDLK_SPACE]) {
            drag = true;
            dragPos = Vec2(mouse->pos.x,mouse->pos.y);
        }
        if (!mouse->left || !keyboard->keys[SDLK_SPACE]) {
            drag = false;
        }
        if (drag) {
            position->x -= (mouse->pos.x - dragPos.x)/scale->x;
            position->y -= (mouse->pos.y - dragPos.y)/scale->y;
            dragPos = Vec2(mouse->pos.x,mouse->pos.y);
        }
    }
}
