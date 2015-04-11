#ifndef _GLBUFFER_H
#define _GLBUFFER_H

#include <cutil_inline.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

////////////////////////////////////////////////////////////////////////////////
//! Create VBO
////////////////////////////////////////////////////////////////////////////////
extern "C" void
createVBO(GLuint* vbo, unsigned int size)
{
	// create buffer object
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);

	// initialize buffer object
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glutReportErrors();
}

////////////////////////////////////////////////////////////////////////////////
//! Delete VBO
////////////////////////////////////////////////////////////////////////////////
extern "C" void
deleteVBO(GLuint* vbo)
{
	glBindBuffer(1, *vbo);
	glDeleteBuffers(1, vbo);
	cutilSafeCall(cudaGLUnregisterBufferObject(*vbo));

	*vbo = 0;
}

extern "C" void
dumpBuffer(uint *d_buffer, int nelements)
{
	uint bytes = nelements*sizeof(uint);
	uint *h_buffer = (uint *) malloc(bytes);
	cutilSafeCall(cudaMemcpy(h_buffer, d_buffer, bytes, cudaMemcpyDeviceToHost) );
	for(int i=0; i<nelements; i++) {
		printf("%d: %u\n", i, h_buffer[i]);
	}
	printf("\n");
	free(h_buffer);
}

extern "C" void
dumpFBuffer(float *d_buffer, int nelements)
{
	uint bytes = nelements*sizeof(float);
	float *h_buffer = (float *) malloc(bytes);
	cutilSafeCall(cudaMemcpy(h_buffer, d_buffer, bytes, cudaMemcpyDeviceToHost) );
	for(int i=0; i<nelements; i++) {
		printf("%d: %f\n", i, h_buffer[i]);
	}
	printf("\n");
	free(h_buffer);
}

#endif