
#pragma once
#ifndef VECTOR3_H
#define VECTOR3_H
#include <iostream>

class Vector3 {
public:
	Vector3();
	Vector3(bool);
	Vector3(int);
	Vector3(float X, float Y, float Z);
	Vector3 operator+(Vector3 const& obj);
	Vector3 operator-(Vector3 const& obj);
	Vector3 operator/(float num);
	Vector3 operator*(float num);
	bool operator==(Vector3 vec);
	float DotProduct(Vector3 vec);
	float Magnitude();
	float Distance(Vector3 vec);
	Vector3 Normalize();
	Vector3 Lerp(Vector3 vec, float percent);
	std::ostream& operator<<(std::ostream&);
	float x, y, z;

};
#endif
