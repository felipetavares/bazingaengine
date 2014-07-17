#ifndef ZUTIL_H
#define ZUTIL_H

#include "ZMath/math/li/li.h"
#include "ZEngineDecl.h"

class ZUtil {
public:
	ZUtil();
	~ZUtil();
};

class Selector {
	vector <ZTextureAsset> icons;	
	li y;
public:
	Selector (vector <ZTextureAsset*>);

	void draw();
};

#endif /* ZUTIL_H */