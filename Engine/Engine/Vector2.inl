//======================================================================================
// Filename: Vector2.inl
// Description:
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
namespace Math
{

//======================================================================================
// Function Definitions
//======================================================================================
inline Vector2 Vector2::Zero()
{
	return Vector2(0.0f, 0.0f);
}

//--------------------------------------------------------------------------------------

inline Vector2 Vector2::One()
{
	return Vector2(1.0f, 1.0f);
}

//--------------------------------------------------------------------------------------

inline Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

//--------------------------------------------------------------------------------------

inline Vector2 Vector2::operator+(const Vector2& rhs) const
{
	return Vector2(x + rhs.x, y + rhs.y);
}

//--------------------------------------------------------------------------------------

inline Vector2 Vector2::operator-(const Vector2& rhs) const
{
	return Vector2(x - rhs.x, y - rhs.y);
}

//--------------------------------------------------------------------------------------

inline Vector2 Vector2::operator*(f32 s) const
{
	return Vector2(x * s, y * s);
}

//--------------------------------------------------------------------------------------

inline Vector2 Vector2::operator/(f32 s) const
{
	assert(s != 0.0f && "[Math] Cannot divide by zero!");
	const f32 inv = 1 / s;

	return Vector2(x * inv, y * inv);
}

//--------------------------------------------------------------------------------------

inline Vector2& Vector2::operator+=(const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

//--------------------------------------------------------------------------------------

inline Vector2& Vector2::operator-=(const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

//--------------------------------------------------------------------------------------

inline Vector2& Vector2::operator*=(f32 s)
{
	x *= s;
	y *= s;
	return *this;
}

//--------------------------------------------------------------------------------------

inline Vector2& Vector2::operator/=(f32 s)
{
	assert(s != 0 && "[Math] Cannot divide by zero!");
	f32 inv = 1 / s;
	x *= inv;
	y *= inv;
	return *this;
}

//--------------------------------------------------------------------------------------

inline bool Vector2::operator== (const Vector2& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

//--------------------------------------------------------------------------------------

inline bool Vector2::operator!= (const Vector2& rhs) const
{
	return !(x == rhs.x && y == rhs.y);
}

//--------------------------------------------------------------------------------------

} //Namespace Math
