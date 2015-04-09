#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

typedef unsigned int u_int;
typedef unsigned char u_char;

// The maximum possible value for a 32-bit doubleing point variable
#ifndef  INFINITY
#define  INFINITY   ((double)3.40282347e+38)
#endif

// The value of pi, which seems to be undefined on most Windows systems
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define AXISX	0
#define AXISY	1
#define AXISZ	2

// When ray tracing, the epsilon that t > EPSILON in order to avoid self intersections
#define EPSILON       1e-5
#define MAXCOORD	  1000000
#define MINCOORD	  -1000000

#define MIN(a, b) (a < b? a : b)
#define MAX(a, b) (a > b? a : b)


inline int Log2Int(float v) {
	return ((*(int *) &v) >> 23) - 127;
}

#endif
