#ifndef _VIEW_H
#define _VIEW_H

/**************************************************************************/
/*				Binary file Defines the Parameter of the Camera			  */
/**************************************************************************/

#pragma once
#include "Vector3.h"

class View
{
public:
	View();
	~View();
	View(int _w, int _h, float _fov, float _near, float _far, \
			Vector3 _eye, Vector3 _at, Vector3 _up);

	void		SetView(const int &_w, const int &_h);
	int			GetWidth() { return m_iWidth; }
	int			GetHeight() { return m_iHeight; }

	void		SetFov(const float &_fov);
	float		GetFov() { return m_fFovy; }

	void		SetNear(const float &_near);
	float		GetNear() { return m_fNear; }

	void		SetFar(const float &_far);
	float		GetFar() { return m_fFar; }

	void		SetEye(const Vector3 &_eye);
	Vector3		GetEye() { return m_vEye; }

	void		SetCenter(const Vector3 &_at);
	Vector3		GetCenter() { return m_vAt; }

	void		SetUp(const Vector3 &_up);
	Vector3		GetUp() { return m_vUp; }


private:
	int			m_iWidth, m_iHeight;
	Vector3		m_vEye, m_vAt, m_vUp;
	float		m_fFovy, m_fNear, m_fFar;
};

#endif
