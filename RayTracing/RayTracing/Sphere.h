#ifndef _SPHERE_H
#define _SPHERE_H

#include "Primitive.h"
#include "Point.h"
#include "Ray.h"
#include "Vector.h"
#include "Color.h"
#include "Dib.h"

class Material;

class Sphere : public Primitive {
public:
	CDIB* p_bitmap;
	Point center;
	float radius;

public:
        Sphere( Material *matl, CDIB *bitmap, const Point &center, float radius );
        virtual ~Sphere();
        virtual bool Intersect( Ray &ray, double &tmin, double &tmax );
		virtual	void Texture(Ray &ray, Color &diffuse);
        virtual Vector ComputeNormal( Ray &ray ) const;
		virtual void CalculateCloseBox();

        inline Point GetCenter( void ) const { return center; }
        inline float GetRadius( void ) const { return radius; }
};

#endif
