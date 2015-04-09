#ifndef _CAMERA_H
#define _CAMERA_H

#include "Vector.h"
#include "Point.h"
#include "Ray.h"
#include "Definitions.h"

#include <iostream>
#include <math.h>

class Camera {
public:
	Point eye, at, center, topLeft;						// 视点、目标点、中心点、视图左上角坐标
	Vector lookdir, up, right;							// 观察方向
	double near, far, fovy;
	double halfWidth, halfHeight;
	int screenWidth, screenHeight;

public:

	/* sets up a camera */
	Camera( void );
	Camera( const Point &eye, const Point &at, const Vector &up, double fovy, double near, double far,
			  int screenWidth, int screenHeight );

	/* generate a ray based on previous set camera parameters */
	void LookAt( void );
	Vector GenerateRay( double x, double y );

	/* move function */
	void MoveLeft( double offset );
	void MoveRight( double offset );
	void TurnRight( double angle );
	void TurnLeft( double angle );
	void MoveUp( double offset );
	void MoveDown( double offset );
	void ZoomIn( double offset );
	void ZoomOut( double offset );

	/* accessor functions */
	inline Point GetEye( void ) const { return eye; }
	inline Point GetAt( void ) const { return at; }
	inline Vector GetUp( void ) const { return up; }
	inline double GetFovy( void ) const { return fovy; }
	inline double GetNear( void ) const { return near; }
	inline double GetFar( void ) const { return far; }
	inline int GetScreenWidth( void ) const { return screenWidth; }
	inline int GetScreenHeight( void ) const { return screenHeight; }
};


#endif
