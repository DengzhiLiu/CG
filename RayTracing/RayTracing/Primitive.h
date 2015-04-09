#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

#include "Object.h"
#include "Color.h"

class Ray;
class Material;
class Vector;

class Primitive : public Object {
public:
        Primitive(Material *matl) : matl(matl) {}
        virtual ~Primitive() { /*if (matl)	delete matl;*/ }
        virtual bool Intersect( Ray &ray, double &tmin, double &tmax ) = 0;
		virtual void Texture(Ray &ray, Color &diffuse) = 0;
        virtual Vector ComputeNormal( Ray &ray ) const = 0;
		virtual void CalculateCloseBox() = 0; 
        inline Material *GetMaterial( void ) { return matl; }

public:
        Material *matl;
};


#endif
