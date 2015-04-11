/**
* $Id: transform.h
*
* This file implemented a mathlib of quaternion and transform,  overload most of 
* the operations and implement functions needed
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vector.h"
#include "utildefines.h"

template<class Real = float>
class Quaternion //normalized quaternion for representing rotations
{
public:
	//constructors
	Quaternion() : r(1.) { } //initialize to identity
	Quaternion(const Quaternion &q) : r(q.r), v(q.v) {} //copy constructor
	Quaternion(const Real &inR, const Vector<Real, 3> &inV) : r(inR), v(inV) {}
	template<class Real> Quaternion(const Quaternion<Real> &q) : r(q.r), v(q.v) {} //convert quaternions of other types
	inline Quaternion operator-() { return Quaternion(-r, -v); }
	//axis angle constructor:
	template<class Real> Quaternion(const Vector<Real, 3> &axis, const Real &angle) : r(cos(angle * Real(0.5))), v(sin(angle * Real(0.5)) * axis.normalize()) {}

	//quaternion multiplication
	Quaternion operator*(const Quaternion &q) const { return Quaternion(r * q.r - v * q.v, r * q.v + q.r * v + v % q.v); }

	//transforming a vector
	inline Vector<Real, 3> operator*(const Vector<Real, 3> &p) const
	{
		Vector<Real, 3> v2 = v + v;
		// vsq2 record dot product of v and v2
		Vector<Real, 3> vsq2 = v.apply(multiplies<Real>(), v2);
		Vector<Real, 3> rv2 = r * v2;
		Vector<Real, 3> vv2(v[1] * v2[2], v[0] * v2[2], v[0] * v2[1]);
		return Vector<Real, 3>(p[0] * (Real(1.) - vsq2[1] - vsq2[2]) + p[1] * (vv2[2] - rv2[2]) + p[2] * (vv2[1] + rv2[1]),
			p[1] * (Real(1.) - vsq2[2] - vsq2[0]) + p[2] * (vv2[0] - rv2[0]) + p[0] * (vv2[2] + rv2[2]),
			p[2] * (Real(1.) - vsq2[0] - vsq2[1]) + p[0] * (vv2[1] - rv2[1]) + p[1] * (vv2[0] + rv2[0]));
	}

	//equality
	template<class R> bool operator==(const Quaternion<R> &oth) const
	{
		return (r == oth.r && v == oth.v) || (r == -oth.r && v == -oth.v);
	}

	inline void normalize()
	{
		float distance = (float)sqrt(r * r + v.lengthsq());
		if( distance < 1e-6 )	{
			r = 1.0;
			v = Vector3(0.0, 0.0, 0.0);
		}
		else {
			r /= distance;
			v = v / distance;
		}
	}

	Quaternion slerp(Quaternion q, Real t)
	{
		float k0, k1;
		float cosw = v * q.v + r * q.r;

		if (cosw < EPSILON)	{
			q = -q;
			cosw = -cosw;
		}

		if (fabs(1 - cosw) < EPSILON)	{
			k0 = 1 - t;
			k1 = t;
		}
		else {
			float sinw = sqrt(1 - cosw * cosw);
			float omega = atan2(sinw, cosw);
			float oneOverSinOmega = 1 / sinw;
			k0 = sin((1-t) * omega) * oneOverSinOmega;
			k1 = sin(t * omega) * oneOverSinOmega;
		}

		return Quaternion(k0 * r + k1 * q.r, k0 * v + k1 * q.v);
	}

	Quaternion slerp(const Quaternion q1, const Quaternion q2, Real t)
	{
		Quaternion sq1 = this->slerp(q1, t);
		Quaternion sq2 = q1.slerp(q2, t);

		return sq1.slerp(sq2, t);
	}

	// Compute the rotation angle
	//-----------------------------------------------------------------------
	float Quaternion::getPitch(bool reprojectAxis) const			// angle rotate about x axis
	{
		if (reprojectAxis)
		{
			// pitch = atan2(localy.z, localy.y)
			// pick parts of yAxis() implementation that we need
			Vector3 tv = v * 2.0;
			float	fTwx = tv[0] * r;
			float	fTxx = tv[0] * v[0];
			float	fTyz = tv[2] * v[1];
			float	fTzz = tv[2] * v[2];

			// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
			return float(atan2((double)(fTyz+fTwx), 1.0-(fTxx+fTzz)));
		}
		else
		{
			// internal version
			return float(atan2(2*(v[1]*v[2] + r*v[0]), r*r - v[0]*v[0] - v[1]*v[1] + v[2]*v[2]));
		}
	}
	//-----------------------------------------------------------------------
	float Quaternion::getYaw(bool reprojectAxis) const			// angle rotate about y axis
	{
		if (reprojectAxis)
		{
			// yaw = atan2(localz.x, localz.z)
			// pick parts of zAxis() implementation that we need
			Vector3 tv = v * 2.0;
			float	fTwy = tv[1] * r;
			float	fTxx = tv[0] * v[0];
			float	fTxz = tv[2] * v[0];
			float	fTyy = tv[1] * v[1];

			// Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

			return float(atan2((double)(fTxz+fTwy), 1.0-(fTxx+fTyy)));

		}
		else
		{
			// internal version
			return float(asin(-2*(v[0]*v[2] - r*v[1])));
		}
	}

	float getRoll(bool reprojectAxis) const				// angle rotate about z axis
	{
		if (reprojectAxis)
		{
			// roll = atan2(localx.y, localx.x)
			// pick parts of xAxis() implementation that we need
			Vector3 tv = v * 2.0;
			float	fTwz = tv[2] * r;
			float	fTxy = tv[1] * v[0];
			float	fTyy = tv[1] * v[1];
			float	fTzz = tv[2] * v[2];

			// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

			return float(atan2((double)(fTxy+fTwz),1.0-(fTyy+fTzz)));

		}
		else
		{
			return float(atan2(2*(v[0]*v[1] + r*v[2]), r*r + v[0]*v[0] - v[1]*v[1] - v[2]*v[2]));
		}
	}

	// Matrix you get looks like this
	//      -----------
	//     |0  4  8  12|
	// M = |1  5  9  13|
	//     |2  6  10 14|
	//     |3  7  11 15|
	//      ----------- 
	inline void getRotationMatrix( float* M)
	{	
		float xx, xy, xz, wx, yy, yz, wy, zz, wz;

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

	inline Quaternion inverse() const { return Quaternion(-r, v); }

	Real getAngle() const { return Real(2.) * atan2(v.length(), r); }
	Vector<Real, 3> getAxis() const { return v.normalize(); }

	const Real &operator[](int i) const { return (i == 0) ? r : v[i - 1]; }
	void set(const Real &inR, const Vector<Real, 3> &inV) {
		Real ratio = Real(1.) / sqrt(inR * inR + inV.lengthsq()); 
		r = inR * ratio; v = inV * ratio; //normalize
	}

public:

	Real r;
	Vector<Real, 3> v;
};

template<class Real = float> class Transform { //T(v) = (rot * v * scale) + trans
public:
	typedef Vector<Real, 3> Vec;

	Transform() : scale(1.) {}
	explicit Transform(const Real &inScale) : scale(inScale) {}
	explicit Transform(const Vec &inTrans) : scale(1.), trans(inTrans) { rot = Quaternion<>(1., Vector3(0, 0, 0)); }
	inline Transform(const Quaternion<Real> &inRot, Real inScale = Real(1.), Vec inTrans = Vec()) : rot(inRot), scale(inScale), trans(inTrans) {}
	inline Transform(const Transform &t) : rot(t.rot), scale(t.scale), trans(t.trans) {}

	Transform operator*(const Transform &t) const { return Transform(rot * t.rot, scale * t.scale, trans + rot * (scale * t.trans)); }
	Vec operator*(const Vec &v) const { return rot * (v * scale) + trans; }

	Transform inverse() const { return Transform(rot.inverse(), 1 / scale, rot.inverse() * -trans * (1 / scale)); }

	Transform linearComponent() const { return Transform(rot, scale); }
	Vec mult3(const Vec &v) const { return rot * (v * scale); }

	inline Real getScale() const { return scale; }
	inline Vec getTrans() const { return trans; }
	inline Quaternion<Real> getRot() const { return rot; }

	Transform slerp(Transform transf, Real t)
	{
		Quaternion<Real> sr = rot.slerp(transf.getRot(), t);
		Vec st = (trans + transf.getTrans()) * t;

		return Transform(sr, 1., st);
	}

	inline void getTransformMatrix(float* m)
	{
		rot.getRotationMatrix(m);
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
	Quaternion<Real> rot;
	Real scale;
	Vec trans;
};

#endif
