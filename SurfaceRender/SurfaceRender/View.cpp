#include "stdafx.h"
#include "View.h"

View::View(void)
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_fFovy = 60;
	m_fNear = 0.01;
	m_fFar = 1000;
	m_vEye = Vector3(0, 0, 2);
	m_vAt = Vector3(0, 0, 0);
	m_vUp = Vector3(0, 1, 0);
}

View::~View(void)
{
}

View::View(int _w, int _h, float _fov, float _near, float _far, \
		   Vector3 _eye, Vector3 _at, Vector3 _up)
{
	m_iWidth = _w;
	m_iHeight = _h;
	m_fFovy = _fov;
	m_fNear = _near;
	m_fFar = _far;
	m_vEye = _eye;
	m_vAt = _at;
	m_vUp = _up;
}

void View::SetView(const int &_w, const int &_h)
{
	m_iWidth = _w;
	m_iHeight = _h;
}

void View::SetFov(const float &_fov)
{
	m_fFovy = _fov;
}

void View::SetNear(const float &_near)
{
	m_fNear = _near;
}

void View::SetFar(const float &_far)
{
	m_fFar = _far;
}

void View::SetEye(const Vector3 &_eye)
{
	m_vEye = _eye;
}

void View::SetCenter(const Vector3 &_at)
{
	m_vAt = _at;
}

void View::SetUp(const Vector3 &_up)
{
	m_vUp = _up;
}
