// MainFrm.h : interface of the CMainFrame class
//
#include "WSplitterWnd.h"
#include "HumanAnimatorView.h"
#include "ControlFrame.h"
#include "../Scene/InterfaceManager.h"

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void UpdateFrame();
	void ShutTimer();
	void ViewSynch();
	void ToggleTimer(float interval);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CControlFrame*	m_pControlFrame;

protected:  // control bar embedded members
	CImageList		m_ilTB;
	CToolBar		m_wndToolBar;
	CStatusBar		m_wndStatusBar;
	WSplitterWnd	m_wndSplitter;

private:
	const static CString ALL_Filter;
	const static CString OBJ_Filter;
	const static CString MOTION_Filter;
	const static CString WEIGHT_Filter;
	const static CString BVH_Filter;

	int				  m_RenderType;
	InterfaceManager *m_pInterface;
	CHumanAnimatorView *view[2];

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileReload();
	afx_msg void OnFileImportmotion();
	afx_msg void OnFileImportobj();
	afx_msg void OnFileImportweight();
	afx_msg void OnDrawFill();
	afx_msg void OnDrawFillwire();
	afx_msg void OnDrawHidden();
	afx_msg void OnDrawPoints();
	afx_msg void OnDrawWire();
	afx_msg void OnShadeFlat();
	afx_msg void OnShadeSmooth();
	afx_msg void OnShadeTexture();
	afx_msg void OnRenderMesh();
	afx_msg void OnRenderSkeleton();
	afx_msg void OnWinControl();
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	afx_msg void OnReset();
	afx_msg void OnDrawFloor();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRenderCpu();
	afx_msg void OnRenderGpu();
	afx_msg void OnUpdateRenderCpu(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRenderGpu(CCmdUI *pCmdUI);
	afx_msg void OnFileExportmotion();
};


