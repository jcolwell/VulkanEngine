//======================================================================================
// Filename: Matrix.inl
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Vector3.h"
#include <cmath>

namespace Math
{

//======================================================================================
// Function Definitions
//======================================================================================

inline Matrix Matrix::Zero()
{
	return Matrix
	(  
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::Identity()
{
	return Matrix();
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::Translation(f32 x, f32 y, f32 z)
{
	return Matrix
	(  
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::Translation(const Vector3& v)
{
	return Matrix
	(
		1.0f, 0.0f, 0.0f, v.x,
		0.0f, 1.0f, 0.0f, v.y,
		0.0f, 0.0f, 1.0f, v.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::RotationX(f32 rad)
{
	f32 cosX = cos(rad);
	f32 sinX = sin(rad);

	return Matrix
	(	
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosX, -sinX, 0.0f,
		0.0f, sinX, cosX, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f	
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::RotationY(f32 rad)
{
	f32 cosX = cos(rad);
	f32 sinX = sin(rad);
	return Matrix
	(	
		cosX, 0.0f, sinX, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinX, 0.0f, cosX, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::RotationZ(f32 rad)
{
	f32 cosX = cos(rad);
	f32 sinX = sin(rad);
	return Matrix
	(	
		cosX, -sinX, 0.0f, 0.0f,
		sinX, cosX, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

//-------------------------------------------------------------------------------------

//inline Matrix Matrix::RotationAxis(const Vector3& axis, f32 rad)
//{
//		return Matrix();
//}

//-------------------------------------------------------------------------------------

//inline Matrix Matrix::RotationQuaternion(const Quaternion& q)
//{
//	return Matrix();
//}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::Scaling(f32 s)
{
	return Matrix
	(
		s, 0.0f, 0.0f, 0.0f,
		0.0f, s, 0.0f, 0.0f,
		0.0f, 0.0f, s, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::Scaling(f32 sx, f32 sy, f32 sz)
{
	return Matrix
	(
		sx, 0.0f, 0.0f, 0.0f,
		0.0f, sy, 0.0f, 0.0f,
		0.0f, 0.0f, sz, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::Scaling(const Vector3& s)
{
	return Matrix
	(
		s.x, 0.0f, 0.0f, 0.0f,
		0.0f, s.y, 0.0f, 0.0f,
		0.0f, 0.0f, s.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::operator-() const
{
	return *this * -1.0f;
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::operator+(const Matrix& rhs) const
{
	return Matrix
	(
		_11 + rhs._11, _21 + rhs._21, _31 + rhs._31, _41 + rhs._41,
		_12 + rhs._12, _22 + rhs._22, _32 + rhs._32, _42 + rhs._42,
		_13 + rhs._13, _23 + rhs._23, _33 + rhs._33, _43 + rhs._43,
		_14 + rhs._14, _24 + rhs._24, _34 + rhs._34, _44 + rhs._44
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::operator-(const Matrix& rhs) const
{
	return Matrix
	(
		_11 - rhs._11, _21 - rhs._21, _31 - rhs._31, _41 - rhs._41,
		_12 - rhs._12, _22 - rhs._22, _32 - rhs._32, _42 - rhs._42,
		_13 - rhs._13, _23 - rhs._23, _33 - rhs._33, _43 - rhs._43,
		_14 - rhs._14, _24 - rhs._24, _34 - rhs._34, _44 - rhs._44
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::operator*(const Matrix& rhs) const
{
	return Matrix
	(
		(_11 * rhs._11 + _21 * rhs._12 + _31 * rhs._13 + _41 * rhs._14), //_11
		(_11 * rhs._21 + _21 * rhs._22 + _31 * rhs._23 + _41 * rhs._24), //_21		
		(_11 * rhs._31 + _21 * rhs._32 + _31 * rhs._33 + _41 * rhs._34), //_31	
		(_11 * rhs._41 + _21 * rhs._42 + _31 * rhs._43 + _41 * rhs._44), //_41
		
		(_12 * rhs._11 + _22 * rhs._12 + _32 * rhs._13 + _42 * rhs._14), //_12
		(_12 * rhs._22 + _22 * rhs._22 + _32 * rhs._23 + _42 * rhs._24), //_22
		(_12 * rhs._31 + _22 * rhs._32 + _32 * rhs._33 + _42 * rhs._34), //_32
		(_12 * rhs._41 + _22 * rhs._42 + _32 * rhs._43 + _42 * rhs._44), //_42
		
		(_13 * rhs._11 + _23 * rhs._12 + _33 * rhs._13 + _43 * rhs._14), //_13
		(_13 * rhs._21 + _23 * rhs._22 + _33 * rhs._23 + _43 * rhs._24), //_23
		(_13 * rhs._31 + _23 * rhs._32 + _33 * rhs._33 + _43 * rhs._34), //_33
		(_13 * rhs._41 + _23 * rhs._42 + _33 * rhs._43 + _43 * rhs._44), //_43
		
		(_14 * rhs._11 + _24 * rhs._12 + _34 * rhs._13 + _44 * rhs._14), //_14
		(_14 * rhs._21 + _24 * rhs._22 + _34 * rhs._23 + _44 * rhs._24), //_24
		(_14 * rhs._31 + _24 * rhs._32 + _34 * rhs._33 + _44 * rhs._34), //_34
		(_14 * rhs._41 + _24 * rhs._42 + _34 * rhs._43 + _44 * rhs._44)	 //_44
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::operator*(f32 s) const
{
	return Matrix
	(
		_11 * s, _21 * s , _31 * s, _41 * s ,
		_12 * s, _22 * s , _32 * s, _42 * s ,
		_13 * s, _23 * s , _33 * s, _43 * s ,
		_14 * s, _24 * s , _34 * s, _44 * s 
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::operator/(f32 s) const
{
	assert(s != 0 && "[Math] Cannot divide by zero!");
	f32 inv = 1.0f / s;

	return Matrix
	(
		_11 * inv, _21 * inv, _31 * inv, _41 * inv,
		_12 * inv, _22 * inv, _32 * inv, _42 * inv,
		_13 * inv, _23 * inv, _33 * inv, _43 * inv,
		_14 * inv, _24 * inv, _34 * inv, _44 * inv
	);
}

//-------------------------------------------------------------------------------------

inline Matrix Matrix::operator+=(const Matrix& rhs)
{
	_11 += rhs._11; _21 += rhs._21; _31 += rhs._31; _41 += rhs._41;
	_12 += rhs._12; _22 += rhs._22; _32 += rhs._32; _42 += rhs._42;
	_13 += rhs._13; _23 += rhs._23; _33 += rhs._33; _43 += rhs._43;
	_14 += rhs._14; _24 += rhs._24; _34 += rhs._34; _44 += rhs._44;
	return *this;
}

//-------------------------------------------------------------------------------------

} // namespace Math
//======================================================================================
