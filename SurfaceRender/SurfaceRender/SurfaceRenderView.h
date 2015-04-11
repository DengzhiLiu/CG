// SurfaceRenderView.h : interface of the CSurfaceRenderView class
//


#pragma once

#include "SceneGL.h"
#include "InterfaceManager.h"


class CSurfaceRenderView : public CView
{
protected: // create from serialization only
	CSurfaceRenderView();
	DECLARE_DYNCREATE(CSurfaceRenderView)

// Attributes
public:
	CSurfaceRenderDoc* GetDocument() const;

// Operations
public:
	int			m_ScreenWidth, m_ScreenHeight;
	Vector3		m_vLastPos;
	CSceneGL	m_SceneGL;
	CSceneGL::TransMode m_OpMode;
	CInterfaceManager*	m_pInterface;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CSurfaceRenderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSurfaceabstract();
	afx_msg void OnWinReset();
	afx_msg void OnDrawFill();
	afx_msg void OnDrawWire();
};

#ifndef _DEBUG  // debug version in SurfaceRenderView.cpp
inline CSurfaceRenderDoc* CSurfaceRenderView::GetDocument() const
   { return reinterpret_cast<CSurfaceRenderDoc*>(m_pDocument); }
#endif

