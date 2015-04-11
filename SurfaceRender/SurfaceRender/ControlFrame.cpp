// ControlFrame.cpp : implementation file
//

#include "stdafx.h"
#include "SurfaceRender.h"
#include "ControlFrame.h"


// CControlFrame dialog

IMPLEMENT_DYNAMIC(CControlFrame, CDialog)

CControlFrame::CControlFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CControlFrame::IDD, pParent),
	m_pInterface(NULL), m_pScene(NULL)
	//, m_bSur1(true), m_bSur2(true), m_bSur3(true)
	//, m_bSur4(true), m_bSur5(true), m_bSur6(true)
	//, m_bSur7(true), m_bSur8(true), m_bSur9(true)
	//, m_bSur10(true), m_bSur11(true), m_bSur12(true)
	//, m_bSur13(true), m_bSur14(true), m_bSur15(true)
	//, m_bSur16(true), m_bSur17(true), m_bSur18(true)
	, m_nSurNum(0)
{
}

CControlFrame::~CControlFrame()
{
}

void CControlFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_SUR1, m_bSur1);
	//DDX_Text(pDX, IDC_SUR2, m_bSur2);
	//DDX_Text(pDX, IDC_SUR3, m_bSur3);
	//DDX_Text(pDX, IDC_SUR4, m_bSur4);
	//DDX_Text(pDX, IDC_SUR5, m_bSur5);
	//DDX_Text(pDX, IDC_SUR6, m_bSur6);
	//DDX_Text(pDX, IDC_SUR7, m_bSur7);
	//DDX_Text(pDX, IDC_SUR8, m_bSur8);
	//DDX_Text(pDX, IDC_SUR9, m_bSur9);
	//DDX_Text(pDX, IDC_SUR10, m_bSur10);
	//DDX_Text(pDX, IDC_SUR11, m_bSur11);
	//DDX_Text(pDX, IDC_SUR12, m_bSur12);
	//DDX_Text(pDX, IDC_SUR13, m_bSur13);
	//DDX_Text(pDX, IDC_SUR14, m_bSur14);
	//DDX_Text(pDX, IDC_SUR15, m_bSur15);
	//DDX_Text(pDX, IDC_SUR16, m_bSur16);
	//DDX_Text(pDX, IDC_SUR17, m_bSur17);
	//DDX_Text(pDX, IDC_SUR18, m_bSur18);

	DDX_Control(pDX, IDC_SUR1, m_ctrSUR1);
	DDX_Control(pDX, IDC_SUR2, m_ctrSUR2);
	DDX_Control(pDX, IDC_SUR3, m_ctrSUR3);
	DDX_Control(pDX, IDC_SUR4, m_ctrSUR4);
	DDX_Control(pDX, IDC_SUR5, m_ctrSUR5);
	DDX_Control(pDX, IDC_SUR6, m_ctrSUR6);
	DDX_Control(pDX, IDC_SUR7, m_ctrSUR7);
	DDX_Control(pDX, IDC_SUR8, m_ctrSUR8);
	DDX_Control(pDX, IDC_SUR9, m_ctrSUR9);
	DDX_Control(pDX, IDC_SUR10, m_ctrSUR10);
	DDX_Control(pDX, IDC_SUR11, m_ctrSUR11);
	DDX_Control(pDX, IDC_SUR12, m_ctrSUR12);
	DDX_Control(pDX, IDC_SUR13, m_ctrSUR13);
	DDX_Control(pDX, IDC_SUR14, m_ctrSUR14);
	DDX_Control(pDX, IDC_SUR15, m_ctrSUR15);
	DDX_Control(pDX, IDC_SUR16, m_ctrSUR16);
	DDX_Control(pDX, IDC_SUR17, m_ctrSUR17);
	DDX_Control(pDX, IDC_SUR18, m_ctrSUR18);
	DDX_Text(pDX, IDC_SURNUM, m_nSurNum);
}


BEGIN_MESSAGE_MAP(CControlFrame, CDialog)
	ON_BN_CLICKED(IDC_CASE1, &CControlFrame::OnBnClickedCase1)
	ON_BN_CLICKED(IDC_CASE2, &CControlFrame::OnBnClickedCase2)
	ON_BN_CLICKED(IDC_DISBLEND, &CControlFrame::OnBnClickedDisblend)
	ON_BN_CLICKED(IDC_ENBLEND, &CControlFrame::OnBnClickedEnblend)
	ON_BN_CLICKED(IDC_SUR1, &CControlFrame::OnBnClickedSur1)
	ON_BN_CLICKED(IDC_SUR2, &CControlFrame::OnBnClickedSur2)
	ON_BN_CLICKED(IDC_SUR3, &CControlFrame::OnBnClickedSur3)
	ON_BN_CLICKED(IDC_SUR4, &CControlFrame::OnBnClickedSur4)
	ON_BN_CLICKED(IDC_SUR5, &CControlFrame::OnBnClickedSur5)
	ON_BN_CLICKED(IDC_SUR6, &CControlFrame::OnBnClickedSur6)
	ON_BN_CLICKED(IDC_SUR7, &CControlFrame::OnBnClickedSur7)
	ON_BN_CLICKED(IDC_SUR8, &CControlFrame::OnBnClickedSur8)
	ON_BN_CLICKED(IDC_SUR9, &CControlFrame::OnBnClickedSur9)
	ON_BN_CLICKED(IDC_SUR10, &CControlFrame::OnBnClickedSur10)
	ON_BN_CLICKED(IDC_SUR11, &CControlFrame::OnBnClickedSur11)
	ON_BN_CLICKED(IDC_SUR12, &CControlFrame::OnBnClickedSur12)
	ON_BN_CLICKED(IDC_SUR13, &CControlFrame::OnBnClickedSur13)
	ON_BN_CLICKED(IDC_SUR14, &CControlFrame::OnBnClickedSur14)
	ON_BN_CLICKED(IDC_SUR15, &CControlFrame::OnBnClickedSur15)
	ON_BN_CLICKED(IDC_SUR16, &CControlFrame::OnBnClickedSur16)
	ON_BN_CLICKED(IDC_SUR17, &CControlFrame::OnBnClickedSur17)
	ON_BN_CLICKED(IDC_SUR18, &CControlFrame::OnBnClickedSur18)
END_MESSAGE_MAP()


BOOL CControlFrame::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pInterface = theApp.GetInterface();
	m_pScene = m_pInterface->GetScene();

	m_ctrSUR1.SetCheck(1);
	m_ctrSUR2.SetCheck(1);
	m_ctrSUR3.SetCheck(1);
	m_ctrSUR4.SetCheck(1);
	m_ctrSUR5.SetCheck(1);
	m_ctrSUR6.SetCheck(1);
	m_ctrSUR7.SetCheck(1);
	m_ctrSUR8.SetCheck(1);
	m_ctrSUR9.SetCheck(1);
	m_ctrSUR10.SetCheck(1);
	m_ctrSUR11.SetCheck(1);
	m_ctrSUR12.SetCheck(1);
	m_ctrSUR13.SetCheck(1);
	m_ctrSUR14.SetCheck(1);
	m_ctrSUR15.SetCheck(1);
	m_ctrSUR16.SetCheck(1);
	m_ctrSUR17.SetCheck(1);
	m_ctrSUR18.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CControlFrame::UpdateSurNum(int _num)
{
	m_nSurNum = _num;
	UpdateData(false);
}

// CControlFrame message handlers

void CControlFrame::OnBnClickedCase1()
{
	// TODO: Add your control notification handler code here
	m_pInterface->SetTest(CInterfaceManager::TEST1);
}

void CControlFrame::OnBnClickedCase2()
{
	// TODO: Add your control notification handler code here
	m_pInterface->SetTest(CInterfaceManager::TEST2);
}

void CControlFrame::OnBnClickedDisblend()
{
	// TODO: Add your control notification handler code here
	CSceneGL *pScene = m_pInterface->GetScene();
	pScene->SetBlendState(false);
	pScene->PaintGL();
}

void CControlFrame::OnBnClickedEnblend()
{
	// TODO: Add your control notification handler code here
	CSceneGL* pScene = m_pInterface->GetScene();
	pScene->SetBlendState(true);
	pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur1()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (0 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(0);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur2()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (1 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(1);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur3()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (2 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(2);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur4()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (3 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(3);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur5()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (4 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(4);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur6()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (5 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(5);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur7()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (6 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(6);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur8()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (7 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(7);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur9()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (8 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(8);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur10()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (9 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(9);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur11()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (10 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(10);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur12()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (11 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(11);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur13()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (12 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(12);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur14()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (13 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(13);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur15()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (14 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(14);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur16()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (15 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(15);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur17()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (16 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(16);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}

void CControlFrame::OnBnClickedSur18()
{
	// TODO: Add your control notification handler code here
	CSurface* pSur = m_pInterface->GetSurface();
	int RenderNum = pSur->GetSurNum();
	if (0 == RenderNum)
		return;

	if (17 < RenderNum)
	{
		RenderData* pRender = pSur->GetRender(17);
		pRender->SetSelect();
	}
	m_pScene->PaintGL();
}
