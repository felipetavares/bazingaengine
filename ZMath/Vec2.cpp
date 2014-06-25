#include "Vec2.h"

Vec2::Vec2 () {
	x = y = 0;
}

Vec2::Vec2 (const Vec2& _vec2) {
	x = _vec2.x;
	y = _vec2.y;
}

Vec2::Vec2 (float _x,float _y) {
	x = _x;
	y = _y;
}

string Vec2::_str ()  {
	stringstream stream;

	stream  << "("
			<< x
			<< " "
			<< y
			<< ")";

	return stream.str();
}