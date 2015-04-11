#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"
#include "../Scene/InterfaceManager.h"

// CControlFrame dialog

class CControlFrame : public CDialog
{
	DECLARE_DYNAMIC(CControlFrame)

public:
	CControlFrame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlFrame();

// Dialog Data
	enum { IDD = IDD_CONTROL };

private:
	InterfaceManager* m_pInterface;

public:
	bool		m_bPlay;
	int			m_FrameNo;
	int			m_FrameNum;
	int			m_VertexNum;
	int			m_FaceNum;
	CString		m_MeshName;
	CString		m_MotionName;
	CSliderCtrl m_FrameSlider;
	float		m_TimeInterval;
	CString		m_FPSDisp;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void SetRange(int );
	void SetFrame(int );
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
	afx_msg void OnDeltaposSpinFps(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedPrev();	
	afx_msg void OnBnClickedInsert();
};
