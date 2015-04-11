#ifndef _WSPLITTERWND_H
#define _WSPLITTERWND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxext.h"
#include <vector>
using namespace std;
#pragma warning(disable:4786)

#define LEFT_SIDE     0
#define RIGHT_SIDE    1
#define TOP_SIDE      LEFT_SIDE    
#define BOTTOM_SIDE   RIGHT_SIDE


class WSplitterWnd : public CSplitterWnd  
{
public:
	WSplitterWnd(int nLevel = 0);
	virtual ~WSplitterWnd();

	BOOL Create(CWnd* pParentWnd, CRuntimeClass* pView1, CRuntimeClass* pView2,
									  CCreateContext* pContext , BOOL bVertical = TRUE, int nID = AFX_IDW_PANE_FIRST);

	void SetInitialStatus(); // restore position and visibility
	inline void SetMinSize(int i, int x) {ASSERT(i >= 0 && i <= 2); m_nPaneMinSize[i] = x;};

    BOOL IsSideHidden(int nSide = 0);			// true if pane is hidden
    void ToggleSide(int row);					// Show or hide row of column
    void SetRegistryKey(LPCSTR p) {m_sRegKey = p;};

	int GetActivePaneNo();
    inline BOOL IsSplittverticaly() {return(m_bVertical);};

    WSplitterWnd* m_pSubSplitterWnd[2];			// 0=Left/Top, 1=Right/Bottom

    WSplitterWnd* AddSubDivision(int nSide, CRuntimeClass* pView1, CRuntimeClass* pView2,
									  CCreateContext* pContext,BOOL bVertical);

   /* support for switching views */
    int AddView(int nSide, CRuntimeClass * pViewClass, CCreateContext* pContext);
	void SwitchToView(int nSide,int nViewIX = -1);

protected:   
	
	inline void SideToRowCol(int nSide, int* nRow, int*  nCol) 
	{
		if (m_bVertical) {
			*nRow = 0;
			*nCol = nSide;
		}
		else {
			*nRow = nSide;
			*nCol = 0;
		}
	};

   void ShowRow();               // show row
   void HideRow(int rowHide);    // hide row
   void ShowColumn();            // show column
   void HideColumn(int colHide); // hide column

   void SaveToRegistry();
   void RememberSize();

   /* support for switching views */
	BOOL HideView(int nRow, int nCol);
	BOOL ShowView(int nRow, int nCol,CWnd* pWnd);

   int		m_nHiddenCol;   
   int		m_nHiddenRow; 
   int		m_nPaneSize[2];
   int		m_nPaneMinSize[2];
   BOOL		m_bVertical;
   CString  m_sRegKey;
   int		m_nLevel;

   /* support for switching views */
	vector<CWnd*>	m_views[2];
	int				m_nCurrentView[2]; // side as index

};

#endif // END _WSPLITTERWND_H
