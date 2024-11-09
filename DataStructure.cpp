#include"DataStructure.h"

Vec2 Vec2::operator+(const Vec2& other) {
	return Vec2(x + other.x, y + other.y);
}
Vec2 Vec2::operator-(const Vec2& other) {
	return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(const Vec2& other) {
	return Vec2(x * other.x, y * other.y);
}

Vec2 Vec2::operator*(float &other)
{
	return Vec2(x * other, y * other);
}

Vec2 Vec2::operator/(const Vec2& other) {
	if (other.x != 0 && other.y != 0) return Vec2(x / other.x, y / other.y);
	else throw std::runtime_error("Divide by zero");
}

float Vec2::magnitude() const{
	return sqrt(x * x + y * y);
}

float Vec2::length2() const
{
	return x * x + y * y;
}

void Vec2::normalize()
{
	float mag = magnitude();
	if (mag != 0) {
		x /= mag;
		y /= mag;
	}
	else throw std::runtime_error("Divide by zero");
}

void Vec2::clear()
{
	x = 0;
	y = 0;
}