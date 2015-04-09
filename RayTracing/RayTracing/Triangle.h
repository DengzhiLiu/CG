#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#pragma once
#include "Primitive.h"

class Triangle : public Primitive
{
public:
	Point  varray[3];
	Vector vnormal[3];
	Vector planeCoefs;
	Vector interNormal;

public:
	Triangle( Material *matl, Point* pos, Vector* norm );
	~Triangle(void);
	virtual bool Intersect( Ray &ray, double &tmin, double &tmax );
	virtual	void Texture(Ray &ray, Color &diffuse);
	virtual Vector ComputeNormal( Ray &ray ) const;
	virtual void CalculateCloseBox();
};

#endif