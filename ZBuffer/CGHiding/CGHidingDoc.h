// CGHidingDoc.h : interface of the CCGHidingDoc class
//


#pragma once

#include "OpenGL.h"
#include "Vector3.h"
#include "ObjInfo.h"
#include "ObjLoader.h"

class CCGHidingView;
class CInteractView;

class CCGHidingDoc : public CDocument
{
protected: // create from serialization only
	CCGHidingDoc();
	DECLARE_DYNCREATE(CCGHidingDoc)

// Attributes
public:
	CObjLoader	m_ObjLoader;									
	t3DModel	m_3DModel;	

private:
	const static CString L_Filter;
	const static CString S_Filter;
	CString	m_strFilePathName;

// Operations
public:
	CCGHidingView* get_cg_view() const;
	CInteractView* get_i_view() const;

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CCGHidingDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	BOOL	IsValid();
};


