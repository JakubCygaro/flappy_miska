#include "Vector2.h"


Vector2 Vector2::operator+(const Vector2& other) {
	return { this->x + other.x, this->y + other.y };
}

Vector2 Vector2::operator-(const Vector2& other) {
	return { this->x - other.x, this->y - other.y };
}

Vector2 Vector2::operator*(const float scalar) {
	return { this->x * scalar, this->y * scalar };
}

Vector2 Vector2::operator/(const float scalar) {
	return { this->x / scalar, this->y / scalar };
}

Vector2& Vector2::operator+=(const Vector2& rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}