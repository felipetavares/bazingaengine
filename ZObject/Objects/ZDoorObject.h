#ifndef ZDOOROBJECT_H
#define ZDOOROBJECT_H

#include "ZObject/ZObject.h"

class ZDoorObject : public ZObject {
        bool dclosed;
        bool dlocked;
    public:
        ZDoorObject(long int, vec3=vec3(), vec3=vec3());
        void interact (ZObject*);
        string getName();
        string getType();
};

#endif // ZDOOROBJECT_H
