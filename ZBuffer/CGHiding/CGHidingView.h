// CGHidingView.h : interface of the CCGHidingView class
//


#pragma once

#define OPENGL_MODE  1
#define PROGRAM_MODE 2

#include "OpenGL.h"
#include "Hide.h"
#include <math.h>

class CCGHidingDoc;

class CCGHidingView : public CView
{
protected: // create from serialization only
	CCGHidingView();
	DECLARE_DYNCREATE(CCGHidingView)

// Attributes
public:
	
private:
	CClientDC	*m_pDC;

// Operations
public:
	CCGHidingDoc* GetDocument() const;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CCGHidingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

public:
	afx_msg void OnPaint();
};

#ifndef _DEBUG  // debug version in CGHidingView.cpp
inline CCGHidingDoc* CCGHidingView::GetDocument() const
   { return reinterpret_cast<CCGHidingDoc*>(m_pDocument); }
#endif

