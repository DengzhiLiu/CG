#ifndef _MARCHING_CUBES_KERNEL_CU_
#define _MARCHING_CUBES_KERNEL_CU_

#include <stdio.h>
#include <string.h>
#include <cutil_inline.h>
#include <cuda_runtime_api.h>
#include "cutil_math.h"

#include "../SurfaceLib/common.h"
#include "tables.h"

// textures containing look-up tables
texture<uint, 1, cudaReadModeElementType> edgeTex;
texture<uint, 1, cudaReadModeElementType> triTex;
texture<uint, 1, cudaReadModeElementType> numVertsTex;

// volume data
texture<float, 1, cudaReadModeElementType> volumeTex;
texture<float, 1, cudaReadModeElementType> volumeFvalTex;

extern "C"
void allocateTextures(	uint **d_edgeTable, uint **d_triTable,  uint **d_numVertsTable )
{
    cutilSafeCall(cudaMalloc((void**) d_edgeTable, 256*sizeof(uint)));
    cutilSafeCall(cudaMemcpy((void *)*d_edgeTable, (void *)edgeTable, 256*sizeof(uint), cudaMemcpyHostToDevice) );
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindUnsigned);
    cutilSafeCall(cudaBindTexture(0, edgeTex, *d_edgeTable, channelDesc) );

    cutilSafeCall(cudaMalloc((void**) d_triTable, 256*16*sizeof(uint)));
    cutilSafeCall(cudaMemcpy((void *)*d_triTable, (void *)triTable, 256*16*sizeof(uint), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, triTex, *d_triTable, channelDesc) );

    cutilSafeCall(cudaMalloc((void**) d_numVertsTable, 256*sizeof(uint)));
    cutilSafeCall(cudaMemcpy((void *)*d_numVertsTable, (void *)numVertsTable, 256*sizeof(uint), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, numVertsTex, *d_numVertsTable, channelDesc) );
}

extern "C"
void bindVolumeTexture(float *d_volume)
{
    // bind to linear texture
    cutilSafeCall(cudaBindTexture(0, volumeTex, d_volume, cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat)));
}

extern "C"
void bindVolumeValTexture(float *d_volumeFval)
{
    // bind to linear texture
    cutilSafeCall(cudaBindTexture(0, volumeFvalTex, d_volumeFval, cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat)));
}

// an interesting field function
__device__
float tangle(float x, float y, float z)
{
    x *= 3.0f;
    y *= 3.0f;
    z *= 3.0f;
    return (x*x*x*x - 5.0f*x*x +y*y*y*y - 5.0f*y*y +z*z*z*z - 5.0f*z*z + 11.8f) * 0.2f + 0.5f;
}

// evaluate field function at point
__device__
float fieldFunc(float3 p)
{
    return tangle(p.x, p.y, p.z);
}

// evaluate field function at a point
// returns value and gradient in float4
__device__
float4 fieldFunc4(float3 p)
{
    float v = tangle(p.x, p.y, p.z);
    const float d = 0.001f;
    float dx = tangle(p.x + d, p.y, p.z) - v;
    float dy = tangle(p.x, p.y + d, p.z) - v;
    float dz = tangle(p.x, p.y, p.z + d) - v;
    return make_float4(dx, dy, dz, v);
}

// sample coordinates data set at a pointlist
__device__
float3 sampleVolume(float *data, uint3 p, uint3 gridSize)
{
    p.x = min(p.x, gridSize.x - 1);
    p.y = min(p.y, gridSize.y - 1);
    p.z = min(p.z, gridSize.z - 1);
    uint i = (p.z*gridSize.x*gridSize.y) + (p.y*gridSize.x) + p.x;
//    return (float) data[i] / 255.0f;
    return make_float3(tex1Dfetch(volumeTex, i*3), tex1Dfetch(volumeTex, i*3+1), tex1Dfetch(volumeTex, i*3+2));
}

// sample fval of volume data set at a point
__device__
float sampleVolumeFval(float *data, uint3 p, uint3 gridSize)
{
    p.x = min(p.x, gridSize.x - 1);
    p.y = min(p.y, gridSize.y - 1);
    p.z = min(p.z, gridSize.z - 1);
    uint i = (p.z*gridSize.x*gridSize.y) + (p.y*gridSize.x) + p.x;
//    return (float) data[i] / 255.0f;
    return tex1Dfetch(volumeFvalTex, i);
}

// classify voxel based on number of vertices it will generate
// one thread per voxel
__global__ void
classifyVoxel(uint* voxelVerts, uint* voxelOccupied, float* volumeFval, 
              uint3 gridSize, uint numVoxels, float isoValue)
{
    uint blockId = __mul24(blockIdx.y, gridDim.x) + blockIdx.x;
    uint i = __mul24(blockId, blockDim.x) + threadIdx.x;

	// Data Array A[:, :, 1], A[:, :, 2], ... , A[:, : , n], row first
	uint x = (i % (gridSize.x * gridSize.y)) % gridSize.x;
	uint y = (i % (gridSize.x * gridSize.y)) / gridSize.x;
	uint z = i / (gridSize.x * gridSize.y);
	//uint m = i % (gridSize.x * gridSize.y);
	//uint n = i / (gridSize.x * gridSize.y);
	uint3 gridPos = make_uint3(x, y, z);

    // read field values at neighbouring grid vertices
    float field[8];
    field[0] = sampleVolumeFval(volumeFval, gridPos, gridSize);
    field[1] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 0, 0), gridSize);
    field[2] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 1, 0), gridSize);
    field[3] = sampleVolumeFval(volumeFval, gridPos + make_uint3(0, 1, 0), gridSize);
    field[4] = sampleVolumeFval(volumeFval, gridPos + make_uint3(0, 0, 1), gridSize);
    field[5] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 0, 1), gridSize);
    field[6] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 1, 1), gridSize);
    field[7] = sampleVolumeFval(volumeFval, gridPos + make_uint3(0, 1, 1), gridSize);

    // calculate flag indicating if each vertex is inside or outside isosurface
    uint cubeindex;
	cubeindex =  uint(field[0] < isoValue); 
	cubeindex += uint(field[1] < isoValue)*2; 
	cubeindex += uint(field[2] < isoValue)*4; 
	cubeindex += uint(field[3] < isoValue)*8; 
	cubeindex += uint(field[4] < isoValue)*16; 
	cubeindex += uint(field[5] < isoValue)*32; 
	cubeindex += uint(field[6] < isoValue)*64; 
	cubeindex += uint(field[7] < isoValue)*128;

    // read number of vertices from texture
    uint numVerts = tex1Dfetch(numVertsTex, cubeindex);

    if (i < numVoxels) {
        voxelVerts[i] = numVerts;
        voxelOccupied[i] = (numVerts > 0);
    }
}

extern "C" void
launch_classifyVoxel( dim3 grid, dim3 threads, uint* voxelVerts, uint *voxelOccupied, float *volumeFval,
					 uint3 gridSize, uint numVoxels, float isoValue)
{
    // calculate number of vertices need per voxel
    classifyVoxel<<<grid, threads>>>(voxelVerts, voxelOccupied, volumeFval, 
                                     gridSize, numVoxels, isoValue);
    cutilCheckMsg("classifyVoxel failed");
}
              

// compact voxel array
__global__ void
compactVoxels(uint *compactedVoxelArray, uint *voxelOccupied, uint *voxelOccupiedScan, uint numVoxels)
{
    uint blockId = __mul24(blockIdx.y, gridDim.x) + blockIdx.x;
    uint i = __mul24(blockId, blockDim.x) + threadIdx.x;

    if (voxelOccupied[i] && (i < numVoxels)) {
        compactedVoxelArray[ voxelOccupiedScan[i] ] = i;
    }
}

extern "C" void 
launch_compactVoxels(dim3 grid, dim3 threads, uint *compactedVoxelArray, uint *voxelOccupied, uint *voxelOccupiedScan, uint numVoxels)
{
    compactVoxels<<<grid, threads>>>(compactedVoxelArray, voxelOccupied, 
                                     voxelOccupiedScan, numVoxels);
    cutilCheckMsg("compactVoxels failed");
}

// compute interpolated vertex along an edge
__device__
float3 vertexInterp(float isolevel, float3 p0, float3 p1, float f0, float f1)
{
    float t = (isolevel - f0) / (f1 - f0);
	return lerp(p0, p1, t);
} 

// calculate triangle normal
__device__
float3 calcNormal(float3 *v0, float3 *v1, float3 *v2)
{
    float3 edge0 = *v1 - *v0;
    float3 edge1 = *v2 - *v0;
    // note - it's faster to perform normalization in vertex shader rather than here
    return cross(edge0, edge1);
}

// version that calculates flat surface normal for each triangle
__global__ void
generateTriangles(float4 *pos, float4 *norm, uint *compactedVoxelArray, uint *numVertsScanned, float *volume,
                  float *volumeFval, uint3 gridSize, float isoValue, uint activeVoxels, uint maxVerts)
{
    uint blockId = __mul24(blockIdx.y, gridDim.x) + blockIdx.x;
    uint i = __mul24(blockId, blockDim.x) + threadIdx.x;

    if (i > activeVoxels - 1) {
        i = activeVoxels - 1;
    }

#if SKIP_EMPTY_VOXELS
    uint voxel = compactedVoxelArray[i];
#else
    uint voxel = i;
#endif

    // compute position in 3d grid
    uint x = (voxel % (gridSize.x * gridSize.y)) % gridSize.x;
	uint y = (voxel % (gridSize.x * gridSize.y)) / gridSize.x;
	uint z = voxel / (gridSize.x * gridSize.y);
	uint3 gridPos = make_uint3(x, y, z);

	// calculate cell vertex positions
    float3 v[8];
    v[0] = sampleVolume(volume, gridPos, gridSize);
    v[1] = sampleVolume(volume, gridPos + make_uint3(1, 0, 0), gridSize);
    v[2] = sampleVolume(volume, gridPos + make_uint3(1, 1, 0), gridSize);
    v[3] = sampleVolume(volume, gridPos + make_uint3(0, 1, 0), gridSize);
    v[4] = sampleVolume(volume, gridPos + make_uint3(0, 0, 1), gridSize);
    v[5] = sampleVolume(volume, gridPos + make_uint3(1, 0, 1), gridSize);
    v[6] = sampleVolume(volume, gridPos + make_uint3(1, 1, 1), gridSize);
    v[7] = sampleVolume(volume, gridPos + make_uint3(0, 1, 1), gridSize);

    float field[8];
    field[0] = sampleVolumeFval(volumeFval, gridPos, gridSize);
    field[1] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 0, 0), gridSize);
    field[2] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 1, 0), gridSize);
    field[3] = sampleVolumeFval(volumeFval, gridPos + make_uint3(0, 1, 0), gridSize);
    field[4] = sampleVolumeFval(volumeFval, gridPos + make_uint3(0, 0, 1), gridSize);
    field[5] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 0, 1), gridSize);
    field[6] = sampleVolumeFval(volumeFval, gridPos + make_uint3(1, 1, 1), gridSize);
    field[7] = sampleVolumeFval(volumeFval, gridPos + make_uint3(0, 1, 1), gridSize);

    // recalculate flag
    uint cubeindex;
	cubeindex =  uint(field[0] < isoValue); 
	cubeindex += uint(field[1] < isoValue)*2; 
	cubeindex += uint(field[2] < isoValue)*4; 
	cubeindex += uint(field[3] < isoValue)*8; 
	cubeindex += uint(field[4] < isoValue)*16; 
	cubeindex += uint(field[5] < isoValue)*32; 
	cubeindex += uint(field[6] < isoValue)*64; 
	cubeindex += uint(field[7] < isoValue)*128;

	// find the vertices where the surface intersects the cube 

#if USE_SHARED
    // use shared memory to avoid using local
	__shared__ float3 vertlist[12*NTHREADS];

	vertlist[threadIdx.x] = vertexInterp(isoValue, v[0], v[1], field[0], field[1]);
    vertlist[NTHREADS+threadIdx.x] = vertexInterp(isoValue, v[1], v[2], field[1], field[2]);
    vertlist[(NTHREADS*2)+threadIdx.x] = vertexInterp(isoValue, v[2], v[3], field[2], field[3]);
    vertlist[(NTHREADS*3)+threadIdx.x] = vertexInterp(isoValue, v[3], v[0], field[3], field[0]);
	vertlist[(NTHREADS*4)+threadIdx.x] = vertexInterp(isoValue, v[4], v[5], field[4], field[5]);
    vertlist[(NTHREADS*5)+threadIdx.x] = vertexInterp(isoValue, v[5], v[6], field[5], field[6]);
    vertlist[(NTHREADS*6)+threadIdx.x] = vertexInterp(isoValue, v[6], v[7], field[6], field[7]);
    vertlist[(NTHREADS*7)+threadIdx.x] = vertexInterp(isoValue, v[7], v[4], field[7], field[4]);
	vertlist[(NTHREADS*8)+threadIdx.x] = vertexInterp(isoValue, v[0], v[4], field[0], field[4]);
    vertlist[(NTHREADS*9)+threadIdx.x] = vertexInterp(isoValue, v[1], v[5], field[1], field[5]);
    vertlist[(NTHREADS*10)+threadIdx.x] = vertexInterp(isoValue, v[2], v[6], field[2], field[6]);
    vertlist[(NTHREADS*11)+threadIdx.x] = vertexInterp(isoValue, v[3], v[7], field[3], field[7]);
    __syncthreads();
#else

	float3 vertlist[12];

    vertlist[0] = vertexInterp(isoValue, v[0], v[1], field[0], field[1]);
    vertlist[1] = vertexInterp(isoValue, v[1], v[2], field[1], field[2]);
    vertlist[2] = vertexInterp(isoValue, v[2], v[3], field[2], field[3]);
    vertlist[3] = vertexInterp(isoValue, v[3], v[0], field[3], field[0]);

	vertlist[4] = vertexInterp(isoValue, v[4], v[5], field[4], field[5]);
    vertlist[5] = vertexInterp(isoValue, v[5], v[6], field[5], field[6]);
    vertlist[6] = vertexInterp(isoValue, v[6], v[7], field[6], field[7]);
    vertlist[7] = vertexInterp(isoValue, v[7], v[4], field[7], field[4]);

	vertlist[8] = vertexInterp(isoValue, v[0], v[4], field[0], field[4]);
    vertlist[9] = vertexInterp(isoValue, v[1], v[5], field[1], field[5]);
    vertlist[10] = vertexInterp(isoValue, v[2], v[6], field[2], field[6]);
    vertlist[11] = vertexInterp(isoValue, v[3], v[7], field[3], field[7]);
#endif

    // output triangle vertices
    uint numVerts = tex1Dfetch(numVertsTex, cubeindex);
    for(int i=0; i<numVerts; i+=3) {
        uint index = numVertsScanned[voxel] + i;

        float3 *v[3];
        uint edge;
        edge = tex1Dfetch(triTex, (cubeindex*16) + i);
#if USE_SHARED
        v[0] = &vertlist[(edge*NTHREADS)+threadIdx.x];
#else
        v[0] = &vertlist[edge];
#endif

        edge = tex1Dfetch(triTex, (cubeindex*16) + i + 1);
#if USE_SHARED
        v[1] = &vertlist[(edge*NTHREADS)+threadIdx.x];
#else
        v[1] = &vertlist[edge];
#endif

        edge = tex1Dfetch(triTex, (cubeindex*16) + i + 2);
#if USE_SHARED
        v[2] = &vertlist[(edge*NTHREADS)+threadIdx.x];
#else
        v[2] = &vertlist[edge];
#endif

        // calculate triangle surface normal
        float3 n = calcNormal(v[0], v[1], v[2]);

        if (index < (maxVerts - 3)) {
            pos[index] = make_float4(*v[0], 1.0f);
            norm[index] = make_float4(n, 0.0f);

            pos[index+1] = make_float4(*v[1], 1.0f);
            norm[index+1] = make_float4(n, 0.0f);

            pos[index+2] = make_float4(*v[2], 1.0f);
            norm[index+2] = make_float4(n, 0.0f);
        }
    }
}

extern "C" void
launch_generateTriangles(dim3 grid, dim3 threads,
						float4 *pos, float4 *norm, uint *compactedVoxelArray, uint *numVertsScanned, float *volume,
						float* volumeFval, uint3 gridSize, float isoValue, uint activeVoxels, uint maxVerts)
{
    generateTriangles<<<grid, NTHREADS>>>(pos, norm, 
                                          compactedVoxelArray, 
                                          numVertsScanned, volume, volumeFval,
                                          gridSize, isoValue, activeVoxels, 
                                          maxVerts);
    cutilCheckMsg("generateTriangles failed");
}

#endif
