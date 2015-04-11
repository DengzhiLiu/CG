/************************************************************************************
*	Files: Quaternioncal.cpp
*
*	Some of the code here was derived from 
*		http://www.neutralzone.org/home/facsys/math.html
*		I acutally retyped most of it to get an understanding of what was going
*		on but the source is pretty similar
*	I would also like to say thanks to the website
*		http://www.flipcode.com/documents/matrfaq.html
*	which helped provide a lot of insight into what was going on
*	
*************************************************************************************/

#include <math.h>
#include <stdio.h>

#include "Quaternion.h"

#ifndef PI
#define PI (3.1415926535)
#endif

#define FLT_EPSILON 0.005

Quaternion::Quaternion()
{
	W = 1;
	X = Y = Z = 0;
}

Quaternion::Quaternion( float w, float x, float y, float z )
{
	W = w;
	X = x;
	Y = y;
	Z = z;
}

Quaternion::Quaternion( float radX, float radY, float radZ)
{
	fromEulerAngles( radX, radY, radZ );
}

Quaternion::Quaternion( const Quaternion &tempQuaternion )
{
	W = tempQuaternion.W;
	X = tempQuaternion.X;
	Y = tempQuaternion.Y;
	Z = tempQuaternion.Z;
}

Quaternion::~Quaternion()
{
}


Quaternion Quaternion::operator* (float fScalar) const
{
	return Quaternion(fScalar*W,fScalar*X,fScalar*Y,fScalar*Z);
}
Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
{
	return Quaternion(W+rkQ.W,X+rkQ.X,Y+rkQ.Y,Z+rkQ.Z);
}
void Quaternion::slerp(const Quaternion &QuatVectorA,const Quaternion &QuatVectorB, const float Tele)
{
	float Omega, CosOmega, SinOmega, StandardCaseLinearQuatA, StandardCaseLinearQuatB;

	// Calculate cosine
	CosOmega = QuatVectorA.X*QuatVectorB.X + QuatVectorA.Y*QuatVectorB.Y + QuatVectorA.Z*QuatVectorB.Z + QuatVectorA.W*QuatVectorB.W;

	if ((1.0f+CosOmega) > FLT_EPSILON)
	{
		if ((1.0f-CosOmega) > FLT_EPSILON)
		{
			// standard case SLERP
			Omega = (float)acos(CosOmega);
			SinOmega = (float)sin(Omega);
			StandardCaseLinearQuatA = (float)sin((1.0f-Tele)*Omega) / SinOmega;
			StandardCaseLinearQuatB = (float)sin(Tele*Omega) / SinOmega;
		}
		else
		{
			// a and b are very close so we can do a linear interpolation
			StandardCaseLinearQuatA = 1.0f - Tele;
			StandardCaseLinearQuatB = Tele;
		}
		X = StandardCaseLinearQuatA*QuatVectorA.X + StandardCaseLinearQuatB*QuatVectorB.X;
		Y = StandardCaseLinearQuatA*QuatVectorA.Y + StandardCaseLinearQuatB*QuatVectorB.Y;
		Z = StandardCaseLinearQuatA*QuatVectorA.Z + StandardCaseLinearQuatB*QuatVectorB.Z;
		W = StandardCaseLinearQuatA*QuatVectorA.W + StandardCaseLinearQuatB*QuatVectorB.W;
	}
	else
	{
		X =-QuatVectorB.Y;
		Y = QuatVectorB.X;
		Z =-QuatVectorB.W;
		W = QuatVectorB.Z;
		StandardCaseLinearQuatA = (float)sin((1.0f-Tele) * PI * 0.5);
		StandardCaseLinearQuatB = (float)sin(Tele * PI * 0.5);

		X = StandardCaseLinearQuatA*QuatVectorA.X + StandardCaseLinearQuatB*QuatVectorB.X;
		Y = StandardCaseLinearQuatA*QuatVectorA.Y + StandardCaseLinearQuatB*QuatVectorB.Y;
		Z = StandardCaseLinearQuatA*QuatVectorA.Z + StandardCaseLinearQuatA*QuatVectorB.Z;
		W = StandardCaseLinearQuatA*QuatVectorA.W + StandardCaseLinearQuatA*QuatVectorB.W;
	}
}

void Quaternion::Slerp(const Quaternion &QuatVectorA,const Quaternion &QuatVectorB, const float u,const float Omega)
{
	Quaternion temp;

	temp=QuatVectorA*(sin((1-u)*Omega*PI/180)/sin(Omega*PI/180)) + QuatVectorB*(sin(Omega*u*PI/180)/sin(Omega*PI/180));
	*this = temp;
	normalize();
}
void Quaternion::print()
{
	printf("Quaternion \tW=%f \tX=%f \tY=%f \tZ=%f\n",W,X,Y,Z);
}

Quaternion operator*(const Quaternion &QuatVectorA, const Quaternion &QuatVectorB)
{
	Quaternion tempQuaternion;

	tempQuaternion.W = QuatVectorA.W*QuatVectorB.W - (QuatVectorA.X*QuatVectorB.X + QuatVectorA.Y*QuatVectorB.Y + QuatVectorA.Z*QuatVectorB.Z);
	tempQuaternion.X = QuatVectorA.W*QuatVectorB.X + QuatVectorB.W*QuatVectorA.X + QuatVectorA.Y*QuatVectorB.Z - QuatVectorA.Z*QuatVectorB.Y;
	tempQuaternion.Y = QuatVectorA.W*QuatVectorB.Y + QuatVectorB.W*QuatVectorA.Y + QuatVectorA.Z*QuatVectorB.X - QuatVectorA.X*QuatVectorB.Z;
	tempQuaternion.Z = QuatVectorA.W*QuatVectorB.Z + QuatVectorB.W*QuatVectorA.Z + QuatVectorA.X*QuatVectorB.Y - QuatVectorA.Y*QuatVectorB.X;

	return tempQuaternion;
}

const Quaternion& Quaternion::operator*=(const Quaternion &tempQuaternion)
{
	float w = W*tempQuaternion.W - (X*tempQuaternion.X + Y*tempQuaternion.Y + Z*tempQuaternion.Z);

	float x = W*tempQuaternion.X + tempQuaternion.W*X + Y*tempQuaternion.Z - Z*tempQuaternion.Y;
	float y = W*tempQuaternion.Y + tempQuaternion.W*Y + Z*tempQuaternion.X - X*tempQuaternion.Z;
	float z = W*tempQuaternion.Z + tempQuaternion.W*Z + X*tempQuaternion.Y - Y*tempQuaternion.X;

	W = w;
	X = x;
	Y = y;
	Z = z;

	return *this;
}
float Quaternion::normal2()
{
	float distance = (float)sqrt(W*W + X*X + Y*Y + Z*Z);
	return distance;
}
void Quaternion::normalize()
{
	float distance = (float)sqrt(W*W + X*X + Y*Y + Z*Z);
	if( distance == 0 )
	{
		W = 1.0;
		X = Y = Z = 0.0;
	}
	else
	{
		W /= distance;
		X /= distance;
		Y /= distance;
		Z /= distance;
	}
}

void Quaternion::fromAxisAngle( float angle, float x, float y, float z )
{
	float SinAngle = (float)sin( angle / 2 );
	float CosAngle = (float)cos( angle / 2);

	W = CosAngle;
	X = x * SinAngle;
	Y = y * SinAngle;
	Z = z * SinAngle;

	normalize();
}

void Quaternion::fromEulerAngles( float radX, float radY, float radZ )
{

	Quaternion QuaternionX, QuaternionY, QuaternionZ, QuaternionResult;

	QuaternionX.fromAxisAngle( radX, 1, 0, 0 );
	QuaternionY.fromAxisAngle( radY, 0, 1, 0 );
	QuaternionZ.fromAxisAngle( radZ, 0, 0, 1 );

	QuaternionResult = QuaternionZ * QuaternionX * QuaternionY;

	W = QuaternionResult.W;
	X = QuaternionResult.X;
	Y = QuaternionResult.Y;
	Z = QuaternionResult.Z;
}

void Quaternion::getAngleAndAxis( float &angle, float &x, float &y, float &z )
{
	angle = (float)(acos( W ) * 2 * 180 / PI);
	float SinAngle = (float)sqrt( 1.0 - W * W );

	if( fabs(SinAngle) < FLT_EPSILON )
		SinAngle = 1;

	x = X / SinAngle;
	y = Y / SinAngle;
	z = Z / SinAngle;
}
//      -----------
//     |0  4  8  12|
// M = |1  5  9  13|
//     |2  6  10 14|
//     |3  7  11 15|
//      ----------- 
void Quaternion::FromRotationMatrix (const float * m)// m is 4*4 matrix
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".


	float fTrace = m[0]+m[5]+m[10];
	float fRoot;

	if ( fTrace > 0.0 )
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrt(fTrace + 1.0);  // 2w
		W = 0.5*fRoot;
		fRoot = 0.5/fRoot;  // 1/(4w)
		X = (m[6]-m[9])*fRoot;
		Y = (m[8]-m[2])*fRoot;
		Z = (m[1]-m[4])*fRoot;
	}
	else
	{
		// |w| <= 1/2
		static size_t s_iNext[3] = { 1, 2, 0 };
		size_t i = 0;
		if ( m[5] > m[0] )
			i = 1;
		if (m[10] > m[4*i+i] )
			i = 2;
		size_t j = s_iNext[i];
		size_t k = s_iNext[j];

		fRoot = sqrt(m[4*i+i]-m[4*j+j]-m[4*k+k] + 1.0);
		float* apkQuat[3] = { &X, &Y, &Z };
		*apkQuat[i] = 0.5*fRoot;
		fRoot = 0.5/fRoot;
		W = ( m[4*k+j]-m[4*j+k])*fRoot;
		*apkQuat[j] = ( m[4*j+i]+m[4*i+j])*fRoot;
		*apkQuat[k] = ( m[4*k+i]+m[4*i+k])*fRoot;
	}
}
//      -----------
//     |0  4  8  12|
// M = |1  5  9  13|
//     |2  6  10 14|
//     |3  7  11 15|
//      ----------- 
void Quaternion::ToRotationMatrix ( float* m) const
{
	float fTx  = 2.0*X;
	float fTy  = 2.0*Y;
	float fTz  = 2.0*Z;
	float fTwx = fTx*W;
	float fTwy = fTy*W;
	float fTwz = fTz*W;
	float fTxx = fTx*X;
	float fTxy = fTy*X;
	float fTxz = fTz*X;
	float fTyy = fTy*Y;
	float fTyz = fTz*Y;
	float fTzz = fTz*Z;

	m[0] = 1.0-(fTyy+fTzz);
	m[4] = fTxy-fTwz;
	m[8] = fTxz+fTwy;
	m[1] = fTxy+fTwz;
	m[5] = 1.0-(fTxx+fTzz);
	m[9] = fTyz-fTwx;
	m[2] = fTxz-fTwy;
	m[6] = fTyz+fTwx;
	m[10] = 1.0-(fTxx+fTyy);

	m[3] = m[7] = m[11] = m[12]= m[13] = m[14]= 0; 
	m[15]= 1;
}
// Matrix you get looks like this
//      -----------
//     |0  4  8  12|
// M = |1  5  9  13|
//     |2  6  10 14|
//     |3  7  11 15|
//      ----------- 
void Quaternion::getRotationMatrix( float* M)
{	
	float xx, xy, xz, wx, yy, yz, wy, zz, wz;

	xx = X * X;
	xy = X * Y;
	xz = X * Z;
	wx = W * X;

	yy = Y * Y;
	yz = Y * Z;
	wy = W * Y;

	zz = Z * Z;
	wz = W * Z;

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
Quaternion Quaternion::conjugate()
{
	Quaternion result;
	result.W = W;
	result.X = -X;
	result.Y = -Y;
	result.Z = -Z;
	return result;
}
float Quaternion::dotProduct(Quaternion& quat)
{
	float result;
	result =W*quat.W + X*quat.X + Y*quat.Y + Z*quat.Z;
	return result;

}
Quaternion Quaternion::Multiplication(Quaternion& quat)//this multiply quat
{
	Quaternion result;
	result.W = W*quat.W - X*quat.X - Y*quat.Y - Z*quat.Z;
	result.X = X*quat.W + W*quat.X + Y*quat.Z - Z*quat.Y;
	result.Y = Y*quat.W + W*quat.Y + Z*quat.X - X*quat.Z;
	result.Z = Z*quat.W + W*quat.Z + X*quat.Y - Y*quat.X;

	return result;	
}

Quaternion Quaternion::Exp() const
{
	Quaternion quat;
	float theta = sqrt(X*X+Y*Y+Z*Z), 
		sin_theta = sin(theta);

	quat.W = cos(theta);
	if ( fabs(sin_theta) > EPSILON)
	{
		quat.X = X/sin_theta * theta;
		quat.Y = Y/sin_theta * theta;
		quat.Z = Z/sin_theta * theta;
	}	
	else
	{
		quat.X = X;
		quat.Y = Y;
		quat.Z = Z;
	}	

	return quat;
}
// 
//Quaternion Quaternion::power(const float t) const
//{
//	Quaternion q = (Log()*t).Exp();
//		return q;
//}

Quaternion Quaternion::Log()const
{
	Quaternion quat;
	quat.W = 0;
	float theta = acos(W),
		sin_theta = sin(theta);
	
	if ( fabs(sin_theta) > EPSILON)
	{
		quat.X = X/sin_theta * theta;
		quat.Y = Y/sin_theta * theta;
		quat.Z = Z/sin_theta * theta;
	}	
	else
	{
		quat.X = X;
		quat.Y = Y;
		quat.Z = Z;
	}	
	
	return quat;
}

// Y X Z
bool Quaternion::ToEulerAnglesZXY (float &rfYAngle, float& rfXAngle,float& rfZAngle) const
{
	// Matrix3(Quaternion)
	float m[3][3];
	float fTx  = 2.0*X;
	float fTy  = 2.0*Y;
	float fTz  = 2.0*Z;
	float fTwx = fTx*W;
	float fTwy = fTy*W;
	float fTwz = fTz*W;
	float fTxx = fTx*X;
	float fTxy = fTy*X;
	float fTxz = fTz*X;
	float fTyy = fTy*Y;
	float fTyz = fTz*Y;
	float fTzz = fTz*Z;

	m[0][0] = 1.0-(fTyy+fTzz);
	m[0][1] = fTxy-fTwz;
	m[0][2] = fTxz+fTwy;
	m[1][0] = fTxy+fTwz;
	m[1][1] = 1.0-(fTxx+fTzz);
	m[1][2] = fTyz-fTwx;
	m[2][0] = fTxz-fTwy;
	m[2][1] = fTyz+fTwx;
	m[2][2] = 1.0-(fTxx+fTyy);

	/*void Matrix3::Orthonormalize ()*/
	
		// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
		// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
		//
		//   q0 = m0/|m0|
		//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
		//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
		//
		// where |V| indicates length of vector V and A*B indicates dot
		// product of vectors A and B.

		// compute q0
		float fInvLength =1.0/sqrt(m[0][0]*m[0][0] + m[1][0]*m[1][0] + m[2][0]*m[2][0]); //Math::InvSqrt

		m[0][0] *= fInvLength;
		m[1][0] *= fInvLength;
		m[2][0] *= fInvLength;

		// compute q1
		float fDot0 = m[0][0]*m[0][1] +	m[1][0]*m[1][1] + m[2][0]*m[2][1];

		m[0][1] -= fDot0*m[0][0];
		m[1][1] -= fDot0*m[1][0];
		m[2][1] -= fDot0*m[2][0];

		fInvLength =1.0/sqrt(m[0][1]*m[0][1] + m[1][1]*m[1][1] + m[2][1]*m[2][1]); // Math::InvSqrt

		m[0][1] *= fInvLength;
		m[1][1] *= fInvLength;
		m[2][1] *= fInvLength;

		// compute q2
		float fDot1 = m[0][1]*m[0][2] +	m[1][1]*m[1][2] + m[2][1]*m[2][2];

		fDot0 =	m[0][0]*m[0][2] + m[1][0]*m[1][2] +	m[2][0]*m[2][2];

		m[0][2] -= fDot0*m[0][0] + fDot1*m[0][1];
		m[1][2] -= fDot0*m[1][0] + fDot1*m[1][1];
		m[2][2] -= fDot0*m[2][0] + fDot1*m[2][1];

		fInvLength = 1.0/sqrt(m[0][2]*m[0][2] +	m[1][2]*m[1][2] + m[2][2]*m[2][2]);  //Math::InvSqrt

		m[0][2] *= fInvLength;
		m[1][2] *= fInvLength;
		m[2][2] *= fInvLength;

	
		//bool Matrix3::ToEulerAnglesXYZ (Radian& rfYAngle, Radian& rfPAngle,Radian& rfRAngle) const

		// The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
		// where yaw is rotation about the Up vector, pitch is rotation about the
		// Right axis, and roll is rotation about the Direction axis.

		// rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
		//        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
		//       -cx*sy           sx              cx*cy

	//rfPAngle = asin(m[2][1]);
	//if ( rfPAngle < float(PI/2) )
	//{
	//	if ( rfPAngle > float(-PI/2) )
	//	{
	//		rfYAngle = atan2(-m[0][1],m[1][1]);
	//		rfRAngle = atan2(-m[2][0],m[2][2]);
	//		return true;
	//	}
	//	else
	//	{
	//		// WARNING.  Not a unique solution.
	//		float fRmY = atan2(m[0][2],m[0][0]);
	//		rfRAngle = 0.0;  // any angle works
	//		rfYAngle = rfRAngle - fRmY;
	//		return false;
	//	}
	//}
	//else
	//{
	//	// WARNING.  Not a unique solution.
	//	float fRpY = atan2(m[0][2],m[0][0]);
	//	rfRAngle = 0.0;  // any angle works
	//	rfYAngle = fRpY - rfRAngle;
	//	return false;
	//}
}
float Quaternion::getRoll(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		// roll = atan2(localx.y, localx.x)
		// pick parts of xAxis() implementation that we need
		float fTx  = 2.0*X;
		float fTy  = 2.0*Y;
		float fTz  = 2.0*Z;
		float fTwz = fTz*W;
		float fTxy = fTy*X;
		float fTyy = fTy*Y;
		float fTzz = fTz*Z;

		// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

		return float(atan2((double)(fTxy+fTwz),1.0-(fTyy+fTzz)));

	}
	else
	{
		return float(atan2(2*(X*Y + W*Z), W*W + X*X - Y*Y - Z*Z));
	}
}
//-----------------------------------------------------------------------
float Quaternion::getPitch(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		// pitch = atan2(localy.z, localy.y)
		// pick parts of yAxis() implementation that we need
		float fTx  = 2.0*X;
		float fTy  = 2.0*Y;
		float fTz  = 2.0*Z;
		float fTwx = fTx*W;
		float fTxx = fTx*X;
		float fTyz = fTz*Y;
		float fTzz = fTz*Z;

		// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
		return float(atan2((double)(fTyz+fTwx), 1.0-(fTxx+fTzz)));
	}
	else
	{
		// internal version
		return float(atan2(2*(Y*Z + W*X), W*W - X*X - Y*Y + Z*Z));
	}
}
//-----------------------------------------------------------------------
float Quaternion::getYaw(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		// yaw = atan2(localz.x, localz.z)
		// pick parts of zAxis() implementation that we need
		float fTx  = 2.0*X;
		float fTy  = 2.0*Y;
		float fTz  = 2.0*Z;
		float fTwy = fTy*W;
		float fTxx = fTx*X;
		float fTxz = fTz*X;
		float fTyy = fTy*Y;

		// Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

		return float(atan2((double)(fTxz+fTwy), 1.0-(fTxx+fTyy)));

	}
	else
	{
		// internal version
		return float(asin(-2*(X*Z - W*Y)));
	}
}

const Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
const Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);
