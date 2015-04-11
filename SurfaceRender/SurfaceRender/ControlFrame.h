
#pragma once

#include "resource.h"
#include "SceneGL.h"
#include "InterfaceManager.h"
#include "afxwin.h"


// CControlFrame dialog

class CControlFrame : public CDialog
{
	DECLARE_DYNAMIC(CControlFrame)

public:
	CControlFrame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlFrame();


// Dialog Data
	enum { IDD = IDD_CONTROL };

public:
	//bool m_bSur1, m_bSur2, m_bSur3, m_bSur4;
	//bool m_bSur5, m_bSur6, m_bSur7, m_bSur8;
	//bool m_bSur9, m_bSur10, m_bSur11, m_bSur12;
	//bool m_bSur13, m_bSur14, m_bSur15, m_bSur16;
	//bool m_bSur17, m_bSur18;

	CButton m_ctrSUR1, m_ctrSUR2, m_ctrSUR3;
	CButton m_ctrSUR4, m_ctrSUR5, m_ctrSUR6;
	CButton m_ctrSUR7, m_ctrSUR8, m_ctrSUR9;
	CButton m_ctrSUR10, m_ctrSUR11, m_ctrSUR12;
	CButton m_ctrSUR13, m_ctrSUR14, m_ctrSUR15;
	CButton m_ctrSUR16, m_ctrSUR17, m_ctrSUR18;

	int					m_nSurNum;
	CSceneGL*			m_pScene;
	CInterfaceManager*	m_pInterface;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void	UpdateSurNum(int _num);
	afx_msg void OnBnClickedCase1();
	afx_msg void OnBnClickedCase2();
	afx_msg void OnBnClickedDisblend();
	afx_msg void OnBnClickedEnblend();

	afx_msg void OnBnClickedSur1();
	afx_msg void OnBnClickedSur2();
	afx_msg void OnBnClickedSur3();
	afx_msg void OnBnClickedSur4();
	afx_msg void OnBnClickedSur5();
	afx_msg void OnBnClickedSur6();
	afx_msg void OnBnClickedSur7();
	afx_msg void OnBnClickedSur8();
	afx_msg void OnBnClickedSur9();
	afx_msg void OnBnClickedSur10();
	afx_msg void OnBnClickedSur11();
	afx_msg void OnBnClickedSur12();
	afx_msg void OnBnClickedSur13();
	afx_msg void OnBnClickedSur14();
	afx_msg void OnBnClickedSur15();
	afx_msg void OnBnClickedSur16();
	afx_msg void OnBnClickedSur17();
	afx_msg void OnBnClickedSur18();
	
};
