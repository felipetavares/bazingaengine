/*
  Copyright (C) 2013 by Felipe Tavares

  This file is part of Render TRI.

  Render TRI is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Render TRI is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Render TRI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RI_VEC2_H
#define RI_VEC2_H

#include <string>
#include <sstream>

/* Nomeclatures like GLSL */
class vec2 {
 public:
  float x,y;

  // You can acess x,y using [0] and [1], too
  // -1 -> maps to 1
  // -2 -> maps to 0
  // 2  -> maps to 0
  float operator [] (const int);

  vec2 operator = (const vec2&);
  vec2 operator * (const float);
  vec2 operator / (const float);
  vec2 operator + (const vec2&);
  vec2 operator - (const vec2&);

  float operator * (const vec2&);

  vec2 operator /= (const float);
  vec2 operator *= (const float);
  vec2 operator += (const vec2&);
  vec2 operator -= (const vec2&);

  float dot (const vec2&);

  float length ();
  vec2 normal ();
  vec2 normalize ();

  std::string str ();

  // Constructors!
  vec2 ();
  vec2 (float,float);
  vec2 (const vec2&);

  // Destructor
  ~vec2 ();
};

#endif /* RI_VEC2_H */
