#ifndef _RECCOORD_H
#define _RECCOORD_H

/**************************************************************************/
/*			Binary file contains coordinates of a Cartesian grid		  */
/**************************************************************************/

#pragma once
#include "Common.h"
#include "Vector3.h"

class CRecCoord
{
public:
	CRecCoord(void);
	~CRecCoord(void);

	CRecCoord(const CRecCoord& coord);

	CRecCoord operator = (const CRecCoord &coord);

	bool	ReadCoord(const char *_filename);

	void	ComputeBoundBox();

	void	PrintRecord();

	void	PrintCube();

	int		GetWidth() { return m_iX; }

	int		GetHeight() { return m_iY; }

	int		GetDepth() { return m_iZ; }

	ElemType* GetCoords() const { return m_pCoords; }

	ElemType GetScale() const { return m_dScale; }

	Vector3  GetCenter() const { return m_Center; }

public:
	int			m_iX, m_iY, m_iZ;
	ElemType	*m_pCoords;

	ElemType	m_dScale;
	Vector3		m_Center;
	Vector3		m_MinBound, m_MaxBound;
};

#endif