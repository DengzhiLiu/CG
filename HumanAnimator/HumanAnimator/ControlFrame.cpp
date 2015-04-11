// ControlFrame.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "HumanAnimator.h"
#include "ControlFrame.h"

#include <assert.h>

// CControlFrame dialog

IMPLEMENT_DYNAMIC(CControlFrame, CDialog)

CControlFrame::CControlFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CControlFrame::IDD, pParent)
	, m_FrameNo(0)
	, m_MeshName(_T(""))
	, m_MotionName(_T(""))
	, m_VertexNum(0)
	, m_FaceNum(0)
	, m_TimeInterval(0)
	, m_FPSDisp(_T(""))
	, m_FrameNum(500)
{
	m_bPlay = false;
}

CControlFrame::~CControlFrame()
{
}

void CControlFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAME_SLIDER, m_FrameSlider);
	DDX_Text(pDX, IDC_FRAME_NO, m_FrameNo);
	DDX_Text(pDX, IDC_DISP_MESH, m_MeshName);
	DDX_Text(pDX, IDC_DISP_BVH, m_MotionName);
	DDX_Text(pDX, IDC_DISP_VERTEX, m_VertexNum);
	DDX_Text(pDX, IDC_DISP_FACE, m_FaceNum);
	DDX_Text(pDX, IDC_EDIT_FPS, m_TimeInterval);
	DDX_Text(pDX, IDC_STATIC_FPS, m_FPSDisp);
	DDX_Text(pDX, IDC_FRAME_NUM, m_FrameNum);
}

BEGIN_MESSAGE_MAP(CControlFrame, CDialog)
	ON_BN_CLICKED(IDC_PLAY, &CControlFrame::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, &CControlFrame::OnBnClickedStop)
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FPS, &CControlFrame::OnDeltaposSpinFps)
	ON_BN_CLICKED(IDC_NEXT, &CControlFrame::OnBnClickedNext)
	ON_BN_CLICKED(IDC_PREV, &CControlFrame::OnBnClickedPrev)
	ON_BN_CLICKED(IDC_INSERT, &CControlFrame::OnBnClickedInsert)
END_MESSAGE_MAP()

BOOL CControlFrame::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pInterface = theApp.GetInterface();

	m_FrameSlider.SetRange(0, 499);
	m_FrameSlider.SetRangeMin(1);
	//m_FrameSlider.SetTicFreq(100);
	m_FrameSlider.SetPos(m_FrameNo);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CControlFrame::SetRange(int range)
{
	assert(range > 0);
	m_FrameSlider.SetRange(0, range-1);
	m_FrameNum = range;
	UpdateData(FALSE);
}

// CControlFrame message handlers
void CControlFrame::SetFrame(int frame)
{
	m_FrameNo = frame;
	m_FrameSlider.SetPos(m_FrameNo);
	UpdateData(FALSE);
}

void CControlFrame::OnBnClickedPlay()
{
	// TODO: Add your control notification handler code here
	m_bPlay = !m_bPlay;
	CMainFrame* pMain=(CMainFrame  *)AfxGetApp()->m_pMainWnd;
	if (m_bPlay) {
		pMain->ToggleTimer(m_TimeInterval);
	}
	else {
		pMain->ShutTimer();
		pMain->ViewSynch();
	}
}

void CControlFrame::OnBnClickedStop()
{
	// TODO: Add your control notification handler code here
	m_bPlay = false;
	CMainFrame* pMain=(CMainFrame  *)AfxGetApp()->m_pMainWnd;
	pMain->ShutTimer();

	m_FrameNo = 0;
	m_FrameSlider.SetPos(0);
	m_pInterface->ReSet();
	UpdateData(FALSE);
}

void CControlFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	m_FrameNo = m_FrameSlider.GetPos();
	m_pInterface->SetFrameNo(m_FrameNo);
	m_pInterface->Play();
	for (int i = 0; i < 2; i++)
		m_pInterface->GetScene(i)->PaintGL();
	
	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CControlFrame::OnDeltaposSpinFps(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_TimeInterval <= 1)
		m_TimeInterval = 1;
	else if (m_TimeInterval >= 1000)
		m_TimeInterval = 1000;

	if (pNMUpDown->iDelta < 0)
		m_TimeInterval += 1;
	else 
		m_TimeInterval -= 1;

	m_FPSDisp.Format("%.2f", 1000. / m_TimeInterval);
	UpdateData(FALSE);

	*pResult = 0;
}

void CControlFrame::OnBnClickedNext()
{
	// TODO: Add your control notification handler code here
	m_FrameNo++;
	if (m_FrameNo >= m_FrameNum)
		m_FrameNo = m_FrameNum - 1;
	else {
		m_pInterface->SetFrameNo(m_FrameNo);
		m_pInterface->Play();

		for (int i = 0; i < 2; i++)
			m_pInterface->GetScene(i)->PaintGL();
	}

	m_FrameSlider.SetPos(m_FrameNo);
	UpdateData(FALSE);
}

void CControlFrame::OnBnClickedPrev()
{
	// TODO: Add your control notification handler code here
	m_FrameNo--;
	if (m_FrameNo < 0)
		m_FrameNo = 0;
	else {
		m_pInterface->SetFrameNo(m_FrameNo);
		m_pInterface->Play();

		for (int i = 0; i < 2; i++)
			m_pInterface->GetScene(i)->PaintGL();
	}

	m_FrameSlider.SetPos(m_FrameNo);
	UpdateData(FALSE);
}

void CControlFrame::OnBnClickedInsert()
{
	// TODO: Add your control notification handler code here
	if ((m_FrameNo < 0) || (m_FrameNo >= m_FrameNum - 1))
		return;

	DefMesh* pdefmesh = m_pInterface->getDefmesh();
	if (pdefmesh == NULL)	return;
	
	pdefmesh->InsertMotion(m_FrameNo);
	++m_FrameNum;
	UpdateData(FALSE);
}
