#pragma once
struct Vector2 {
	float x, y;
	Vector2 operator + (const Vector2& other);
	Vector2 operator - (const Vector2& other);
	Vector2 operator * (const float scalar);
	Vector2 operator / (const float scalar);
	Vector2& operator += (const Vector2& rhs);
	Vector2& operator -= (const Vector2& rhs);
};
