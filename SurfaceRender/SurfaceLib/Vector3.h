#ifndef _VEC3_H
#define _VEC3_H

#include "Common.h"
#include <math.h>

class Vector3
{
public:
	typedef Vector3 Self;

	Vector3()
	{
		m[0] = m[1] = m[2] = 0;
	}

	Vector3(ElemType a, ElemType b, ElemType c)
	{
		m[0] = a;
		m[1] = b;
		m[2] = c;
	}

	Vector3(ElemType *A)
	{
		m[0] = A[0];
		m[1] = A[1];
		m[2] = A[2];
	}

	Vector3(const Vector3& other)
	{
		m[0] = other[0];
		m[1] = other[1];
		m[2] = other[2];
	}

	ElemType* operator & ()
	{
		return m;
	}

	const ElemType operator[] (int _i) const
	{
		return m[_i]; 
	}

	ElemType &operator[] (int _i)
	{
		return m[_i];
	}

	inline Self operator = (const Vector3 &other)
	{
		m[0] = other[0];
		m[1] = other[1];
		m[2] = other[2];
		return *this;
	}

	inline Self operator + (const Vector3 &other) const
	{
		Vector3 v;
		v[0] = m[0] + other[0];
		v[1] = m[1] + other[1];
		v[2] = m[2] + other[2];

		return v;
	}

	inline Self operator - () const
	{
		Vector3 v;
		v[0] = -m[0];
		v[1] = -m[1];
		v[2] = -m[2];

		return v;
	}

	inline Self operator - (const Vector3 &other) const
	{
		Vector3 v;
		v[0] = m[0] - other[0];
		v[1] = m[1] - other[1];
		v[2] = m[2] - other[2];

		return v;
	}

	inline ElemType operator * (const Vector3 &other) const
	{
		ElemType r;
		r = m[0] * other[0] + m[1] * other[1] + m[2] * other[2];

		return r;
	}

	inline Self operator % (const Vector3 &other) const
	{
		Vector3 v;
		v[0] = m[1] * other[2] - m[2] * other[1];
		v[1] = m[2] * other[0] - m[0] * other[2];
		v[2] = m[0] * other[1] - m[1] * other[0];
		
		return v;
	}

	inline Self operator * (const ElemType scalar) const
	{
		Vector3 v;
		v[0] = m[0] * scalar;
		v[1] = m[1] * scalar;
		v[2] = m[2] * scalar;

		return v;
	}

	inline Self operator / (const ElemType scalar) const
	{
		if (abs(scalar) < EPS)
			return *this;
		
		Vector3 v;
		v[0] = m[0] / scalar;
		v[1] = m[1] / scalar;
		v[2] = m[2] / scalar;
		return v;
	}

	inline bool operator == (const Vector3& other) const
	{
		return ((m[0] == other[0]) && (m[1] == other[1]) && (m[2] == other[2]));
	}

	ElemType LengthSq() const
	{
		return (m[0]*m[0] + m[1]*m[1] + m[2]*m[2]);
	}

	ElemType Length() const
	{
		return sqrt(LengthSq());
	}

	inline Self Normalize() const
	{
		ElemType length = Length();

		if (abs(length) < EPS)
		{
			return Vector3(0, 0, 0);
		}
		else
			return (*this) / length;
	}

private:
	ElemType	m[3];
};

#endif