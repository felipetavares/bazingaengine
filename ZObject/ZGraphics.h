#ifndef ZGRAPHICS_H
#define ZGRAPHICS_H

#include "ZMath/math/vec3/vec3.h"
#include "../ZAssetsManager/ZAssetsManager.h"
#include "../ZAnimation/ZAnimation.h"

// A ZGraphic is a bitmap that is positioned
// relatively from the respective ZObject

class ZGraphic {
public:
	// Texture for this graphic
	ZTextureAsset* texture;
	ZAnimation *animation;

	vec3 *position;
	vec3 *size;
	vec3 *rotation;

	// Is this object visible now?
	bool visible;

	ZGraphic (bool=true);
	~ZGraphic ();
};

#endif /* ZGRAPHICS_H */
