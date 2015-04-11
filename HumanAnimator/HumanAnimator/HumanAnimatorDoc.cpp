// HumanAnimatorDoc.cpp : implementation of the CHumanAnimatorDoc class
//

#include "stdafx.h"
#include "HumanAnimator.h"

#include "HumanAnimatorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHumanAnimatorDoc

IMPLEMENT_DYNCREATE(CHumanAnimatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CHumanAnimatorDoc, CDocument)
END_MESSAGE_MAP()


// CHumanAnimatorDoc construction/destruction

CHumanAnimatorDoc::CHumanAnimatorDoc()
{
	// TODO: add one-time construction code here

}

CHumanAnimatorDoc::~CHumanAnimatorDoc()
{
}

BOOL CHumanAnimatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CHumanAnimatorDoc serialization

void CHumanAnimatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CHumanAnimatorDoc diagnostics

#ifdef _DEBUG
void CHumanAnimatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHumanAnimatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CHumanAnimatorDoc commands
