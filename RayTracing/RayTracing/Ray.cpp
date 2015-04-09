#include "Ray.h"
#include "Definitions.h"

Ray::Ray( Point orig, Vector dir, u_int depth, double contribution) :
        orig(orig), dir(dir), depth(depth), contribution(contribution)
{
	hitDist = INFINITY;
    hitObj = 0;
}

Ray::~Ray()
{
}


void Ray::CheckHit( double t, Primitive *obj )
{
    if (t < hitDist && t > EPSILON) {
            hitDist = t;
            hitObj = obj;
    }
}

