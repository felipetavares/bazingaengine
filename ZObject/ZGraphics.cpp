#include "ZGraphics.h"

ZGraphic::ZGraphic (bool _visible) {
	visible = _visible;

	position = new vec3();
	rotation = new vec3();
	size     = new vec3(1,1,1);

	texture	 = NULL;
	animation = NULL;
}

ZGraphic::~ZGraphic () {
	delete position;
	delete rotation;
	delete size;
}
