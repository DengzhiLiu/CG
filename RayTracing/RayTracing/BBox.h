#ifndef _BBOX_H_
#define _BBOX_H_

#pragma once
#include "Point.h"
#include "Ray.h"

class BBox {
public:
	// BBox Public Data
	Point pMin, pMax;

public:
	// BBox Public Methods
	BBox() {
		pMin = Point( INFINITY,  INFINITY,  INFINITY);
		pMax = Point(-INFINITY, -INFINITY, -INFINITY);
	}
	BBox(const Point &p) : pMin(p), pMax(p) { }
	BBox(const Point &p1, const Point &p2) {
		pMin = Min(p1, p2);
		pMax = Max(p1, p2);
	}
	BBox Union(const BBox &b) {
		BBox ret;
		ret.pMin = Min(pMin, b.pMin);
		ret.pMax = Max(pMax, b.pMax);
		return ret;
	}
	bool Overlaps(const BBox &b) const {
		bool x = (pMax.X() >= b.pMin.X()) && (pMin.X() <= b.pMax.X());
		bool y = (pMax.Y() >= b.pMin.Y()) && (pMin.Y() <= b.pMax.Y());
		bool z = (pMax.Z() >= b.pMin.Z()) && (pMin.Z() <= b.pMax.Z());
		return (x && y && z);
	}
	bool Inside(const Point &pt) const {
		return (pt.X() >= pMin.X() && pt.X() <= pMax.X() &&
			pt.Y() >= pMin.Y() && pt.Y() <= pMax.Y() &&
			pt.Z() >= pMin.Z() && pt.Z() <= pMax.Z());
	}
	void Expand(float delta) {
		pMin -= Vector(delta, delta, delta);
		pMax += Vector(delta, delta, delta);
	}

	//bool Intersect(const Ray &ray, double *hitt0 = NULL, double *hitt1 = NULL) const;
	bool Intersect(const Ray &ray, double &tmin, double &tmax) const;
};

#endif
