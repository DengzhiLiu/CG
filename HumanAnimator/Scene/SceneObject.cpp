#include "stdafx.h"
#include "SceneObject.h"
#include "../Common/vector.h"
#include "../Common/utildefines.h"

#include <math.h>
#include <gl/glu.h>
#include <gl/glut.h>

void SceneObject::Cylinder(int slices,float lenght,float width)
{

	Vector3 p0;
	Vector3 p1;

	glScaled(lenght,width,width);

	float step=6.28f/(float)slices;
	float angle=0;

	// render side of cylinder
	glBegin(GL_TRIANGLE_STRIP);
	for(int b = 0; b <= slices; ++b)
	{
		if(b == slices) angle = 0.0;
		p0 = Vector3( 0, sin(angle), cos(angle));
		p1 = p0; p1[0] = 1.f;
		glNormal3f(p0[0],p0[1],p0[2]);
		glVertex3d(p0[0],p0[1],p0[2]);
		glVertex3d(p1[0],p1[1],p1[2]);
		angle+=step;
	}
	glEnd();

	// render bottom of cylinder
	angle=0;
	p0 = Vector3(0,0,0);

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(-1,0,0);
	glVertex3d(p0[0],p0[1],p0[2]);
	for(int b = 0; b <= slices-1; ++b)
	{
		glNormal3f(-1,0,0);
		p1 = Vector3( 0, sin(angle),cos(angle));
		glVertex3d(p1[0],p1[1],p1[2]);
		angle+=step;
	}
	glNormal3f(-1,0,0);
	glVertex3d(0,0,1);
	glEnd();

	// render top of cylinder
	angle=6.28;
	p0 = Vector3(1,0,0);

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1,0,0);
	glVertex3d(p0[0],p0[1],p0[2]);
	for(int b = 0; b <= slices-1; ++b)
	{
		glNormal3f(1,0,0);
		p1 = Vector3( 1, sin(angle),cos(angle));
		glVertex3d(p1[0],p1[1],p1[2]);
		angle-=step; //CCW
	}
	glNormal3f(1,0,0);
	glVertex3d(1,0,1);
	glEnd();

}	

void SceneObject::Cone(int slices,float lenght,float width)
{				
	Vector3 p0;			//center
	Vector3 P[2]; 
	Vector3 N[2];

	glScaled(lenght,width,width); //scale

	//render the side
	p0 = Vector3(1.0,0,0);
	N[0]= Vector3( 1.f,sinf(0),cosf(0) );
	P[0]= Vector3( 0,sinf(0),cosf(0));

	for(int b = 1; b<=slices; ++b)
	{
		float angle = -6.28f*(float)b/(float)slices;//CCW
		if (b==slices) angle=0;

		N[1] = Vector3(1.f, sinf(angle), cosf(angle));
		P[1] = Vector3(0,sinf(angle), cosf(angle));

		glBegin(GL_TRIANGLES);
		Vector3 n =  ( (P[0]-p0) % (P[1]-p0) ).normalize();
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(p0[0],p0[1],p0[2]);
		glNormal3f(N[0][0],N[0][1],N[0][2]);
		glVertex3f(P[0][0],P[0][1],P[0][2]);
		glNormal3f(N[1][0],N[1][1],N[1][2]);
		glVertex3f(P[1][0],P[1][1],P[1][2]);
		glEnd();

		N[0] = N[1];
		P[0] = P[1];
	}

	// render the bottom
	p0 = Vector3(0,0,0);
	P[0]= Vector3( 0,sinf(0),cosf(0));

	for(int b = 1; b<=slices; ++b)
	{
		float angle = -6.28f*(float)b/(float)slices;
		if (b==slices) angle=0;

		Vector3 n= Vector3( -1.f,0,0 );
		P[1] = Vector3(0, sinf(angle), cosf(angle));

		glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(P[0][0],P[0][1],P[0][2]);
		glVertex3f(p0[0],p0[1],p0[2]);
		glVertex3f(P[1][0],P[1][1],P[1][2]);
		glEnd();

		P[0] = P[1];
	}
}	

void SceneObject::DrawCoordinate()
{
	GLdouble orig[3];
	int viewport[4];
	double modelview[16], projection[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	int width = viewport[2], height = viewport[3];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glFrustum(viewport[0], viewport[2], viewport[1], viewport[3], 0.01, 1000.);
	if (width > height)
		glOrtho(-width / (double)height, width / (double)height, -1., 1., 0.01, 1000);
	else
		glOrtho(-1., 1., -height / (double)width, height / (double)width, 0.01, 1000);

	glDepthMask(GL_FALSE);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	gluUnProject(50, 50, 0.0005, modelview, projection, viewport, &orig[0], &orig[1], &orig[2]);

	glPushMatrix();
	glTranslated(orig[0], orig[1], orig[2]);
	glScaled(.1, .1, .1);
	glPushAttrib (GL_TRANSFORM_BIT |GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glEnable(GL_COLOR_MATERIAL);

	glColor4f(1.0,0.0,1.0,1.0); //purple sphere
	glutSolidSphere(0.3,30,30);

	glColor4f(1.0,0.0,0.0,1.0); //x red
	glPushMatrix();
	glPushMatrix();
	Cylinder(50,1,0.1);
	glPopMatrix();
	glTranslatef(1,0,0);
	Cone(50,0.5,0.3);
	glPopMatrix();

	glColor4f(0.0,1.0,0.0,1.0); //y green
	glPushMatrix();
	glRotatef(90,0,0,1);
	glPushMatrix();
	Cylinder(50,1,0.1);
	glPopMatrix();
	glTranslatef(1,0,0);
	Cone(50,0.5,0.3);
	glPopMatrix();

	glColor4f(0.0,0.0,1.0,1.0); //z blue
	glPushMatrix();
	glRotatef(90,0,-1,0);
	glPushMatrix();
	Cylinder(50,1,0.1);
	glPopMatrix();
	glTranslatef(1,0,0);
	Cone(50,0.5,0.3);
	glPopMatrix();

	glPopAttrib();
	glPopMatrix();

	glDepthMask(GL_TRUE);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void SceneObject::DrawCircle ()
{
	int nside = DH.CircleStep;
	const double pi2 = PI * 2.0;
	glBegin (GL_LINE_LOOP);
	for (double i = 0; i < nside; i++) {
		glNormal3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
		glVertex3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
	}
	glEnd ();
}

void SceneObject::DrawSphereIcon (bool active)
{  
	glPushAttrib (GL_TRANSFORM_BIT |GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix ();

	GLfloat amb[4] = { .3f, .3f, .3f, 1.0f };
	GLfloat col[4] = { .5f, .5f, .8f, 1.0f };
	glEnable (GL_LINE_SMOOTH);
	if (active)
		glLineWidth (DH.LineWidthMoving);
	else
		glLineWidth (DH.LineWidthStill);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4fv (DH.color);
	glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, amb);
	glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);

	DrawCircle ();
	glRotatef (90, 1, 0, 0);
	DrawCircle ();
	glRotatef (90, 0, 1, 0);
	DrawCircle ();

	glPopMatrix ();
	glPopAttrib ();
}