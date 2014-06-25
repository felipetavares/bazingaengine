#ifndef VEC2_H
#define VEC2_H

#include <iostream>
#include <sstream>
using namespace std;

class Vec2 {
public:
	float x,y;

	Vec2 ();
	Vec2 (const Vec2&);
	Vec2 (float,float);

	string _str ();
};

#endif /* VEC2_H */