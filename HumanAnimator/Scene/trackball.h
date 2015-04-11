/**
* $Id: trackball.h
*
*
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef _TRACKBALL_H
#define _TRACKBALL_H

#include "../Common/vector.h"
#include "../Common/transform.h"
#include <gl/glew.h>

class Trackball
{
public:
	Trackball();
	~Trackball();

public:
	void Apply();
	void ApplyRot();
	void ApplyScale();
	void ApplyTranslate();

	void Scale(float notch);
	void Trans(Vector3 v);
	void Rot(Vector3, Vector3);

	void	Reset();
	void	SetZoom(Vector3);
	Vector3 EdgeCoords(Vector3);
	Vector3 SphereCoords(Vector3);

public:
	float		radius;
	Vector3		center;	

	float		scale;
	Vector3		trans;
	Quaternion<> rot;
	int 		viewport[4];
	double		projection[16];

	// the distance from the origin to the eye
	GLfloat ab_zoom;
	GLfloat ab_zoom2;
	// the radius of the arcball
	GLfloat ab_sphere;
	GLfloat ab_sphere2;
	// the distance from the origin of the plane that intersects
	// the edge of the visible sphere (tangent to a ray from the eye)
	GLfloat ab_edge;

	Vector3 ab_eye;
	Vector3 ab_eyedir;
	Vector3 ab_up;
};

#endif
