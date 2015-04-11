// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CGHiding.h"

#include "MainFrm.h"
#include "CGHidingView.h"
#include "InteractView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	GetMenu()->GetSubMenu(1)->CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
	GetMenu()->GetSubMenu(1)->CheckMenuItem(3, MF_BYPOSITION | MF_CHECKED);
	GetMenu()->GetSubMenu(1)->CheckMenuItem(6, MF_BYPOSITION | MF_CHECKED);
	GetMenu()->GetSubMenu(0)->SetDefaultItem(1, TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = _T("É¨ÃèÏßZ»º³åÆ÷Ëã·¨");
	cs.cx = 1000;
	cs.cy = 600;
	cs.lpszClass=AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,0,0,
		LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1)));
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	if(!m_wndSplitter.CreateStatic(this,1,2)
		|| !m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CCGHidingView),CSize(800,600),pContext)
		|| !m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CInteractView),CSize(200,600),pContext)
		)
		return FALSE;
	// ¹Ø¼ü°¡
	SetActiveView((CCGHidingView *)m_wndSplitter.GetPane(0,0));

	return TRUE;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}
