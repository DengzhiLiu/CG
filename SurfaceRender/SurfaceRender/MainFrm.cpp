// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SurfaceRender.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_WIN_CONTROL, &CMainFrame::OnWinControl)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame(): m_pControlFrame(NULL), m_pInterface(NULL)
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	m_pControlFrame->DestroyWindow();
	if (m_pControlFrame)
		delete m_pControlFrame;
	m_pControlFrame = NULL;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_ilTB.Create(32, 32, TRUE | ILC_COLOR8, 10, 0);
	HICON hicon = NULL;
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_OPEN), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_RELOAD), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SAVE), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FILL), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_WIRE), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_COORD), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ABSTRACT), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CONTROL), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ABOUT), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);

	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilTB);
	m_wndToolBar.SetSizes(CSize(40, 40), CSize(32, 32));

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_pInterface = theApp.GetInterface();
	m_pControlFrame = new CControlFrame();
	m_pControlFrame->Create(IDD_CONTROL, this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = _T("Surface Render");
	cs.cx = 1024;
	cs.cy = 768;
	//cs.lpszClass=AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, 0, 0,
	//	LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LOGO)));

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




void CMainFrame::OnWinControl()
{
	// TODO: Add your command handler code here
	m_pControlFrame->ShowWindow(SW_SHOW);
	RECT rc, rcMain, rcScreen;
	::GetWindowRect( m_pControlFrame->GetSafeHwnd(), &rc );
	::GetWindowRect( this->GetSafeHwnd(), &rcMain );
	::GetClientRect(::GetDesktopWindow(), &rcScreen);

	::SetWindowPos(m_pControlFrame->GetSafeHwnd(), 0,
		rcMain.right, rcMain.top,
		max(160, min(rcScreen.right, rc.right) - rc.left), rcMain.bottom - rcMain.top, 0);

	::SetFocus(this->GetSafeHwnd());
}
