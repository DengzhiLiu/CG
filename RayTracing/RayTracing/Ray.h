#ifndef _RAY_H
#define _RAY_H

#include "Vector.h"
#include "Point.h"
#include "Definitions.h"

class Object;
class Primitive;

class Ray
{
public:
	Point  orig;
	Vector dir;
	u_int depth;
	double contribution;
	double hitDist;			// 相交参数t
	Primitive *hitObj;

public:
    // Set up the ray with a give origin and direction (and optionally a ray depth)
    Ray( Point orig, Vector dir, u_int depth=0, double contribution=1.0);
    ~Ray();

    // Accessor functions to access the origin, direction, and depth
    inline Point GetOrigin( void ) const { return orig; }
    inline Vector GetDirection( void ) const { return dir; }
    inline u_int GetDepth( void ) const { return depth; }
	inline double GetContribution( void ) const { return contribution; };

    // Given a distance t, this computes the point on the ray that distance from the origin
    inline Point PointOnRay( double t ) const { return orig + t*dir; }

    // Call this method when you hit an object obj at a distance t from the ray origin.
    //    It tests to see if t is the closest object hit yet, and if so remembers it.
    void CheckHit( double t, Primitive *obj );

    // Gets the hit point and/or object.  If no point was hit, the HitObject is NULL.
    inline Point GetHitPoint( void ) const { return PointOnRay( hitDist ); }
    inline Primitive *GetHitObject( void ) { return hitObj; }

    // Checks if there has been an intersection along this ray.
    inline bool WasIntersection( void ) const { return hitObj != 0; }

};

#endif
