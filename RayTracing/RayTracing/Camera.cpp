#include "Camera.h"
#include <gl/glut.h>

Camera::Camera( void )
{
	eye = Point(0, 0, 100);
	at = Point(0, 0, 0);
	up = Vector(0, 1, 0);
	fovy = 60;
	near = 1.0;
	far = 1000.0;
	screenWidth = 512;
	screenHeight = 512;
	LookAt();
}

Camera::Camera( const Point &eye, const Point &at, const Vector &up, double fovy, double near, double far,
			   int screenWidth, int screenHeight ) :
	eye(eye), at(at), up(up), fovy(fovy), near(near), far(far), screenWidth(screenWidth), screenHeight(screenHeight)
{
	LookAt();
}

void Camera::LookAt( void )
{
	double aspect = screenWidth / screenHeight;
	halfHeight = tan(fovy / 2 * M_PI / 180) * near;
	halfWidth = halfHeight * aspect;

	lookdir = at - eye;
	right = lookdir.Cross(up);
	up = right.Cross(lookdir);
	lookdir.Normalize();
	right.Normalize();
	up.Normalize();

	center = eye + lookdir * near;
	topLeft = center - right * halfWidth + up * halfHeight;
}

Vector Camera::GenerateRay( double x, double y )
{
	double pixelSize = halfWidth * 2.0 / screenWidth;	
	Vector direction = topLeft + right * (x * pixelSize) - up * (y * pixelSize) - eye;
	direction.Normalize();

	return direction;
}

// 左右移动时注意人与物体的相对关系
void Camera::MoveLeft( double offset )
{
	eye.d[0] -= offset;
	at.d[0] -= offset;

	center = eye + lookdir * near;
	topLeft = center - right * halfWidth + up * halfHeight;
}

void Camera::MoveRight( double offset )
{
	eye.d[0] += offset;
	at.d[0] += offset;

	center = eye + lookdir * near;
	topLeft = center - right * halfWidth + up * halfHeight;
}

void Camera::TurnRight( double angle )
{
	Vector viewdir;
	float fcos,fsin;
	viewdir = at - eye;
	fcos=cos(angle/180.0*M_PI);
	fsin=sin(angle/180.0*M_PI);
	lookdir.d[0] = Vector(fcos, 0, -fsin).Dot(viewdir);
	lookdir.d[1] = Vector(0, 1, 0).Dot(viewdir);
	lookdir.d[2] = Vector(fsin, 0, fcos).Dot(viewdir);
	at = eye + lookdir;
	
	LookAt();
}

void Camera::TurnLeft( double angle )
{
	Vector viewdir;
	float fcos,fsin;
	viewdir = at - eye;
	fcos=cos(angle/180.0*M_PI);
	fsin=sin(angle/180.0*M_PI);
	lookdir.d[0] = Vector(fcos, 0, fsin).Dot(viewdir);
	lookdir.d[1] = Vector(0, 1, 0).Dot(viewdir);
	lookdir.d[2] = Vector(-fsin, 0, fcos).Dot(viewdir);
	at = eye + lookdir;

	LookAt();
}

void Camera::MoveUp( double offset )
{
	eye.d[1] += offset;
	at.d[1] += offset;

	center = eye + lookdir * near;
	topLeft = center - right * halfWidth + up * halfHeight;
}

void Camera::MoveDown( double offset )
{
	eye.d[1] -= offset;
	at.d[1] -= offset;

	center = eye + lookdir * near;
	topLeft = center - right * halfWidth + up * halfHeight;
}

void Camera::ZoomIn( double offset )
{
	eye.d[2] -= offset;
	at.d[2] -= offset;

	center = eye + lookdir * near;
	topLeft = center - right * halfWidth + up * halfHeight;
}

void Camera::ZoomOut( double offset )
{
	eye.d[2] += offset;
	at.d[2] += offset;

	center = eye + lookdir * near;
	topLeft = center - right * halfWidth + up * halfHeight;
}