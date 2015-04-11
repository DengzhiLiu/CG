/**
* $Id: SceneGL.h
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

#ifndef _SCENEGL_H
#define _SCENEGL_H

#include "view.h"
#include "GLDraw.h"
#include "trackball.h"
#include "SceneObject.h"
#include "../Common/transform.h"

#include <time.h>
#pragma once

/***************************************************

  3D Interface(translate, rotation, scale, etc.)

**************************************************/


class InterfaceManager;

extern clock_t startTime;

class CSceneGL
{
public:
	enum TransMode { Translation, Rotation, Selecting };
	enum ProjectMode { OrthoPro, PerspecPro };

// Operations
public:
	CSceneGL(void);
	~CSceneGL(void);

	void InitGL();
	void SetView();
	BOOL SetupPixelFormat();
	BOOL InitializeDevice(const HDC&);

	void SetTexture();
	void SetSkelSphere();
	void SetCamera(View::ViewMode );
	void SetTransMode(TransMode );
	void SetProjMode(ProjectMode );
	void SetDrawMode(GLW::DrawMode );
	void SetShadeMode(GLW::ShadeMode );
	void SetInterface(InterfaceManager *);
	void SetDrawMesh();
	void SetDrawSkel();
	void SetDrawFloor();

	Vector3 GetTransPoint(Vector3 p);
	void ResetCoord();
	void MouseWheelEvent(float);
	void MouseMoveEvent(Vector3 , Vector3 );

	void PaintGL();
	void DrawFloor();
	void DrawBackground();
	void DisplayTimer();
	
	// Attributes
public:
	HDC		m_DC;
	HGLRC	m_hRC;
	View*	m_pcam;
	//Trackball	m_Trackball;
	
	bool	m_bDrawSkel;
	bool	m_bDrawMesh;
	bool	m_bDrawFloor;

	Transform<> m_Transform;
private:
	bool			m_bTexture;
	bool			m_SkelSphere;
	TransMode		m_TransMode;
	ProjectMode		m_ProjMode;
	GLW::DrawMode	m_DrawMode;
	GLW::ShadeMode	m_ShadeMode;
	SceneObject		m_SceneObject;
	InterfaceManager *m_pInterface;

	/* information for creating and displaying a frame counter */
	int		m_frame;
	int		m_clockPtr;
	clock_t start, end;
};

#endif // _SCENEGL_H	
