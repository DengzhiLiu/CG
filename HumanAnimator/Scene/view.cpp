#include"stdafx.h"
#include "view.h"

View::View()
{
	m_width = 1024;
	m_height = 768;
	m_fovy = 60.;
	m_near = 0.01;
	m_far = 1000.;
	m_eye = Vector3(0., 0., 1);
	m_at = Vector3(0., 0., 0.);
	m_up = Vector3(0., 1., 0.);
	m_viewMode = ViewFront;
}

View::View(int w, int h, float fov, float near, float far,Vector3 eye, Vector3 at,Vector3 up):
			m_width(w), m_height(h), m_fovy(fov), m_near(near), m_far(far), 
			m_eye(eye), m_at(at), m_up(up)
{
}

void View::SetView(int w, int h)
{
	m_width = w;
	m_height = h;
}

void View::SetFov(float infov)
{
	m_fovy = infov;
}

void View::SetNear(float znear)
{
	m_near = znear;
}

void View::SetFar(float zfar)
{
	m_far = zfar;
}

void View::SetEye(Vector3 inEye)
{
	m_eye = inEye;
}

void View::SetCenter(Vector3 inAt)
{
	m_at = inAt;
}

void View::SetUp(Vector3 inUp)
{
	m_up = inUp;
}

void View::SetViewMode(ViewMode mode)
{
	m_viewMode = mode;

	switch (mode)
	{
	case ViewFront:
		m_eye = m_at + Vector3(0, 0, 2);
		break;
	case ViewBack:
		m_eye = m_at + Vector3(0, 0, -2);
		break;
	case ViewLeft:
		m_eye = m_at + Vector3(2, 0, 0);
		break;
	case ViewRight:
		m_eye = m_at + Vector3(-2, 0, 0);
		break;
	default:
		break;
	}
	
}