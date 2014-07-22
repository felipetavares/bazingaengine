#ifndef ZCABINET_OBJECT_H
#define ZCABINET_OBJECT_H

#include "ZUtil/ZUtil.h"
#include "ZObject/ZObject.h"

class ZCabinetObject : public ZObject {
        bool dclosed;
        bool dlocked;
        Selector selector;
    public:
        ZCabinetObject(long int, vec3=vec3(), vec3=vec3());
        void init();
        void interact (ZObject*);
        void draw();
        string getName();
        string getType();
};

#endif /* ZCABINETtOBJECT_H */
