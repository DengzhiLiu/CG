#ifndef _FinitePlane_H
#define _FinitePlane_H

#include "Primitive.h"
#include "Point.h"
#include "Vector.h"
#include "Dib.h"

class Material;

// 定义三角形、四边形或多边形类
class FinitePlane : public Primitive {
private:
	Vector planeCoefs;
	int	   vnum;
	Point* varray;
	CDIB*  pmap;

public:
	FinitePlane( Material *matl, CDIB* pmap, const int &num, Point* pos );
	virtual ~FinitePlane();
	virtual bool Intersect( Ray &ray, double &tmin, double &tmax );
	virtual	void Texture(Ray &ray, Color &diffuse);
	virtual Vector ComputeNormal( Ray &ray ) const;
	virtual void CalculateCloseBox();
};

#endif
