/**
* $Id: view.h
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

#ifndef _VIEW_H
#define _VIEW_H

#include "../Common/vector.h"

class View {
public:
	enum ViewMode { ViewFront, ViewBack, ViewLeft, ViewRight };

public:
	View();
	~View() {};
	View(int w, int h, float fov, float near, float far, Vector3 eye, Vector3 at, Vector3 up);

	void	SetView(int w, int h);
	int		GetWidth() { return m_width; }
	int		GetHeight() { return m_height; }

	void	SetFov(float infov);
	float	GetFov() { return m_fovy; }

	void	SetNear(float znear);
	float   GetNear() { return m_near; }

	void	SetFar(float zfar);
	float	GetFar() { return m_far; }

	void	SetEye(Vector3 inEye);
	Vector3 GetEye() { return m_eye; }

	void	SetCenter(Vector3 inAt);
	Vector3 GetCenter() { return m_at; }

	void	SetUp(Vector3 inUp);
	Vector3 GetUp() { return m_up; }

	void	SetViewMode(ViewMode mode);
	ViewMode GetViewMode() { return m_viewMode; }

private:
	int		m_width, m_height;
	Vector3 m_eye, m_at, m_up;
	float	m_fovy, m_near, m_far;
	ViewMode m_viewMode;
};


#endif // _VIEW_H
