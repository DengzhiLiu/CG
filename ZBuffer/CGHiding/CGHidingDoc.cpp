// CGHidingDoc.cpp : implementation of the CCGHidingDoc class
//

#include "stdafx.h"
#include "CGHiding.h"

#include "MainFrm.h"
#include "CGHidingDoc.h"
#include "CGHidingView.h"
#include "InteractView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCGHidingDoc

IMPLEMENT_DYNCREATE(CCGHidingDoc, CDocument)

BEGIN_MESSAGE_MAP(CCGHidingDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CCGHidingDoc::OnFileOpen)
END_MESSAGE_MAP()


// CCGHidingDoc construction/destruction

CCGHidingDoc::CCGHidingDoc()
{
	// TODO: add one-time construction code here
}

CCGHidingDoc::~CCGHidingDoc()
{
}

BOOL CCGHidingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CCGHidingDoc serialization

void CCGHidingDoc::Serialize(CArchive& ar)
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


// CCGHidingDoc diagnostics

#ifdef _DEBUG
void CCGHidingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCGHidingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

//////////////////////////////////////////////////////////////////////////
//////	members
//////////////////////////////////////////////////////////////////////////
const CString CCGHidingDoc::L_Filter = _T
			("Obj Files (*.obj)|*.obj|3DS Files \(*.3ds)|*.3ds|All Files (*.*)|*.*||");
const CString CCGHidingDoc::S_Filter = _T("Obj Files (*.obj)|*.obj||");
// CCGHidingDoc commands

void CCGHidingDoc::OnFileOpen()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		S_Filter);

	static CString szTitle = "Open Scene Files";
	dlg.m_ofn.lpstrTitle = szTitle;

	m_3DModel.numOfObjects = 0;
	if (dlg.DoModal() == IDOK) {
		float object_scale = 0;
		CFVector3 object_center;
		int vertex = 0, face = 0; 
		char str[10];
		CInteractView *pInterView = get_i_view();
		m_strFilePathName = dlg.GetPathName();
		m_ObjLoader.ImportObj(&m_3DModel, m_strFilePathName);			// 将obj文件装入到模型结构体中
		m_ObjLoader.GetObjInfo(m_3DModel, object_center, object_scale);
 		g_OpenGL.SetObjInfo(object_center, object_scale);
 		g_OpenGL.FocusObject();
		for (int i = 0; i < m_3DModel.numOfObjects; i++)
		{
			vertex += m_3DModel.pObject[i].numOfVerts;
			face += m_3DModel.pObject[i].numOfFaces;
		}
		itoa(vertex, str, 10);
		pInterView->m_vertex_num.SetWindowText(str);
		itoa(face, str, 10);
		pInterView->m_face_num.SetWindowText(str);
	}
	else {
		//AfxMessageBox("Can't Open .obj File!");
	}
	UpdateAllViews(NULL);
}

BOOL	CCGHidingDoc::IsValid()
{
	if (m_3DModel.numOfObjects > 0)
		return	TRUE;
	return	FALSE;
}

// Get the main view for short
CCGHidingView* CCGHidingDoc::get_cg_view() const
{
	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	return (CCGHidingView *)(pFrame->m_wndSplitter.GetPane(0, 0));
}
//

// Get the interact view for short
CInteractView* CCGHidingDoc::get_i_view() const
{
	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	return (CInteractView *)(pFrame->m_wndSplitter.GetPane(0, 1));

}
//