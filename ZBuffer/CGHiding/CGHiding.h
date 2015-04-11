// CGHiding.h : main header file for the CGHiding application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CCGHidingApp:
// See CGHiding.cpp for the implementation of this class
//

class CCGHidingApp : public CWinApp
{
public:
	CCGHidingApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCGHidingApp theApp;