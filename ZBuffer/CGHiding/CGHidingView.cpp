// CGHidingView.cpp : implementation of the CCGHidingView class
//

#include "stdafx.h"
#include "CGHiding.h"

#include "CGHidingDoc.h"
#include "CGHidingView.h"
#include "InteractView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCGHidingView

IMPLEMENT_DYNCREATE(CCGHidingView, CView)

BEGIN_MESSAGE_MAP(CCGHidingView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CCGHidingView construction/destruction

CCGHidingView::CCGHidingView()
{
	// TODO: add construction code here
}

CCGHidingView::~CCGHidingView()
{
	g_OpenGL.Finish();
}

BOOL CCGHidingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//防止在窗口重叠时把图形绘制到子窗口和兄弟窗口
	cs.style |=WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

// CCGHidingView drawing

void CCGHidingView::OnDraw(CDC* pDC)
{
	CCGHidingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CCGHidingView diagnostics

#ifdef _DEBUG
void CCGHidingView::AssertValid() const
{
	CView::AssertValid();
}

void CCGHidingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGHidingDoc* CCGHidingView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGHidingDoc)));
	return (CCGHidingDoc*)m_pDocument;
}
#endif //_DEBUG


// CCGHidingView message handlers

int CCGHidingView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	//获取客户区的设备描述表 
	m_pDC = new CClientDC(this);
	//初始化OpenGL
	g_OpenGL.InitializeOpenGL(m_pDC);
	//初始化OpenGL的一些状态参数并对地形数据进行初始化，可以按其它方式进行进行初始化地形数据。
	g_OpenGL.InitGL(); 

	return 0;
}

void CCGHidingView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	glViewport(0, 0, cx, cy);
	// TODO: Add your message handler code here
}

void CCGHidingView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	static CPoint last(-1, -1);
	if ((GetDocument()->IsValid()) && (nFlags & MK_LBUTTON))
	{
		if (last.x < 0 || last.y < 0) last = point;
		g_OpenGL.RotateObject(point.x - last.x, point.y - last.y);
		last = point;
		RedrawWindow();
	}
	//CView::OnMouseMove(nFlags, point);
}

BOOL CCGHidingView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (GetDocument()->IsValid())
		 g_OpenGL.ZoomObject(zDelta * 0.0001);
		RedrawWindow();
	return TRUE;
	//return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CCGHidingView::OnPaint()
{

	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	CCGHidingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rect;
	GetClientRect(rect);
	g_OpenGL.Set3DProjection(rect.Width(), rect.Height());

	CInteractView *pInterView = pDoc->get_i_view();
	pInterView->UpdateData(TRUE);
	g_Hide.Draw(pInterView->m_light_type, pInterView->m_color_type, 
		pInterView->m_render_type, &(pDoc->m_3DModel));

	// Do not call CView::OnPaint() for painting messages
}
