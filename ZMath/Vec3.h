#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <sstream>
using namespace std;

class Vec3 {
public:
	float x,y,z;

	Vec3 ();
	Vec3 (const Vec3&);
	Vec3 (float,float,float);

        Vec3 operator+ (Vec3);

	string _str ();
};

#endif /* VEC3_H */
