#include "BBox.h"
#include <vector>

using namespace std;

bool BBox::Intersect(const Ray &ray, double &tmin, double &tmax) const 
{
	Point orig = ray.GetOrigin();
	double t0 = 0, t1 = INFINITY;
	for (int i = 0; i < 3; ++i) {
	  // Update interval for _i_th bounding box slab
	  double dir = ray.dir.d[i];
	  double invRayDir = 1.0 / dir;
	  double tNear = (pMin.d[i] - orig.d[i]) * invRayDir;
	  double tFar  = (pMax.d[i] - orig.d[i]) * invRayDir;
	  // Update parametric interval from slab intersection $t$s
	  if (tNear > tFar) swap(tNear, tFar);
	  t0 = tNear > t0 ? tNear : t0;
	  t1 = tFar  < t1 ? tFar  : t1;
	  if (t0 > t1) return false;
	}
	tmin = t0;
	tmax = t1;
	return true;
}

//bool BBox::Intersect(const Ray &ray, double &tmin, double &tmax) const
//{
//	double coef = 1.1;
//	Point start = ray.GetOrigin();
//	Vector direction = ray.GetDirection();
//	double t0, t1;
//	double minX = pMin.X() * coef;
//	double maxX = pMax.X() * coef;
//	double minY = pMin.Y() * coef;
//	double maxY = pMax.Y() * coef;
//	double minZ = pMin.Z() * coef;
//	double maxZ = pMax.Z() * coef;
//	/* 首先计算与YZ平面的相交情况 */
//	if (fabs(direction.X()) < EPSILON)
//		if (start.X() < minX || start.X() > maxX)
//			return false;
//	tmin = (minX - start.X()) / direction.X();
//	tmax = (maxX - start.X()) / direction.X();
//
//	if (tmin - tmax > EPSILON)
//	{
//		double temp = tmin;
//		tmin = tmax;
//		tmax = temp;
//	}
//
//	if (tmax < EPSILON)
//		return false;
//	//tmin = MAX(tmin, 0);
//
//	/* 其次计算与XZ平面的相交情况 */
//	if (fabs(direction.Y()) < EPSILON)
//		if (start.Y() < minY || start.Y() > maxY)
//			return false;
//	t0 = (minY - start.Y()) / direction.Y();
//	t1 = (maxY - start.Y()) / direction.Y();
//
//	if (t0 - t1 > EPSILON)
//	{
//		double temp = t0;
//		t0 = t1;
//		t1 = temp;
//	}
//	if (t1 < EPSILON)
//		return false;
//
//	if (t0 - tmin > EPSILON)
//		tmin = t0;
//	if (tmax - t1 > EPSILON)
//		tmax = t1;
//
//	/* 最后计算与XY平面的相交情况 */
//	if (fabs(direction.Z()) < EPSILON)
//		if (start.Z() < minZ || start.Z() > maxZ)
//			return false;
//	t0 = (minZ - start.Z()) / direction.Z();
//	t1 = (maxZ - start.Z()) / direction.Z();
//
//	if (t0 - t1 > EPSILON)
//	{
//		double temp = t0;
//		t0 = t1;
//		t1 = temp;
//	}
//	if (t1 < EPSILON)
//		return false;
//
//	if (t0 - tmin > EPSILON)
//		tmin = t0;
//	if (tmax - t1 > EPSILON)
//		tmax = t1;
//
//	return true;
//
//	//double t1, t2;
//	//double bmin, bmax;
//
//	//double rtmin = INFINITY;
//	//double rtmax = INFINITY;
//	//Point orig = ray.GetOrigin();
//	//Vector dir = ray.GetDirection();
//	//for (int i = 0; i < 3; i++) {
//	//	if (fabs(dir.d[i]) > EPSILON) {
//	//		bmin = dir.d[i] > 0 ? CBPmin.d[i] : CBPmax.d[i];
//	//		bmax = dir.d[i] > 0 ? CBPmax.d[i] : CBPmin.d[i];
//	//		t1 = (bmin - orig.d[i]) / dir.d[i];
//	//		t2 = (bmax - orig.d[i]) / dir.d[i];
//	//		if (t2 < EPSILON)	return false;
//	//		if (t1 > EPSILON)
//	//			rtmin = MIN(t1, rtmin);
//	//		if (tmax > EPSILON)
//	//			rtmax = MIN(t2, rtmax);
//	//	}
//	//	else {
//	//		if ((orig.d[i] < CBPmin.d[i]) && (orig.d[i] > CBPmax.d[i]))
//	//			return	false;
//	//	}
//	//}
//	//if (rtmin > rtmax)
//	//	rtmin = 0;
//	//tmin = rtmin;
//	//tmax = rtmax;
//	//return true;
//
//}
