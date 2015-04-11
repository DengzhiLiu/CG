// HumanAnimatorView.h : interface of the CHumanAnimatorView class
//
#include "../Scene/SceneGL.h"
#include "../Scene/InterfaceManager.h"

#pragma once

class CHumanAnimatorDoc;

class CHumanAnimatorView : public CView
{
protected: // create from serialization only
	CHumanAnimatorView();
	DECLARE_DYNCREATE(CHumanAnimatorView)

// Attributes
public:
	CHumanAnimatorDoc* GetDocument() const;
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CHumanAnimatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	int					scree_width, screen_height;
	Vector3				lastPos;
	CSceneGL			m_SceneGL;
	InterfaceManager*	m_pInterface;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // debug version in HumanAnimatorView.cpp
inline CHumanAnimatorDoc* CHumanAnimatorView::GetDocument() const
   { return reinterpret_cast<CHumanAnimatorDoc*>(m_pDocument); }
#endif

