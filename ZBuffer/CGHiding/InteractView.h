#pragma once
#include "afxwin.h"



// CInteractView form view

class CInteractView : public CFormView
{
	DECLARE_DYNCREATE(CInteractView)

protected:
	CInteractView();           // protected constructor used by dynamic creation
	virtual ~CInteractView();

public:
	enum { IDD = IDD_FORMVIEW_INTERACT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_vertex_num;
	CStatic m_face_num;

	int m_light_type;
	int m_color_type;
	int m_render_type;
public:
	afx_msg void OnBnClickedRedraw();
};


