#ifndef ZGRAPHICS_H
#define ZGRAPHICS_H

#include "../ZMath/Vec3.h"
#include "../ZAssetsManager/ZAssetsManager.h"
#include "../ZAnimation/ZAnimation.h"

// A ZGraphic is a bitmap that is positioned
// relatively from the respective ZObject

class ZGraphic {
public:
	// Texture for this graphic
	ZTextureAsset* texture;
	ZAnimation *animation;

	Vec3 *position;
	Vec3 *size;
	Vec3 *rotation;

	// Is this object visible now?
	bool visible;

	ZGraphic (bool=true);
	~ZGraphic ();
};

#endif /* ZGRAPHICS_H */
