#ifndef _RECFVAL_H
#define _RECFVAL_H

/**************************************************************************/
/* Binary file contains functions values evaluated at the Cartesian grid  */
/**************************************************************************/

#pragma once
#include "Common.h"
#include <vector>

using std::vector;

typedef struct _surface 
{
	int			order;
	int			type;
	int			nXYZ;
	ElemType	isoVal;
	ElemType	*pFval;
	
	_surface(): order(0), type(-1), nXYZ(0), isoVal(-1), pFval(NULL) 
	{}

	_surface(int _n): order(0), type(-1), nXYZ(_n), isoVal(-1)
	{
		pFval = new ElemType[_n];
	}
	
	_surface(const _surface &surf)
	{
		order = surf.order;
		type = surf.type;
		nXYZ = surf.nXYZ;
		isoVal = surf.isoVal;
		pFval = new ElemType[nXYZ];
		memmove(pFval, surf.pFval, sizeof(ElemType) * nXYZ);
	}

	_surface operator = (const _surface &surf)
	{
		order = surf.order;
		type = surf.type;
		nXYZ = surf.nXYZ;
		isoVal = surf.isoVal;
		pFval = new ElemType[nXYZ];
		memmove(pFval, surf.pFval, sizeof(ElemType) * nXYZ);

		return *this;
	}

	_surface* clone()
	{
		return new _surface(*this);
	}

	~_surface()
	{ 
		if (pFval)
			delete []pFval;
		pFval = NULL;
	}

}SURFACE;

typedef std::vector<SURFACE *>	SurfaceVec;

class CRecFval
{
public:
	CRecFval(void);
	~CRecFval(void);

	CRecFval(const CRecFval &fval);

	CRecFval operator = (const CRecFval &fval);

	bool		ReadFval(const char *_filename);

	void		PrintRecord();

	int			GetSurfaceNum() const { return m_iSurfaces; }

	SURFACE*    GetSurface(int _index) 
	{ 
		if (_index < 0 || _index >= m_iSurfaces)
			return NULL;
		
		return m_pSurfaces[_index];
	}

public:
	int			m_iSurfaces;
	int			m_iXYZ;
	SurfaceVec	m_pSurfaces;
};

#endif