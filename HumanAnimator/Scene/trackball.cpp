#include "stdafx.h"
#include "trackball.h"

#include <math.h>
#include <gl/glew.h>
#include <gl/glu.h>

static GLdouble modelview[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

Trackball::Trackball()
{
	radius = 1.;
	scale = 1.;
}

Trackball::~Trackball()
{
}

void Trackball::Reset()
{
	scale = 1.;
	rot = Quaternion<>();
}

void Trackball::Apply()
{
	GLfloat M[16];
	rot.getRotationMatrix(M);
	glTranslatef(trans[0], trans[1], trans[2]);
	glScalef(scale, scale, scale);
	glMultMatrixf(M);
}

void Trackball::ApplyTranslate()
{
	glTranslatef(trans[0], trans[1], trans[2]);
}

void Trackball::ApplyScale()
{
	glScalef(scale, scale, scale);
}

void Trackball::ApplyRot()
{
	GLfloat M[16];
	rot.getRotationMatrix(M);
	//glTranslatef(center[0], center[1], center[2]);
	glMultMatrixf(M);
	//glTranslatef(-center[0], -center[1], -center[2]);
}

void Trackball::Scale(float notch)
{
	scale *= pow(1.2f, notch);
}

void Trackball::Trans(Vector3 v)
{
	trans += v;
}

void Trackball::SetZoom(Vector3 eye)
{
	ab_eye = eye;				 // store eye vector
	ab_zoom2 = ab_eye * ab_eye;
	ab_zoom = sqrt(ab_zoom2);	// store eye distance
	ab_sphere = radius;				// sphere radius
	ab_sphere2 = ab_sphere * ab_sphere;
	ab_eyedir = ab_eye * (1.0 / ab_zoom);	// normalize distance to eye
	ab_edge = ab_sphere2 / ab_zoom;			// plane of visible edge

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
}

void Trackball::Rot(Vector3 lastPos, Vector3 curPos)
{
	Vector3 ab_start = SphereCoords(lastPos);
	Vector3 ab_curr = SphereCoords(curPos);
	if(ab_curr == ab_start) {
		// avoid potential rare divide by tiny
		return;
	}

	// use a dot product to get the angle between them
	// use a cross product to get the vector to rotate around
	//GLfloat cos2a = ab_start*ab_curr;
	//GLfloat sina = sqrt((1.0 - cos2a)*0.5);
	//GLfloat cosa = sqrt((1.0 + cos2a)*0.5);
	//Vector3 cross = (ab_start % ab_curr).normalize() * sina;
	//Quaternion<> cur = Quaternion<>(cosa, cross);

	Quaternion<> cur = Quaternion<>(ab_start % ab_curr, acos(ab_start*ab_curr));
	rot = cur * rot;
	//transform = Transform<>(cur) * transform;
}

// find the intersection with the plane through the visible edge
Vector3 Trackball::EdgeCoords(Vector3 m)
{
	// find the intersection of the edge plane and the ray
	float t = (ab_edge - ab_zoom) / (ab_eyedir * m);
	Vector3 a = ab_eye + (m*t);
	// find the direction of the eye-axis from that point
	// along the edge plane
	Vector3 c = (ab_eyedir * ab_edge) - a;

	// find the intersection of the sphere with the ray going from
	// the plane outside the sphere toward the eye-axis.
	float ac = (a*c);
	float c2 = (c*c);

	float q;
	if (ab_eye[2] < 0)
		q = ( 0.0 - ac - sqrt( ac*ac - c2*((a*a)-ab_sphere2) ) ) / c2;
	else
		q = ( ac - sqrt( ac*ac - c2*((a*a)-ab_sphere2) ) ) / c2;

	return (a+(c*q)).normalize();
}

// find the intersection with the sphere
Vector3 Trackball::SphereCoords(Vector3 win)
{
	GLdouble ax,ay,az;

	gluUnProject((GLdouble)win[0], (GLdouble)win[1], (GLdouble)win[2],modelview,projection,viewport,&ax,&ay,&az);
	Vector3 m = Vector3((float)ax,(float)ay,(float)az) - ab_eye;

	// mouse position represents ray: eye + t*m
	// intersecting with a sphere centered at the origin
	GLfloat a = m*m;
	GLfloat b = (ab_eye*m);
	GLfloat root = (b*b) - a*(ab_zoom2 - ab_sphere2);
	if(root <= 0) return EdgeCoords(m);

	GLfloat t;
	if (ab_eye[2] < 0)
		t = (0.0 - b - sqrt(root)) / a;
	else
		t = (b - sqrt(root)) / a;
	return (ab_eye+(m*t)).normalize();
}