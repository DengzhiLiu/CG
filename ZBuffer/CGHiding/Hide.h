#pragma once

#ifndef _HIDE_H_
#define _HIDE_H_

#define OPENGL_RENDER  0
#define PROGRAM_RENDER 1

#include "OpenGL.h"
#include "ObjInfo.h"
#include "ScanZBuffer.h"
#include <vector>
#include <math.h>
using namespace std;

class CHide
{
	// Attributes
public:
	int light_type;
	int color_type;

	//CScanZBuffer	m_scaner;
public:
	CHide(void);
	~CHide(void);

	void	Initial(t3DObject object);
	void	SortFace(t3DObject object);
	void	OpenGL_Render(t3DModel *p3DModel);
	void	Draw(int light, int color, int render, t3DModel *p3DModel);
};

extern	CHide	g_Hide;

#endif