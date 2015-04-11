#ifndef _MARCHINGCUBES_H
#define _MARCHINGCUBES_H

#pragma once
#include "Common.h"
#include <cudpp/cudpp.h>
#include <vector_types.h>
#include <vector_functions.h>

extern "C" void
launch_classifyVoxel( dim3 grid, dim3 threads, uint* voxelVerts, uint *voxelOccupied, float *volumeFval,
					 uint3 gridSize, uint numVoxels, float isoValue);

extern "C" void 
launch_compactVoxels(dim3 grid, dim3 threads, uint *compactedVoxelArray, uint *voxelOccupied, 
					 uint *voxelOccupiedScan, uint numVoxels);

extern "C" void
launch_generateTriangles(dim3 grid, dim3 threads,
						  float4 *pos, float4 *norm, uint *compactedVoxelArray, 
						  uint *numVertsScanned, float *volume, float *volumeFval,
						  uint3 gridSize, float isoValue, uint activeVoxels, uint maxVerts);

extern "C" void
bindVolumeTexture(float *d_volume);

extern "C" void
bindVolumeValTexture(float *d_volumeFval);

extern "C" void
allocateTextures(	uint **d_edgeTable, uint **d_triTable,  uint **d_numVertsTable );

extern "C" void
createVBO(GLuint* vbo, unsigned int size);

extern "C" void
deleteVBO(GLuint* vbo);

extern "C" void
dumpBuffer(uint *d_buffer, int nelements);

extern "C" void
dumpFBuffer(float *d_buffer, int nelements);

// for render
typedef struct _RenderNode
{
	_RenderNode(): selected(true), totalVerts(0), 
		posVbo(-1), normalVbo(-1)
	{}

	bool		selected;
	uint		totalVerts;

	// device data
	GLuint		posVbo, normalVbo;

	void		SetSelect() { selected = !selected; }
} RenderData;

class CMarchingCubes
{
public:
	CMarchingCubes(void);
	~CMarchingCubes(void);

	void		InitMC(int _width, int _height, int _depth, ElemType* _pVolume);

	void		ComputeIsosurface(ElemType* _pFval, ElemType _isoValue, RenderData* _pRender);

public:
	uint		m_NumVoxels;
	uint		m_MaxVerts;
	uint3		m_GridSize;
	uint		m_ActiveVoxels;
	
	float*		m_pdVolume;					// ¶¥µã×ø±êÎÆÀí
	CUDPPHandle m_Scanplan;

	uint*		m_pdVoxelVerts;
	uint*		m_pdVoxelVertsScan;
	uint*		m_pdVoxelOccupied;
	uint*		m_pdVoxelOccupiedScan;
	uint*		m_pdCompactedVoxelArray;

	// tables
	uint*		m_pdNumVertsTable;
	uint*		m_pdEdgeTable;
	uint*		m_pdTriTable;

};

#endif
