// SurfaceRenderView.cpp : implementation of the CSurfaceRenderView class
//

#include "stdafx.h"
#include "SurfaceRender.h"

#include "SurfaceRenderDoc.h"
#include "SurfaceRenderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSurfaceRenderView

IMPLEMENT_DYNCREATE(CSurfaceRenderView, CView)

BEGIN_MESSAGE_MAP(CSurfaceRenderView, CView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_WIN_RESET, &CSurfaceRenderView::OnWinReset)
	ON_COMMAND(ID_DRAW_FILL, &CSurfaceRenderView::OnDrawFill)
	ON_COMMAND(ID_DRAW_WIRE, &CSurfaceRenderView::OnDrawWire)
END_MESSAGE_MAP()

// CSurfaceRenderView construction/destruction

CSurfaceRenderView::CSurfaceRenderView()
{
	// TODO: add construction code here
	m_pInterface = NULL;
	m_OpMode = CSceneGL::Translation;
	m_vLastPos = Vector3(-1000, -1000, -1000);

}

CSurfaceRenderView::~CSurfaceRenderView()
{
}

BOOL CSurfaceRenderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |=WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

// CSurfaceRenderView drawing

void CSurfaceRenderView::OnDraw(CDC* /*pDC*/)
{
	CSurfaceRenderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	m_SceneGL.PaintGL();
}


// CSurfaceRenderView diagnostics

#ifdef _DEBUG
void CSurfaceRenderView::AssertValid() const
{
	CView::AssertValid();
}

void CSurfaceRenderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSurfaceRenderDoc* CSurfaceRenderView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSurfaceRenderDoc)));
	return (CSurfaceRenderDoc*)m_pDocument;
}
#endif //_DEBUG


// CSurfaceRenderView message handlers

void CSurfaceRenderView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_ScreenWidth = cx;
	m_ScreenHeight = cy;
	m_SceneGL.ReSize(cx, cy);
}

int CSurfaceRenderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_SceneGL.InitializeDevice(GetDC()->m_hDC);
	m_SceneGL.InitGL(); 

	m_pInterface = theApp.GetInterface();
	m_SceneGL.SetInterface(m_pInterface);
	m_pInterface->SetCurScene(&m_SceneGL);

	return 0;
}

void CSurfaceRenderView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	Vector3 pos(point.x, (m_ScreenHeight - point.y), 0);
	if ((nFlags & MK_LBUTTON))
	{
		m_vLastPos = pos;
	}
}

void CSurfaceRenderView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonUp(nFlags, point);
}

void CSurfaceRenderView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	Vector3 pos(point.x, (m_ScreenHeight - point.y), 0);
	if ((nFlags & MK_LBUTTON))
	{
		if (-1000 == m_vLastPos[2])
		{
			m_vLastPos = pos;
			return;
		}

		if (nFlags & MK_CONTROL)
			m_OpMode = CSceneGL::Translation;
		else
			m_OpMode = CSceneGL::Rotation;
		m_SceneGL.SetTransMode(m_OpMode);

		m_SceneGL.MouseMoveEvent(m_vLastPos, pos);
		m_vLastPos = pos;
	}
}

BOOL CSurfaceRenderView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	m_SceneGL.MouseWheelEvent(-zDelta * 0.0001);
	return true;
}
void CSurfaceRenderView::OnWinReset()
{
	// TODO: Add your command handler code here
	m_SceneGL.ResetModel();
}

void CSurfaceRenderView::OnDrawFill()
{
	// TODO: Add your command handler code here
	m_SceneGL.SetDrawMode(CSceneGL::DMFill);
	m_SceneGL.PaintGL();
}

void CSurfaceRenderView::OnDrawWire()
{
	// TODO: Add your command handler code here
	m_SceneGL.SetDrawMode(CSceneGL::DMWire);
	m_SceneGL.PaintGL();
}
