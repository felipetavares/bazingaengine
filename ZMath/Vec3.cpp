#include "Vec3.h"

Vec3::Vec3 () {
	x = y = z = 0;
}

Vec3::Vec3 (const Vec3& _vec3) {
	x = _vec3.x;
	y = _vec3.y;
	z = _vec3.z;
}

Vec3::Vec3 (float _x,float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

string Vec3::_str ()  {
	stringstream stream;

	stream  << "("
			<< x
			<< " "
			<< y
			<< " "
			<< z
			<< ")";

	return stream.str();
}