#include "ZGraphics.h"

ZGraphic::ZGraphic (bool _visible) {
	visible = _visible;

	position = new Vec3();
	rotation = new Vec3();
	size     = new Vec3(1,1,1);

	texture	 = NULL;
	animation = NULL;
}

ZGraphic::~ZGraphic () {
	delete position;
	delete rotation;
	delete size;
}