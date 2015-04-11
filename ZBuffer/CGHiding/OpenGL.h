#pragma once

#ifndef _OPENGL_H_
#define _OPENGL_H_

#include "Vector3.h"
#include "ObjInfo.h"
#include <math.h>
#include <gl\glut.h>
#include <gl\glaux.h>

#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

class COpenGL
{
// Attributes
public:
	CDC   *m_pDC;
	HGLRC m_hRC;
	float m_deltaz;
	float m_object_scale;
	CFVector3 m_object_center;
	
// Operations
public:
	COpenGL(void);
	~COpenGL(void);

	BOOL InitGL();
	void FocusObject();
	BOOL InitializeOpenGL(CDC *pDC);
	BOOL SetupPixelFormat();
	void Set3DProjection(int w, int h);
	void ZoomObject(double delta);
	void RotateObject(double x, double y);
	void SetObjInfo(CFVector3 object_center, float scale);
	void Finish();
};

extern float g_IWLight;
extern float g_IALight;
extern float g_RotateX;
extern float g_RotationSpeed;
extern COpenGL	g_OpenGL;
extern CFVector3 g_LightPos;

#endif

