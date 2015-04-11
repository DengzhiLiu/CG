// HumanAnimator.h : main header file for the HumanAnimator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Scene/InterfaceManager.h"

// CHumanAnimatorApp:
// See HumanAnimator.cpp for the implementation of this class
//

class CHumanAnimatorApp : public CWinApp
{
public:
	CHumanAnimatorApp();
	~CHumanAnimatorApp();

private:
	InterfaceManager *m_pInterface;

// Overrides
public:
	virtual BOOL InitInstance();
	InterfaceManager *GetInterface();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CHumanAnimatorApp theApp;