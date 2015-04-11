// InteractView.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "CGHiding.h"
#include "InteractView.h"
#include "CGHidingView.h"

// CInteractView

IMPLEMENT_DYNCREATE(CInteractView, CFormView)

CInteractView::CInteractView()
	: CFormView(CInteractView::IDD)
	, m_light_type(1)
	, m_color_type(0)
	, m_render_type(0)
{
}

CInteractView::~CInteractView()
{
}

void CInteractView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VERTEX, m_vertex_num);
	DDX_Control(pDX, IDC_STATIC_FACE, m_face_num);
	DDX_Radio(pDX, IDC_LIGHT_PARA, m_light_type);
	DDX_Radio(pDX, IDC_COLOR_GRAY, m_color_type);
	DDX_Radio(pDX, IDC_OPENGL, m_render_type);
}

BEGIN_MESSAGE_MAP(CInteractView, CFormView)
	ON_BN_CLICKED(IDC_REDRAW, &CInteractView::OnBnClickedRedraw)
END_MESSAGE_MAP()


// CInteractView diagnostics

#ifdef _DEBUG
void CInteractView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CInteractView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void CInteractView::OnBnClickedRedraw()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	((CCGHidingView *)(pFrame->m_wndSplitter.GetPane(0, 0)))->OnPaint();
}
