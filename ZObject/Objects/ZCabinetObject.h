#ifndef ZCABINET_OBJECT_H
#define ZCABINET_OBJECT_H

#include "ZUtil/ZUtil.h"
#include "ZObject/ZObject.h"

class ZCabinetObject : public ZObject {
        bool dclosed;
        bool dlocked;
        PI::Category content;
    public:
        ZCabinetObject(long int, vec3=vec3(), vec3=vec3());
        void init();
        void interact (ZObject*);
        void draw();
        string getName();
        string getType();
        PI::Item* get();
        void put(PI::Item*);
};

#endif /* ZCABINETtOBJECT_H */
