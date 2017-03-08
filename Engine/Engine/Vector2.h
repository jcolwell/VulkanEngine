#ifndef ENGINE_VECTOR2_H__
#define	ENGINE_VECTOR2_H__
//======================================================================================
// Filename: Vector2.h
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Common.h"

namespace Math
{
//======================================================================================
// Struct
//======================================================================================

struct Vector2
{
	f32 x;
	f32 y;

	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(f32 x, f32 y) : x(x), y(y) {}

	static Vector2 Zero();
	static Vector2 One();

	Vector2 operator-() const;
	Vector2 operator+(const Vector2& rhs) const;
	Vector2 operator-(const Vector2& rhs) const;
	Vector2 operator*(f32 s) const;
	Vector2 operator/(f32 s) const;

	Vector2& operator+=(const Vector2& rhs);
	Vector2& operator-=(const Vector2& rhs);
	Vector2& operator*=(f32 s);
	Vector2& operator/=(f32 s);

	bool operator== (const Vector2& rhs) const;
	bool operator!= (const Vector2& rhs) const;
};

} //Namespace Math

//======================================================================================
// Inline
//======================================================================================
#include "Vector2.inl"

//======================================================================================
#endif //!ENGINE_Vector2_H__