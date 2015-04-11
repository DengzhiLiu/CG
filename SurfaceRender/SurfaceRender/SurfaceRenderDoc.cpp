// SurfaceRenderDoc.cpp : implementation of the CSurfaceRenderDoc class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "SurfaceRender.h"

#include "SurfaceRenderDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSurfaceRenderDoc

IMPLEMENT_DYNCREATE(CSurfaceRenderDoc, CDocument)

BEGIN_MESSAGE_MAP(CSurfaceRenderDoc, CDocument)
	ON_COMMAND(ID_FILE_IMPORTCOORDS, &CSurfaceRenderDoc::OnFileImportcoords)
	ON_COMMAND(ID_FILE_IMPORTFVALS, &CSurfaceRenderDoc::OnFileImportfvals)
	ON_COMMAND(ID_FILE_IMPORTPARTITION, &CSurfaceRenderDoc::OnFileImportpartition)
	ON_COMMAND(ID_SURFACEABSTRACT, &CSurfaceRenderDoc::OnSurfaceabstract)
END_MESSAGE_MAP()


// CSurfaceRenderDoc construction/destruction

CSurfaceRenderDoc::CSurfaceRenderDoc()
{
	// TODO: add one-time construction code here

}

CSurfaceRenderDoc::~CSurfaceRenderDoc()
{
}

BOOL CSurfaceRenderDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CSurfaceRenderDoc serialization

void CSurfaceRenderDoc::Serialize(CArchive& ar)
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


// CSurfaceRenderDoc diagnostics

#ifdef _DEBUG
void CSurfaceRenderDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSurfaceRenderDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

const CString CSurfaceRenderDoc::ALL_Filter = _T("All File|*.*||");
const CString CSurfaceRenderDoc::DAT_Filter = _T("DAT Files (*.dat)|*.dat||");

// CSurfaceRenderDoc commands

void CSurfaceRenderDoc::OnFileImportcoords()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		DAT_Filter);
	dlg.m_ofn.lpstrTitle = "Open DAT File";

	CString fileName;
	CInterfaceManager* pInterface = theApp.GetInterface();
	CSurface* pSurface = pInterface->GetSurface();

	if (dlg.DoModal() == IDOK) {
		//fileName = dlg.GetFileName();
		fileName = dlg.GetPathName();
		pSurface->ImportCoordData(fileName.GetBuffer());
		fileName.ReleaseBuffer();
	}
	else {
		TRACE("Can't Open %s File!", fileName);
		return ;
	}
}

void CSurfaceRenderDoc::OnFileImportfvals()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		DAT_Filter);
	dlg.m_ofn.lpstrTitle = "Open DAT File";

	CString fileName;
	CInterfaceManager* pInterface = theApp.GetInterface();
	CSurface* pSurface = pInterface->GetSurface();

	if (dlg.DoModal() == IDOK) {
		//fileName = dlg.GetFileName();
		fileName = dlg.GetPathName();
		pSurface->ImportFvalData(fileName.GetBuffer());
		fileName.ReleaseBuffer();
	}
	else {
		TRACE("Can't Open %s File!", fileName);
		return ;
	}
}

void CSurfaceRenderDoc::OnFileImportpartition()
{
	// TODO: Add your command handler code here
	CFileDialog dlg (TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		DAT_Filter);
	dlg.m_ofn.lpstrTitle = "Open DAT File";

	CString fileName;
	CInterfaceManager* pInterface = theApp.GetInterface();
	CSurface* pSurface = pInterface->GetSurface();

	if (dlg.DoModal() == IDOK) {
		//fileName = dlg.GetFileName();
		fileName = dlg.GetPathName();
		pSurface->ImportPartData(fileName.GetBuffer());
		fileName.ReleaseBuffer();
	}
	else {
		TRACE("Can't Open %s File!", fileName);
		return ;
	}
}

void CSurfaceRenderDoc::OnSurfaceabstract()
{
	// TODO: Add your command handler code here
	CInterfaceManager* pInterface = theApp.GetInterface();
	pInterface->Process();

	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->m_pControlFrame->UpdateSurNum(pInterface->GetSurface()->GetSurNum());
}
