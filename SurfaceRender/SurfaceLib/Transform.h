#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "Common.h"
#include "Vector3.h"
#include <math.h>

#pragma warning(disable:4172)

class Quaternion //normalized quaternion for representing rotations
{
public:
	//constructors
	Quaternion() : r(1.) { } //initialize to identity
	Quaternion(const Quaternion &q) : r(q.r), v(q.v) {} //copy constructor
	Quaternion(const ElemType &inR, const Vector3 &inV) : r(inR), v(inV) {}

	// operator
	inline Quaternion operator-() { return Quaternion(-r, -v); }
	//axis angle constructor:
	Quaternion(const Vector3 &axis, const ElemType &angle) : r(cos(angle * ElemType(0.5))), v(axis.Normalize() * sin(angle * ElemType(0.5))) {}

	// added
	Quaternion operator+(const Quaternion &q) const { return Quaternion(r + q.r, v + q.v); }
	Quaternion operator*(const ElemType &t) const { return Quaternion(r * t, v * t); }

	//quaternion multiplication
	Quaternion operator*(const Quaternion &q) const { return Quaternion(r * q.r - v * q.v, q.v * r + v * q.r + v % q.v); }

	//transforming a vector
	inline Vector3 operator*(const Vector3 &p) const
	{
		Vector3 v2 = v + v;
		// vsq2 record dot product of v and v2
		Vector3 vsq2 = Vector3(v[0]*v2[0], v[1]*v2[1],v[2]*v2[2]);
		//Vector3 vsq2 = v.apply(multiplies<Real>(), v2);
		Vector3 rv2 = v2 * r;
		Vector3 vv2(v[1] * v2[2], v[0] * v2[2], v[0] * v2[1]);
		return Vector3(p[0] * (ElemType(1.) - vsq2[1] - vsq2[2]) + p[1] * (vv2[2] - rv2[2]) + p[2] * (vv2[1] + rv2[1]),
			p[1] * (ElemType(1.) - vsq2[2] - vsq2[0]) + p[2] * (vv2[0] - rv2[0]) + p[0] * (vv2[2] + rv2[2]),
			p[2] * (ElemType(1.) - vsq2[0] - vsq2[1]) + p[0] * (vv2[1] - rv2[1]) + p[1] * (vv2[0] + rv2[0]));
	}

	//equality
	bool operator==(const Quaternion &oth) const
	{
		return (r == oth.r && v == oth.v) || (r == -oth.r && v == -oth.v);
	}

	inline void Normalize()
	{
		ElemType distance = (ElemType)sqrt(r * r + v.LengthSq());
		if( distance < EPS )	{
			r = 1;
			v = Vector3(0, 0, 0);
		}
		else {
			r /= distance;
			v = v / distance;
		}
	}

	// Matrix you get looks like this
	//      -----------
	//     |0  4  8  12|
	// M = |1  5  9  13|
	//     |2  6  10 14|
	//     |3  7  11 15|
	//      ----------- 
	inline void GetRotationMatrix( ElemType* M)
	{	
		ElemType xx, xy, xz, wx, yy, yz, wy, zz, wz;

		xx = v[0] * v[0];
		xy = v[0] * v[1];
		xz = v[0] * v[2];
		wx = r * v[0];

		yy = v[1] * v[1];
		yz = v[1] * v[2];
		wy = r * v[1];

		zz = v[2] * v[2];
		wz = r * v[2];

		M[0]  = 1 - 2 * ( yy + zz );
		M[1]  =     2 * ( xy + wz );
		M[2]  =     2 * ( xz - wy );

		M[4]  =     2 * ( xy - wz );
		M[5]  = 1 - 2 * ( xx + zz );
		M[6]  =     2 * ( yz + wx );

		M[8]  =     2 * ( xz + wy );
		M[9]  =     2 * ( yz - wx );
		M[10] = 1 - 2 * ( xx + yy );

		M[3] = M[7] = M[11] = M[12] = M[13] = M[14] = 0;
		M[15] = 1;
	}

	inline Quaternion Inverse() const { return Quaternion(-r, v); }

	ElemType GetAngle() const { return ElemType(2.) * atan2(v.Length(), r); }
	Vector3 GetAxis() const { return v.Normalize(); }

	const ElemType &operator[](int i) const { return (i == 0) ? r : v[i-1]; }
	void Set(const ElemType &inR, const Vector3 &inV) {
		ElemType ratio = ElemType(1.) / sqrt(inR * inR + inV.LengthSq()); 
		r = inR * ratio; v = inV * ratio; //normalize
	}

public:
	ElemType	r;
	Vector3		v;
};

class Transform {				//T(v) = (rot * v * scale) + trans
public:
	//typedef Vector<Real, 3> Vec;

	// construct
	Transform() : scale(1) {}
	explicit Transform(const ElemType &inScale) : scale(inScale) {}
	explicit Transform(const Vector3 &inTrans) : scale(1), trans(inTrans) { rot = Quaternion(1, Vector3(0, 0, 0)); }
	inline Transform(const Quaternion &inRot, ElemType inScale = 1, Vector3 inTrans = Vector3()) : rot(inRot), scale(inScale), trans(inTrans) {}
	inline Transform(const Transform &t) : rot(t.rot), scale(t.scale), trans(t.trans) {}

	// added
	inline Transform operator+(const Transform &t) const { return Transform(rot + t.rot,  scale + t.scale, trans + t.trans); }
	inline Transform operator*(const ElemType &t) const { return Transform(rot * t, scale * t, trans * t); }

	inline Transform operator*(const Quaternion &q) const { return Transform(rot * q, scale, trans); }
	inline Transform operator*(const Transform &t) const { return Transform(rot * t.rot, scale * t.scale, trans + rot * (t.trans * scale)); }
	Vector3 operator*(const Vector3 &v) const { return rot * (v * scale) + trans; }

	inline Vector3 Rotate(Vector3 v) { return rot * v * scale; }

	inline Transform Inverse() const { return Transform(rot.Inverse(), 1 / scale, rot.Inverse() * -trans * (1 / scale)); }

	Transform LinearComponent() const { return Transform(rot, scale); }
	Vector3 Mult3(const Vector3 &v) const { return rot * (v * scale); }

	inline ElemType GetScale() const { return scale; }
	inline Vector3 GetTrans() const { return trans; }
	inline Quaternion GetRot() const { return rot; }

	inline void GetTransformMatrix(ElemType* m)
	{
		rot.GetRotationMatrix(m);
		m[0] *= scale;	m[1] *= scale;
		m[2] *= scale;	m[3] *= scale;
		m[4] *= scale;	m[5] *= scale;
		m[6] *= scale;	m[7] *= scale;
		m[8] *= scale;	m[9] *= scale;
		m[10] *= scale; m[11] *= scale;
		m[12] = trans[0];
		m[13] = trans[1];
		m[14] = trans[2];
		m[15] = 1;
	}

private:
	Vector3		trans;
	ElemType	scale;
	Quaternion	rot;
};

#endif