#include "stdafx.h"
#include "MarchingCubes.h"
#include "GLBuffer.h"

CMarchingCubes::CMarchingCubes(void):
		m_pdVolume(NULL), m_pdVoxelVerts(NULL), m_pdVoxelVertsScan(NULL),
		m_pdVoxelOccupied(NULL), m_pdVoxelOccupiedScan(NULL), m_pdCompactedVoxelArray(NULL),
		m_pdNumVertsTable(NULL), m_pdEdgeTable(NULL), m_pdTriTable(NULL)
{
}

CMarchingCubes::~CMarchingCubes(void)
{
	if (m_pdVolume)
		cutilSafeCall(cudaFree(m_pdVolume));
	cutilSafeCall(cudaFree(m_pdEdgeTable));
	cutilSafeCall(cudaFree(m_pdTriTable));
	cutilSafeCall(cudaFree(m_pdNumVertsTable));

	cutilSafeCall(cudaFree(m_pdVoxelVerts));
	cutilSafeCall(cudaFree(m_pdVoxelVertsScan));
	cutilSafeCall(cudaFree(m_pdVoxelOccupied));
	cutilSafeCall(cudaFree(m_pdVoxelOccupiedScan));
	cutilSafeCall(cudaFree(m_pdCompactedVoxelArray));

	cudppDestroyPlan(m_Scanplan);
	cudaThreadExit();
}

void CMarchingCubes::InitMC(int _width, int _height, int _depth, ElemType* _pVolume)
{
	// Data Array A[:, :, 1], A[:, :, 2], ... , A[:, : , n]
	//m_GridSize = make_uint3(_depth, _width, _height);
	m_GridSize = make_uint3(_width, _height, _depth);
	m_NumVoxels = m_GridSize.x * m_GridSize.y * m_GridSize.z;
	m_MaxVerts = m_GridSize.x * m_GridSize.y * 30;					// Num of MaxVerts need change
#ifdef _DEBUG
	printf("grids: %d * %d * %d = %d voxels\n", m_GridSize.x, m_GridSize.y, m_GridSize.z, m_NumVoxels);
#endif // _DEBUG

	// needed change
	int size = m_GridSize.x * m_GridSize.y * m_GridSize.z * sizeof(float);
	//////////////////////////////////////////////////////////////////////////
	int len = m_GridSize.x * m_GridSize.y * m_GridSize.z * 3;
	float *pVolTemp = new float[len];
	for (int i = 0; i < len; i++)
		pVolTemp[i] = _pVolume[i];
	//////////////////////////////////////////////////////////////////////////
	cutilSafeCall(cudaMalloc((void**) &m_pdVolume, size * 3));
	cutilSafeCall(cudaMemcpy(m_pdVolume, pVolTemp, size * 3, cudaMemcpyHostToDevice) );
	bindVolumeTexture(m_pdVolume);				// map the coordinates to the texture directly
	delete []pVolTemp;

	// allocate textures
	allocateTextures( &m_pdEdgeTable, &m_pdTriTable, &m_pdNumVertsTable );

	// allocate device memory
	unsigned int memSize = sizeof(uint) * m_NumVoxels;
	cutilSafeCall(cudaMalloc((void**) &m_pdVoxelVerts, memSize));
	cutilSafeCall(cudaMalloc((void**) &m_pdVoxelVertsScan, memSize));
	cutilSafeCall(cudaMalloc((void**) &m_pdVoxelOccupied, memSize));
	cutilSafeCall(cudaMalloc((void**) &m_pdVoxelOccupiedScan, memSize));
	cutilSafeCall(cudaMalloc((void**) &m_pdCompactedVoxelArray, memSize));

	// initialize CUDPP scan
	CUDPPConfiguration config;
	config.algorithm = CUDPP_SCAN;
	config.datatype = CUDPP_UINT;
	config.op = CUDPP_ADD;
	config.options = CUDPP_OPTION_FORWARD | CUDPP_OPTION_EXCLUSIVE;
	cudppPlan(&m_Scanplan, config, m_NumVoxels, 1, 0);
}

void CMarchingCubes::ComputeIsosurface(ElemType* _pFval, ElemType _isoValue, RenderData* _pRender)
{
	int threads = 128;
	dim3 grid(m_NumVoxels / threads, 1, 1);
	// get around maximum grid size of 65535 in each dimension
	if (grid.x > 65535) {
		grid.y = grid.x / 32768;
		grid.x = 32768;
	}

	uint totalVerts = 0;
	int size = m_GridSize.x * m_GridSize.y * m_GridSize.z * sizeof(float);
	//////////////////////////////////////////////////////////////////////////
	int len = m_GridSize.x * m_GridSize.y * m_GridSize.z;
	float *pFvalTemp = new float[len];
	for (int i = 0; i < len; i++)
	{
		pFvalTemp[i] = _pFval[i];
	}
	//////////////////////////////////////////////////////////////////////////
	float* pdVolumeFval;				// 顶点函数值纹理(n　Surface)
	cutilSafeCall(cudaMalloc((void**) &pdVolumeFval, size));
	cutilSafeCall(cudaMemcpy(pdVolumeFval, pFvalTemp, size, cudaMemcpyHostToDevice) );
	bindVolumeValTexture(pdVolumeFval);
	delete []pFvalTemp;

	// calculate number of vertices need per voxel
	launch_classifyVoxel(grid, threads, 
		m_pdVoxelVerts, m_pdVoxelOccupied, pdVolumeFval,
		m_GridSize,	m_NumVoxels, _isoValue);

#if DEBUG_BUFFERS
	printf("voxelVerts:\n");
	dumpBuffer(m_pdVoxelVerts, m_NumVoxels);
#endif

#if SKIP_EMPTY_VOXELS

	// scan voxel occupied array
	cudppScan(m_Scanplan, m_pdVoxelOccupiedScan, m_pdVoxelOccupied, m_NumVoxels);

#if DEBUG_BUFFERS
	printf("voxelOccupiedScan:\n");
	dumpBuffer(m_pdVoxelOccupiedScan, m_NumVoxels);
#endif

	// read back values to calculate total number of non-empty voxels
	// since we are using an exclusive scan, the total is the last value of
	// the scan result plus the last value in the input array
	{
		uint lastElement, lastScanElement;
		cutilSafeCall(cudaMemcpy((void *) &lastElement, 
			(void *) (m_pdVoxelOccupied + m_NumVoxels - 1), 
			sizeof(uint), cudaMemcpyDeviceToHost));
		cutilSafeCall(cudaMemcpy((void *) &lastScanElement, 
			(void *) (m_pdVoxelOccupiedScan + m_NumVoxels - 1), 
			sizeof(uint), cudaMemcpyDeviceToHost));
		m_ActiveVoxels = lastElement + lastScanElement;
	}

	if (0 == m_ActiveVoxels) {
		// return if there are no full voxels
		totalVerts = 0;
		return;
	}

	// compact voxel index array
	launch_compactVoxels(grid, threads, m_pdCompactedVoxelArray, m_pdVoxelOccupied, m_pdVoxelOccupiedScan, m_NumVoxels);
	cutilCheckMsg("compactVoxels failed");

#endif // SKIP_EMPTY_VOXELS

	// scan voxel vertex count array
	cudppScan(m_Scanplan, m_pdVoxelVertsScan, m_pdVoxelVerts, m_NumVoxels);
#if DEBUG_BUFFERS
	printf("voxelVertsScan:\n");
	dumpBuffer(m_pdVoxelVertsScan, m_NumVoxels);
#endif

	// readback total number of vertices
	{
		uint lastElement, lastScanElement;
		cutilSafeCall(cudaMemcpy((void *) &lastElement, 
			(void *) (m_pdVoxelVerts + m_NumVoxels - 1), 
			sizeof(uint), cudaMemcpyDeviceToHost));
		cutilSafeCall(cudaMemcpy((void *) &lastScanElement, 
			(void *) (m_pdVoxelVertsScan + m_NumVoxels - 1), 
			sizeof(uint), cudaMemcpyDeviceToHost));
		totalVerts = lastElement + lastScanElement;
	}

	// create VBOs
	GLuint	posVbo, normalVbo;
	createVBO(&posVbo, totalVerts * sizeof(float) * 4);
	cutilSafeCall(cudaGLRegisterBufferObject(posVbo));
	createVBO(&normalVbo, totalVerts * sizeof(float) * 4);
	cutilSafeCall(cudaGLRegisterBufferObject(normalVbo));

	// generate triangles, writing to vertex buffers
	float4 *d_pos = 0, *d_normal = 0;
	cutilSafeCall(cudaGLMapBufferObject((void**)&d_pos, posVbo));
	cutilSafeCall(cudaGLMapBufferObject((void**)&d_normal, normalVbo));

#if SKIP_EMPTY_VOXELS
	dim3 grid2((int) ceil(m_ActiveVoxels / (float) NTHREADS), 1, 1);
#else
	dim3 grid2((int) ceil(m_NumVoxels / (float) NTHREADS), 1, 1);
#endif
	while(grid2.x > 65535) {
		grid2.x/=2;
		grid2.y*=2;
	}

	launch_generateTriangles(grid2, NTHREADS, d_pos, d_normal, 
		m_pdCompactedVoxelArray, m_pdVoxelVertsScan,
		m_pdVolume, pdVolumeFval,
		m_GridSize, _isoValue,
		m_ActiveVoxels, m_MaxVerts);

	cutilSafeCall(cudaGLUnmapBufferObject(normalVbo));
	cutilSafeCall(cudaGLUnmapBufferObject(posVbo));

	_pRender->posVbo = posVbo;
	_pRender->normalVbo = normalVbo;
	_pRender->totalVerts = totalVerts;
	cutilSafeCall(cudaFree(pdVolumeFval));
}
