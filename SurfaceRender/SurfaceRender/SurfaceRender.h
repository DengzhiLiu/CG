// SurfaceRender.h : main header file for the SurfaceRender application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "InterfaceManager.h"

// CSurfaceRenderApp:
// See SurfaceRender.cpp for the implementation of this class
//

class CSurfaceRenderApp : public CWinApp
{
public:
	CSurfaceRenderApp();
	~CSurfaceRenderApp();

private:
	CInterfaceManager *m_pInterface;

// Overrides
public:
	virtual BOOL InitInstance();

	CInterfaceManager* GetInterface() { return m_pInterface; }

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSurfaceRenderApp theApp;