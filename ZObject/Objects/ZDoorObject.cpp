#include "ZDoorObject.h"
#include "ZEngine/ZEngine.h"

ZDoorObject::ZDoorObject(long int _oid, vec3 _position, vec3 _rotation):
    ZObject(_oid, _position, _rotation) {
    dclosed = true;
    dlocked = false;
}

void ZDoorObject::interact (ZObject* _object) {
    if (dclosed) {
        graphic->texture = Engine->assetsManager->getAsset<ZTextureAsset*> ("image.door.01");
        dclosed = false;
        size->x = 8;
        graphic->size->x = 7;
        graphic->position->x += 1;
        position->x += 12;
        init();
    } else {
        graphic->texture = Engine->assetsManager->getAsset<ZTextureAsset*> ("image.door.00");
        dclosed = true;
        size->x = 32;
        position->x -= 12;
        graphic->size->x = 32;
        graphic->position->x -= 1;
        init();
    }
}

string ZDoorObject::getName() {
    return "Porta";
}

string ZDoorObject::getType() {
    return "door";
}
