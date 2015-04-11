// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "resource.h"
#include "HumanAnimator.h"

#include <assert.h>
#include "MainFrm.h"
#include "../Scene/SceneGL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_IMPORTMOTION, &CMainFrame::OnFileImportmotion)
	ON_COMMAND(ID_FILE_IMPORTOBJ, &CMainFrame::OnFileImportobj)
	ON_COMMAND(ID_FILE_IMPORTWEIGHT, &CMainFrame::OnFileImportweight)
	ON_COMMAND(ID_FILE_EXPORTMOTION, &CMainFrame::OnFileExportmotion)
	ON_COMMAND(ID_DRAW_FILL, &CMainFrame::OnDrawFill)
	ON_COMMAND(ID_DRAW_FILLWIRE, &CMainFrame::OnDrawFillwire)
	ON_COMMAND(ID_DRAW_HIDDEN, &CMainFrame::OnDrawHidden)
	ON_COMMAND(ID_DRAW_POINTS, &CMainFrame::OnDrawPoints)
	ON_COMMAND(ID_DRAW_WIRE, &CMainFrame::OnDrawWire)
	ON_COMMAND(ID_SHADE_FLAT, &CMainFrame::OnShadeFlat)
	ON_COMMAND(ID_SHADE_SMOOTH, &CMainFrame::OnShadeSmooth)
	ON_COMMAND(ID_SHADE_TEXTURE, &CMainFrame::OnShadeTexture)
	ON_COMMAND(ID_FILE_RELOAD, &CMainFrame::OnFileReload)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_RENDER_MESH, &CMainFrame::OnRenderMesh)
	ON_COMMAND(ID_RENDER_SKELETON, &CMainFrame::OnRenderSkeleton)
	ON_COMMAND(ID_WIN_CONTROL, &CMainFrame::OnWinControl)
	ON_COMMAND(ID_DRAW_FLOOR, &CMainFrame::OnDrawFloor)
	ON_COMMAND(ID_RESET, &CMainFrame::OnReset)
	ON_WM_TIMER()
	ON_COMMAND(ID_RENDER_CPU, &CMainFrame::OnRenderCpu)
	ON_COMMAND(ID_RENDER_GPU, &CMainFrame::OnRenderGpu)
	ON_UPDATE_COMMAND_UI(ID_RENDER_CPU, &CMainFrame::OnUpdateRenderCpu)
	ON_UPDATE_COMMAND_UI(ID_RENDER_GPU, &CMainFrame::OnUpdateRenderGpu)
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
	m_pControlFrame = NULL;
}

CMainFrame::~CMainFrame()
{
	m_pControlFrame->DestroyWindow();
	if (m_pControlFrame)
		delete m_pControlFrame;
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

	m_ilTB.Create(32, 32, TRUE | ILC_COLOR8, 17, 0);
	HICON hicon = NULL;
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_OPEN), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_RELOAD), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SAVE), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_POINTS), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_WIRE), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BACKLINES), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FLATLINES), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FLAT), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SMOOTH), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TEXTURE), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CPUACC), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_GPUACC), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_COORD), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MESH), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SKEL), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CONTROL), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);
	hicon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ABOUT), IMAGE_ICON, 32, 32, 0);
	m_ilTB.Add(hicon);

	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilTB);
	m_wndToolBar.SetSizes(CSize(40, 40), CSize(32, 32));

	//m_wndToolBar.SetButtonInfo()

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_RenderType = CPURENDER;
	m_pInterface = theApp.GetInterface();
	m_pControlFrame = new CControlFrame();
	m_pControlFrame->Create(IDD_CONTROL, this);

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~FWS_ADDTOTITLE;
	//cs.lpszName = _T("Human Animator");
	cs.lpszName = _T("3D Avatar");
	cs.cx = 1024;
	cs.cy = 768;

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
	if ( !m_wndSplitter.Create(this, RUNTIME_CLASS(CHumanAnimatorView), 
		RUNTIME_CLASS(CHumanAnimatorView), pContext, TRUE) )
		return FALSE;

	view[0] = (CHumanAnimatorView *)m_wndSplitter.GetPane(0, 0);
	view[1] = (CHumanAnimatorView *)m_wndSplitter.GetPane(0, 1);
	m_wndSplitter.SetActivePane(0, 0);
	m_wndSplitter.ToggleSide(RIGHT_SIDE);

	return TRUE;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}

const CString CMainFrame::ALL_Filter = _T("All File|*.*||");
const CString CMainFrame::OBJ_Filter = _T("OBJ Files (*.obj)|*.obj||");
const CString CMainFrame::MOTION_Filter = _T("BVH Files (*.bvh)|*.bvh|SKELETON Files (*.skel)|*.skel||");
const CString CMainFrame::WEIGHT_Filter = _T("WEIGHT Files (*.weight)|*.weight||");
const CString CMainFrame::BVH_Filter = _T("Motion Files (*.bvh)|*.bvh||");

void CMainFrame::ToggleTimer(float interval)
{
	SetTimer(1, (UINT)(interval), NULL);
}

void CMainFrame::ShutTimer()
{
	KillTimer(1);
}

void CMainFrame::ViewSynch()
{
	m_pInterface->Play();
	for (int i = 0; i < 2; i++)
		m_pInterface->GetScene(i)->PaintGL();
}

void CMainFrame::OnFileOpen()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		ALL_Filter);
	dlg.m_ofn.lpstrTitle = "Open File";

	CString file_name;
	if (dlg.DoModal() == IDOK) {
		file_name = dlg.GetPathName();
		string name = file_name.GetBuffer();
		file_name.ReleaseBuffer();
		int npos = name.rfind('.');
		if (npos == -1)	return;
		else {
			string surfix = string(name, npos+1);
			if (_stricmp(surfix.c_str(), "obj") == 0) {
				m_pInterface->LoadMesh(name);
				m_pControlFrame->m_VertexNum = m_pInterface->getDefmesh()->GetMesh().totvert;
				m_pControlFrame->m_FaceNum = m_pInterface->getDefmesh()->GetMesh().totface;
				m_pControlFrame->m_MeshName = file_name;
				m_pControlFrame->UpdateData(FALSE);
			}
			else if ((_stricmp(surfix.c_str(), "bvh") == 0) || (_stricmp(surfix.c_str(), "skel") == 0)) {
				m_pInterface->LoadMotion(name);

				int frame_num = m_pInterface->getDefmesh()->GetFrameNum();
				float interval = m_pInterface->getDefmesh()->GetClip()->getInterval();

				m_pControlFrame->SetRange(frame_num);
				m_pControlFrame->m_TimeInterval = (int)(interval*1000000) / 1000.;
				m_pControlFrame->m_FPSDisp.Format("%.2f", 1. / interval);
				m_pControlFrame->m_MotionName = file_name.Mid(file_name.ReverseFind('\\')+1);
				m_pControlFrame->UpdateData(FALSE);
			}
			else if (_stricmp(surfix.c_str(), "weight") == 0)
				m_pInterface->LoadWeight(name);
			else
				return;
		}
	}
	else {
		TRACE("Can't Open %s File!", file_name);
		return ;
	}
}

void CMainFrame::OnFileReload()
{
	// TODO: Add your command handler code here
	DefMesh* pdefmesh = m_pInterface->getDefmesh();
	if (pdefmesh)	{
		string name = pdefmesh->GetMesh().ModelFileName;
		m_pInterface->LoadMesh(name);
	}
}

void CMainFrame::OnFileImportobj()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		OBJ_Filter);
	dlg.m_ofn.lpstrTitle = "Open Mesh Model";

	CString file_name;
	if (dlg.DoModal() == IDOK) {
		file_name = dlg.GetPathName();
		m_pInterface->LoadMesh(file_name.GetBuffer());
		file_name.ReleaseBuffer();
		m_pControlFrame->m_VertexNum = m_pInterface->getDefmesh()->GetMesh().totvert;
		m_pControlFrame->m_FaceNum = m_pInterface->getDefmesh()->GetMesh().totface;
		m_pControlFrame->m_MeshName = file_name.Mid(file_name.ReverseFind('\\')+1);
		m_pControlFrame->UpdateData(FALSE);
	}
	else {
		TRACE("Can't Open %s File!", file_name);
		return ;
	}
}

void CMainFrame::OnFileImportmotion()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		MOTION_Filter);
	dlg.m_ofn.lpstrTitle = "Open Motion Data";

	CString file_name;
	if (dlg.DoModal() == IDOK) {
		CString file_name = dlg.GetPathName();
		m_pInterface->LoadMotion(file_name.GetBuffer());
		file_name.ReleaseBuffer();

		int frame_num = m_pInterface->getDefmesh()->GetFrameNum();
		float interval = m_pInterface->getDefmesh()->GetClip()->getInterval();

		m_pControlFrame->SetRange(frame_num);
		m_pControlFrame->m_TimeInterval = (int)(interval*1000000) / 1000.;
		m_pControlFrame->m_FPSDisp.Format("%.2f", 1. / interval);
		m_pControlFrame->m_MotionName = file_name.Mid(file_name.ReverseFind('\\')+1);
		m_pControlFrame->UpdateData(FALSE);
	}
	else {
		TRACE("Can't Open %s File!", file_name);
	}
}

void CMainFrame::OnFileImportweight()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		WEIGHT_Filter);
	dlg.m_ofn.lpstrTitle = "Open Weight Data";

	CString file_name;
	if (m_pInterface->getDefmesh() == NULL)
	{
		TRACE("No mesh exists!");
		return ;
	}

	if (dlg.DoModal() == IDOK) {
		CString file_name = dlg.GetFileName();
		m_pInterface->LoadWeight(file_name.GetBuffer());
		file_name.ReleaseBuffer();
	}
	else {
		TRACE("Can't Open %s File!", file_name);
	}
}

void CMainFrame::OnFileExportmotion()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(FALSE, NULL, NULL, OFN_CREATEPROMPT | OFN_HIDEREADONLY, BVH_Filter, this);
	dlg.m_ofn.lpstrTitle =  "Save Motion Data";

	if (dlg.DoModal() == IDOK) {
		CString file_path = dlg.GetPathName();
		CString file_ext = dlg.GetFileExt();
		if (file_ext.GetLength() == 0)
			file_path = file_path + ".bvh";

		m_pInterface->ExportMotion(file_path.GetBuffer());
		file_path.ReleaseBuffer();
	}
	else {
		TRACE("Can't Save Motion Data!");
	}
}

void CMainFrame::OnDrawFill()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMode(GLW::DMFill);
	curScene->PaintGL();
}

void CMainFrame::OnDrawFillwire()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMode(GLW::DMFillWire);
	curScene->PaintGL();
}

void CMainFrame::OnDrawHidden()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMode(GLW::DMHidden);
	curScene->PaintGL();
}

void CMainFrame::OnDrawPoints()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMode(GLW::DMPoints);
	curScene->PaintGL();
}

void CMainFrame::OnDrawWire()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMode(GLW::DMWire);
	curScene->PaintGL();
}

void CMainFrame::OnShadeFlat()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMode(GLW::DMFill);
	curScene->SetShadeMode(GLW::SMFlat);
	curScene->PaintGL();
}

void CMainFrame::OnShadeSmooth()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMode(GLW::DMFill);
	curScene->SetShadeMode(GLW::SMSmooth);
	curScene->PaintGL();
}

void CMainFrame::OnShadeTexture()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetTexture();
	curScene->PaintGL();
}

void CMainFrame::OnReset()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->ResetCoord();
	curScene->PaintGL();
}

void CMainFrame::OnDrawFloor()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawFloor();
	curScene->PaintGL();
}

void CMainFrame::OnRenderMesh()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawMesh();
	curScene->PaintGL();
}

void CMainFrame::OnRenderSkeleton()
{
	// TODO: Add your command handler code here
	int view_no = m_wndSplitter.GetActivePaneNo();
	CSceneGL *curScene = m_pInterface->GetScene(view_no);
	curScene->SetDrawSkel();
	curScene->PaintGL();
}

void CMainFrame::OnWinControl()
{
	// TODO: Add your command handler code here
	RECT rc, rcMain, rcScreen;

	m_pControlFrame->ShowWindow(SW_SHOW);
	::GetWindowRect( m_pControlFrame->GetSafeHwnd(), &rc );
	::GetWindowRect( this->GetSafeHwnd(), &rcMain );
	::GetClientRect(::GetDesktopWindow(), &rcScreen);
	::SetWindowPos(m_pControlFrame->GetSafeHwnd(), 0, rcMain.right, rcMain.top,
		max(160, min(rcScreen.right, rc.right) - rc.left), rcMain.bottom - rcMain.top, 0);
	::SetFocus(this->GetSafeHwnd());
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_SYSKEYDOWN)   
	{   
		if(pMsg->lParam & 0x21000000) {  
			if(pMsg->wParam=='W') {
				int active = m_wndSplitter.GetActivePaneNo();
				assert(active < 2);
				m_wndSplitter.ToggleSide(1 - active);
				UpdateWindow();
			}
		}
	}  
	if (pMsg->message == WM_KEYDOWN)
	{
		int view_no = m_wndSplitter.GetActivePaneNo();
		CSceneGL *curScene = m_pInterface->GetScene(view_no);

		if (pMsg->wParam == 0x0031 || pMsg->wParam == 0x0061)
			curScene->SetProjMode(CSceneGL::PerspecPro);
		else if (pMsg->wParam == 0x0032 || pMsg->wParam == 0x0062)
			curScene->SetProjMode(CSceneGL::OrthoPro);
		else if (pMsg->wParam == 0x0034 || pMsg->wParam == 0x0064)
			curScene->SetCamera(View::ViewFront);	
		else if (pMsg->wParam == 0x0035 || pMsg->wParam == 0x0065)
			curScene->SetCamera(View::ViewBack);
		else if (pMsg->wParam == 0x0037 || pMsg->wParam == 0x0067)
			curScene->SetCamera(View::ViewLeft);
		else if (pMsg->wParam == 0x0038 || pMsg->wParam == 0x0068)
			curScene->SetCamera(View::ViewRight);
		else if (pMsg->wParam == 0x0053 || pMsg->wParam == 0x0073)
			curScene->SetSkelSphere();
		else if (pMsg->wParam == 0x0046 || pMsg->wParam == 0x0066)
			curScene->SetDrawFloor();

		curScene->PaintGL();
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::UpdateFrame()
{
	int frame_no = m_pInterface->GetFrameNo();
	m_pControlFrame->SetFrame(frame_no);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)	{
		int view_no = m_wndSplitter.GetActivePaneNo();
		m_pInterface->Play();
		m_pInterface->GetScene(view_no)->PaintGL();
		UpdateFrame();
	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnRenderCpu()
{
	// TODO: Add your command handler code here
	m_RenderType = CPURENDER;
	m_pInterface->SetRenderContext(CPURENDER);
	for (int i = 0; i < 2; i++)
		m_pInterface->GetScene(i)->PaintGL();
}

void CMainFrame::OnUpdateRenderCpu(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_RenderType == CPURENDER);
}

void CMainFrame::OnRenderGpu()
{
	// TODO: Add your command handler code here
	m_RenderType = GPURENDER;
	m_pInterface->SetRenderContext(GPURENDER);
	for (int i = 0; i < 2; i++)
		m_pInterface->GetScene(i)->PaintGL();
}

void CMainFrame::OnUpdateRenderGpu(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_RenderType == GPURENDER);
}