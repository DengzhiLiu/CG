// SurfaceRenderDoc.h : interface of the CSurfaceRenderDoc class
//


#pragma once


class CSurfaceRenderDoc : public CDocument
{
protected: // create from serialization only
	CSurfaceRenderDoc();
	DECLARE_DYNCREATE(CSurfaceRenderDoc)

// Attributes
private:
	const static CString ALL_Filter;
	const static CString DAT_Filter;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CSurfaceRenderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileImportcoords();
	afx_msg void OnFileImportfvals();
	afx_msg void OnFileImportpartition();
	afx_msg void OnSurfaceabstract();
};


