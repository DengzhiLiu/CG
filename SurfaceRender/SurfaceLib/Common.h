#ifndef _COMMON_H
#define _COMMON_H

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

//typedef float ElemType;
typedef double ElemType;
typedef unsigned int uint;
typedef unsigned char uchar;

// Using shared to store computed vertices and normals during triangle generation
// improves performance
#define USE_SHARED 1

// The number of threads to use for triangle generation (limited by shared memory size)
#define NTHREADS 32

#define SKIP_EMPTY_VOXELS 1

#define DEBUG_BUFFERS 0

#ifndef PI
#define PI 3.1415926
#endif

#ifndef EPS
#define EPS 1e-6
#endif

#ifndef MINVAL
#define MINVAL -1e9
#endif

#ifndef MAXVAL
#define MAXVAL 1e9
#endif

#endif