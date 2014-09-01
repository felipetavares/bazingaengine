#ifndef ZUTIL_H
#define ZUTIL_H

#include "ZMath/math/li/li.h"
#include "ZAssetsManager/ZAssetsManager.h"
#include "ZObject/Objects/PI.h"

#define tnow Engine->gameTime->currentTime

class ZUtil {
public:
	ZUtil();
	~ZUtil();

	void fillRect (vec2, vec2, float);
	void strokeRect (vec2, vec2, float);
};

class Selector {
	vector <PI::Item*> icons;
	li y;
public:
	Selector ();
	Selector (vector <PI::Item*>);

	void setIcons(vector <PI::Item*>);
	void addItem (PI::Item*);

	void draw();

	void open();
	void close();

	bool isHidden();
};

#endif /* ZUTIL_H */
