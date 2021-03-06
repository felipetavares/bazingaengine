#include "ZCabinetObject.h"
#include "ZEngine/ZEngine.h"
#include "ZObject/Objects/ZPlayerObject.h"

ZCabinetObject::ZCabinetObject(long int _oid, vec3 _position, vec3 _rotation):
    ZObject(_oid, _position, _rotation) {
    dclosed = true;
    dlocked = false;
}

void ZCabinetObject::init () {
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
    def->linearDamping = 5;

    if (shape != NULL)
        delete shape;

    shape = new b2PolygonShape();

    shape->SetAsBox (size->x/2,size->y/2);

    box2dBody = Engine->box2dWorld->CreateBody (def);

    b2FixtureDef *fixtureDef = new b2FixtureDef();
    fixtureDef->shape = shape;
    fixtureDef->density = 0.3;
    fixtureDef->friction = 0.3f;
    fixtureDef->filter.groupIndex = (int)phys;

    box2dBody->CreateFixture (fixtureDef);

    delete fixtureDef;
    delete def;

    mapped = true;
}

void ZCabinetObject::draw () {
}

void ZCabinetObject::interact (ZObject* _object) {
    if (dclosed) {
        Engine->guiManager->addSpeak (new ZUI::Speak("Hi. Welcome to IF-Games demo v1.1.3. Redistribution allowed only for CCSL-IFRN members."));
        graphic->texture = Engine->assetsManager->getAsset<ZTextureAsset*> ("image.cabinet.01");
        dclosed = false;
       //((ZPlayerObject*)_object)->getInventory()->addCategory (content);
    } else {
        graphic->texture = Engine->assetsManager->getAsset<ZTextureAsset*> ("image.cabinet.00");
        dclosed = true;
       //((ZPlayerObject*)_object)->getInventory()->removeCategory (content.id);
    }
}

string ZCabinetObject::getName() {
    return "Armário";
}

string ZCabinetObject::getType() {
    return "cabinet";
}

PI::Item* ZCabinetObject::get () {
    return NULL;
}

void ZCabinetObject::put (PI::Item* item) {
}
