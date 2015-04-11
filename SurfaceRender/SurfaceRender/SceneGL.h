#ifndef _SCENEGL_H
#define _SCENEGL_H

#pragma once
#include "View.h"
#include "Transform.h"
#include <cutil_inline.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

class CInterfaceManager;

class CSceneGL
{
public:
	enum TransMode { OpNull, Translation, Scale, Rotation, Selecting };
	enum DrawMode	{ DMPoints, DMWire, DMHidden, DMFill, DMFillWire };

	CSceneGL(void);
	~CSceneGL(void);

	bool		InitGL();

	void		SetView();

	bool		SetupPixelFormat();

	bool		InitializeDevice(const HDC& _hdc);

	void		ReSize(int _width, int _height);

	void		SetCamera(Vector3 _center);

	void		SetTransMode(TransMode _tm);

	TransMode	GetTransMode() const { return m_TransMode; }

	void		SetBlendState(bool _state);

	//void		SetBlendColor() { m_bBlend = !m_bBlend; }

	void		SetRenderState() { m_bRender = !m_bRender; }

	void		SetDrawMode(DrawMode _dm);

	void		SetInterface(CInterfaceManager *pInterface);

	Vector3		GetTransPointClamp(Vector3 p);

	Vector3		GetTransPointWrap(Vector3 p);

	void		MouseMoveEvent(Vector3 _last, Vector3 _cur);

	void		MouseWheelEvent(ElemType _scale);

	void		ResetModel();

	void		DrawBackGround();

	void		PaintGL();

	void		SaveBMP(const char* _fileName);

public:
	HDC			m_DC;

	HGLRC		m_hRC;

	View*		m_pCamera;

	bool		m_bBlend;

	bool		m_bRender;

	TransMode	m_TransMode;

	DrawMode	m_DrawMode;

	Transform	m_Transform;

	CInterfaceManager* m_pInterface;
};

#endif
