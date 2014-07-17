#ifndef ZUTIL_H
#define ZUTIL_H

#include "ZMath/math/li/li.h"
#include "ZAssetsManager/ZAssetsManager.h"

#define tnow Engine->gameTime->currentTime

class ZUtil {
public:
	ZUtil();
	~ZUtil();
};

class Selector {
	vector <ZTextureAsset*> icons;
	li y;
public:
	Selector ();
	Selector (vector <ZTextureAsset*>);

	void setIcons(vector <ZTextureAsset*>);
	void draw();
};

#endif /* ZUTIL_H */