#ifndef ENGINE_VECTOR3_H__
#define	ENGINE_VECTOR3_H__

//======================================================================================
// Filename: Vector3.h
// Description:
//
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Common.h"

namespace Math
{

//======================================================================================
// Vector3 Struct
//======================================================================================

struct Vector3
{
	f32 x;
	f32 y;
	f32 z;

	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

	static Vector3 Zero();
	static Vector3 One();
	static Vector3 XAxis();
	static Vector3 YAxis();
	static Vector3 ZAxis();

	Vector3 operator-() const;
	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator*(f32 s) const;
	Vector3 operator/(f32 s) const;

	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(f32 s);
	Vector3& operator/=(f32 s);

	bool operator== (const Vector3& rhs) const;
	bool operator!= (const Vector3& rhs) const;
};

} // namespace Math

#include "Vector3.inl"

//======================================================================================
#endif //!ENGINE_Vector3_H__