#ifndef _RECPATITION_H
#define _RECPATITION_H

/**************************************************************************/
/*		Binary file contains region indices on  the Cartesian grid		 */
/**************************************************************************/

#pragma once
#include "Common.h"

class CRecPartition
{
public:
	CRecPartition(void);
	~CRecPartition(void);

	CRecPartition(const CRecPartition &rec);

	CRecPartition operator = (const CRecPartition &rec);

	bool	ReadPartition(const char *_filename);

	void	ConvertISO(int _index);

	void	PrintRecord();

	int*	GetIndices() const { return m_piIndex; }

	ElemType* GetValue() const { return m_pValue; }

	int		GetMinIndex() const { return m_iMinIndex; }

	int		GetMaxIndex() const { return m_iMaxIndex; }

public:
	int			m_iXYZ;
	int*		m_piIndex;
	ElemType*	m_pValue;
	int			m_iMinIndex, m_iMaxIndex;
};

#endif
