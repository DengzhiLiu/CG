#ifndef _OBJECT_H
#define _OBJECT_H

#include "Point.h"

class Ray;
class Material;
class Vector;

class Object {
public:
	Point	CBPmin, CBPcenter, CBPmax;		// ³¡¾°°üÎ§ºÐ

public:
        Object() {}
        virtual ~Object() {}
        virtual bool Intersect( Ray &ray, double &tmin, double &tmax ) = 0;
		virtual void CalculateCloseBox() = 0;
};


#endif
