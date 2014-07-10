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

#include "vec3.h"
#include <cmath>

vec3::vec3 () {
  x = 0;
  y = 0;
  z = 0;
}

vec3::vec3 (float x, float y, float z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

vec3::vec3 (const vec3& vec) {
  x = vec.x;
  y = vec.y;
  z = vec.z;
}

vec3::vec3 (const vec2& vec, float z) {
  x = vec.x;
  y = vec.y;

  this->z = z;
}

vec3::~vec3 () {
  // Nothing to do here
}

vec2 vec3::tovec2 () {
  vec2 r (x,y);

  return r;
}

float vec3::operator [] (const int i) {
  switch (i) {
  case 0:
    return x;
  case 1:
    return y;
  case 2:
    return z;
  default:
    return (*this)[std::abs(i%3)]; // This allows a lot more flexibility in the input number
  }
}

std::string vec3::str () {
  std::stringstream ss;

  ss << "(";
  ss << x;
  ss << ", ";
  ss << y;
  ss << ", ";
  ss << z;
  ss << ")";

  return ss.str();
}

float vec3::dot (const vec3& vec) {
  return x*vec.x+y*vec.y+z*vec.z;
}

vec3 vec3::operator * (const vec3& vec) {
  vec3 r;

  r.x = y*vec.z - z*vec.y;
  r.y = z*vec.x - x*vec.z;
  r.z = x*vec.y - y*vec.x;

  return r;
}

vec3 vec3::operator *= (const vec3& vec) {
  vec3 r;

  r.x = y*vec.z - z*vec.y;
  r.y = z*vec.x - x*vec.z;
  r.z = x*vec.y - y*vec.x;

  x = r.x;
  y = r.y;
  z = r.z;

  return r;
}

vec3 vec3::operator = (const vec3& vec) {
  x = vec.x;
  y = vec.y;
  z = vec.z;

  return *this;
}

vec3 vec3::operator * (const float s) {
  vec3 r;

  r.x = x*s;
  r.y = y*s;
  r.z = z*s;

  return r;
}

vec3 vec3::operator / (const float s) {
  vec3 r;

  r.x = x/s;
  r.y = y/s;
  r.z = z/s;

  return r;
}

vec3 vec3::operator + (const vec3& vec) {
  vec3 r;

  r.x = x+vec.x;
  r.y = y+vec.y;
  r.z = z+vec.z;

  return r;
}

vec3 vec3::operator - (const vec3& vec) {
  vec3 r;

  r.x = x-vec.x;
  r.y = y-vec.y;
  r.z = z-vec.z;

  return r;
}

vec3 vec3::operator *= (const float s) {
  x *= s;
  y *= s;
  z *= s;

  return *this;
}

vec3 vec3::operator /= (const float s) {
  x /= s;
  y /= s;
  z /= s;

  return *this;
}

vec3 vec3::operator += (const vec3& vec) {
  x += vec.x;
  y += vec.y;
  z += vec.z;

  return *this;
}

vec3 vec3::operator -= (const vec3& vec) {
  x -= vec.x;
  y -= vec.y;
  z -= vec.z;

  return *this;
}

float vec3::length () {
  return sqrt (x*x+y*y+z*z);
}

vec3 vec3::normal () {
  vec3 r;
  float l = length();

  r.x = x/l;
  r.y = y/l;
  r.z = z/l;

  return r;
}

vec3 vec3::normalize () {
  float l = length();

  x /= l;
  y /= l;
  z /= l;

  return *this;
}
