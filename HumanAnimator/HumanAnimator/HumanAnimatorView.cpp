// HumanAnimatorView.cpp : implementation of the CHumanAnimatorView class
//

#include "stdafx.h"
#include "HumanAnimator.h"

#include "HumanAnimatorDoc.h"
#include "HumanAnimatorView.h"

//#include <gl/wglext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHumanAnimatorView

IMPLEMENT_DYNCREATE(CHumanAnimatorView, CView)

BEGIN_MESSAGE_MAP(CHumanAnimatorView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CHumanAnimatorView construction/destruction

CHumanAnimatorView::CHumanAnimatorView()
{
	// TODO: add construction code here
	lastPos = Vector3(-100, -100, -100);
}

CHumanAnimatorView::~CHumanAnimatorView()
{
}

BOOL CHumanAnimatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CHumanAnimatorView drawing

void CHumanAnimatorView::OnDraw(CDC* /*pDC*/)
{
	CHumanAnimatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_SceneGL.PaintGL();	
}


// CHumanAnimatorView diagnostics

#ifdef _DEBUG
void CHumanAnimatorView::AssertValid() const
{
	CView::AssertValid();
}

void CHumanAnimatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CHumanAnimatorDoc* CHumanAnimatorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHumanAnimatorDoc)));
	return (CHumanAnimatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CHumanAnimatorView message handlers

int CHumanAnimatorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_SceneGL.InitializeDevice(GetDC()->m_hDC);
	m_SceneGL.InitGL(); 

	glewInit();
	m_pInterface = theApp.GetInterface();
	m_SceneGL.SetInterface(m_pInterface);
	m_pInterface->StoreScene(&m_SceneGL);

	//PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;    // determine pointer to wglGetExtensionsStringEXT function
	//_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
	//_wglGetExtensionsStringEXT(0);
	return 0;
}

void CHumanAnimatorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	scree_width = cx;
	screen_height = cy;
	m_SceneGL.m_pcam->SetView(cx, cy);
}

void CHumanAnimatorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	Vector3 pos((float)point.x, (float)(screen_height - 1 - point.y), 0);
	if ((nFlags & MK_LBUTTON))
		lastPos = pos;
}

void CHumanAnimatorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	Vector3 pos((float)point.x, (float)(screen_height - point.y), 0);
	if ((nFlags & MK_LBUTTON))
	{
		if (lastPos[2] == -100)	{
			lastPos = pos;
			return;
		}

		if (nFlags & MK_CONTROL)
			m_SceneGL.SetTransMode(CSceneGL::Translation);
		else
			m_SceneGL.SetTransMode(CSceneGL::Rotation);

		m_SceneGL.MouseMoveEvent(lastPos, pos);
		lastPos = pos;
	}
}

BOOL CHumanAnimatorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	m_SceneGL.MouseWheelEvent(-zDelta * 0.0001);

	return TRUE;
}