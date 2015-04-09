#ifndef _BOX_H_
#define _BOX_H_

#pragma once
#include "Point.h"
#include "Primitive.h"

class Box :	public Primitive
{
public:
	Point Bmin, Bmax;

public:
	Box(Material* matl, Point Bmin, Point Bmax);
	~Box(void);
	virtual bool Intersect( Ray &ray, double &tmin, double &tmax );
	virtual	void Texture(Ray &ray, Color &diffuse);
	virtual Vector ComputeNormal( Ray &ray ) const;
	virtual void CalculateCloseBox();
};

#endif