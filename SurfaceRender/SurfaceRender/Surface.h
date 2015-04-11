#ifndef _SURFACE_H
#define _SURFACE_H

/**************************************************************************/
/*								Data for the surfaces					  */
/**************************************************************************/

#pragma once
#include "RecCoord.h"
#include "RecFval.h"
#include "RecPartition.h"
#include "MarchingCubes.h"
#include <vector>

using std::vector;
typedef vector<RenderData *> RenderVec;

//extern "C" void
//deleteVBO(GLuint* vbo);

class CSurface
{
public:
	CSurface(void);
	~CSurface(void);

	void		ImportCoordData(const char *_filename);

	void		ImportFvalData(const char *_filename);

	void		ImportPartData(const char *_filename);

	void		RenderCoords();

	void		RenderSurfaces();

	void		ComputeSurface(int _case);

	RenderData*	GetRender(int _i) const { return m_pSurfRender[_i]; }

	int			GetSurNum() const { return (int)m_pSurfRender.size(); }

	const CRecCoord& GetCoordHandle() const { return m_RecCoord; }

	const CRecFval&	GetFvalHandle() const { return m_RecFval; }

	const CRecPartition& GetPartitionHandle() const { return m_RecPart; }	

public:
	CRecCoord		m_RecCoord;
	CRecFval		m_RecFval;
	CRecPartition	m_RecPart;

	RenderVec		m_pSurfRender;
	CMarchingCubes	m_CubeTool;
};


static float colorTable[20][4] = {
	{1.00, 0.00, 0.00, 0.50},
	{0.61, 0.40, 0.12, 0.50},
	{1.00, 0.40, 0.31, 0.50},
	{0.69, 0.19, 0.38, 0.50},
	{1.00, 0.00, 1.00, 0.50},
	{1.00, 1.00, 0.00, 0.50},
	{0.00, 0.00, 1.00, 0.50},
	{0.10, 0.10, 0.44, 0.50},
	{0.20, 0.63, 0.79, 0.50},
	{0.00, 1.00, 1.00, 0.50},
	{0.50, 1.00, 0.83, 0.50},
	{0.13, 0.55, 0.13, 0.50},
	{0.63, 0.13, 0.94, 0.50},
	{0.85, 0.44, 0.84, 0.50},
	{1.00, 0.38, 0.00, 0.50},
	{0.96, 0.87, 0.70, 0.50},
	{0.50, 0.16, 0.16, 0.50},
	{0.82, 0.41, 0.12, 0.50},
	{0.00, 0.78, 0.55, 0.50},
	{0.00, 1.00, 0.00, 0.50}
};


#endif
