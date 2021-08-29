/*#include "Vector3.h"


Vector3::Vector3() {
	x = y = z = 0;
}
Vector3::Vector3(bool value) {
	x = y = z = value;
}
Vector3::Vector3(int value) {
	x = y = z = value;
}
Vector3::Vector3(float X, float Y, float Z) {
	x = X;
	y = Y;
	z = Z;
}

Vector3 Vector3::operator+ (Vector3 const& obj) {
	return { x + obj.x, y + obj.y, z + obj.z };
}

Vector3 Vector3::operator- (Vector3 const& obj) {
	return { x - obj.x, y - obj.y, z - obj.z };
}

Vector3 Vector3::operator/ (float num) {
	return { x / num, y / num, z / num };
}

Vector3 Vector3::operator* (float num) {
	return { x * num, y * num, z * num };
}

bool Vector3::operator==(Vector3 vec) {
	return (vec.x == x && vec.y == y && vec.z == z);
}

std::ostream& Vector3::operator<< (std::ostream& output) {
	output << "X: " << x << " Y: " << y << " Z: " << z;
	return output;
}
// @param Vector to calculate dot product with
float Vector3::DotProduct(Vector3 vec) {
	return x * vec.x + y * vec.y + z * vec.z;
}

float Vector3::Magnitude() {
	return (float)sqrtf(x * x + y * y + z * z);
}
//@param Vector to calculate distance to
float Vector3::Distance(Vector3 vec) {
	return (*this - vec).Magnitude();
}

Vector3 Vector3::Normalize() {
	return *this / Magnitude();
}
/*
 Moves a certain percent between to points
 @param destination to move to
 @param percent to move towards the destination
 @return location of partial movement towards destination
*/
/*Vector3 Vector3::Lerp(Vector3 vec, float percent) {
	Vector3 Dist = (*this - vec);
	if (Dist.y < -180) {
		Dist.y += 360;
	}
	else if (Dist.y > 180) {
		Dist.y -= 360;
	}
	Dist = Dist * percent;
	return Dist;
}*/
