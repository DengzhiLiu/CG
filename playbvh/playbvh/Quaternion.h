
#ifndef _QUATERNIONCAL_H_
#define _QUATERNIONCAL_H_

/************************************************************************************
*	Files: Quaternioncal.h
*
*	Some of the code here was derived from 
*		http://www.neutralzone.org/home/facsys/math.html
*	I acutally retyped most of it to get an understanding of what was going
*	on but the source is pretty similar
*	I would also like to say thanks to the website
*		http://www.flipcode.com/documents/matrfaq.html
*	which helped provide a lot of insight into what was going on
*	
*************************************************************************************/

#ifndef EPSILON
#define  EPSILON   0.0000001 
#endif

class Quaternion 
{

private:
public:
	float W;
	float X;
	float Y;
	float Z;



	Quaternion();

	// Create quaternion from the values w, x, y, z
	Quaternion( float w, float x, float y, float z );

	// Create quaternion from euler angles X, Y, and Z
	Quaternion( float radX, float radY, float radZ );

	inline Quaternion(const float *m)
	{
		this->FromRotationMatrix(m);
	}

	// Copy constructor
	Quaternion( const Quaternion & );
	~Quaternion();

	// These are overload so that we can just multiply quaternions together
	friend Quaternion operator* (const Quaternion&, const Quaternion&);
	const Quaternion& operator*= (const Quaternion&);
	Quaternion operator* (float fScalar) const;
	Quaternion operator+ (const Quaternion& rkQ) const;

	void print();

	void FromRotationMatrix (const float * m);
	void ToRotationMatrix (float* m) const;

	// Normalize a quaternion
	float normal2();
	void normalize();

	// Compute quaternion from axis angle representation
	//		Example (45, 1, 0, 0) ==> gives you a 45 degree rotation about X-axis
	void fromAxisAngle( float, float, float, float );

	// Compute quaternion from euler angles
	void fromEulerAngles( float, float, float );

	// This return the axis and angle to rotate around, I don't really use this however,
	// I rather just get the Rotation matrix and multiply that on my gl stack
	void getAngleAndAxis( float &angle, float &x, float &y, float &z );

	// This will return a matrix ready for use with openGL
	void getRotationMatrix( float* );

	// The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
	// where yaw is rotation about the Up vector, pitch is rotation about the
	// Right axis, and roll is rotation about the Direction axis.
	bool ToEulerAnglesZXY (float &rfYAngle, float& rfPAngle,float& rfRAngle) const;

	// This is the function that will linearly interpolate between two quaternion rotation
	// You call this with a quaternion and give it two quaternions to interpolate betwee
	// and also t which is a float between 0 and 1 which tells you where you are between
	// a and b
	void slerp(const Quaternion &QuatVectorA,const Quaternion &QuatVectorB, const float Tele);
	void Slerp(const Quaternion &QuatVectorA,const Quaternion &QuatVectorB, const float u,const float Omega);

	Quaternion conjugate();
	float dotProduct(Quaternion& quat);
	Quaternion Multiplication(Quaternion& quat);

	Quaternion Exp() const;
	//Quaternion power(const float t) const;
	Quaternion Log()const;

	/** Calculate the local roll element of this quaternion.
	@param reprojectAxis By default the method returns the 'intuitive' result
	that is, if you projected the local Y of the quaternion onto the X and
	Y axes, the angle between them is returned. If set to false though, the
	result is the actual yaw that will be used to implement the quaternion,
	which is the shortest possible path to get to the same orientation and 
	may involve less axial rotation. 
	*/
	float getRoll(bool reprojectAxis = true) const;
	/** Calculate the local pitch element of this quaternion
	@param reprojectAxis By default the method returns the 'intuitive' result
	that is, if you projected the local Z of the quaternion onto the X and
	Y axes, the angle between them is returned. If set to true though, the
	result is the actual yaw that will be used to implement the quaternion,
	which is the shortest possible path to get to the same orientation and 
	may involve less axial rotation. 
	*/
	float getPitch(bool reprojectAxis = true) const;
	/** Calculate the local yaw element of this quaternion
	@param reprojectAxis By default the method returns the 'intuitive' result
	that is, if you projected the local Z of the quaternion onto the X and
	Z axes, the angle between them is returned. If set to true though, the
	result is the actual yaw that will be used to implement the quaternion,
	which is the shortest possible path to get to the same orientation and 
	may involve less axial rotation. 
	*/
	float getYaw(bool reprojectAxis = true) const;		


	static const Quaternion Quaternion::ZERO;
	static const Quaternion Quaternion::IDENTITY;
};

#endif