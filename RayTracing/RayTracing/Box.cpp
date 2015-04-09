#include "Box.h"
#include "Ray.h"
#include "Definitions.h"
#include <math.h>

Box::Box(Material* matl, Point p1, Point p2) :
		Primitive(matl)
{
	Bmin = Min(p1, p2);
	Bmax = Max(p1, p2);
}

Box::~Box(void)
{
	//if (matl)		delete	matl;
}

bool Box::Intersect( Ray &ray, double &tnear, double &tfar )
{
	double t, rt, tmin, tmax;
	double bmin, bmax;

	t = INFINITY;
	Point orig = ray.GetOrigin();
	Vector dir = ray.GetDirection();
	for (int i = 0; i < 3; i++) {
		if (fabs(dir.d[i]) > EPSILON) {
			bmin = dir.d[i] > 0 ? Bmin.d[i] : Bmax.d[i];
			bmax = dir.d[i] > 0 ? Bmax.d[i] : Bmin.d[i];
			tmin = (bmin - orig.d[i]) / dir.d[i];
			tmax = (bmax - orig.d[i]) / dir.d[i];
			if (tmin > EPSILON)
				rt = tmin;
			else if (tmax > EPSILON)
				rt = tmax;
			else
				return	false;
			t = MIN(t, rt);
		}
		else {
			if ((orig.d[i] < Bmin.d[i]) && (orig.d[i] > Bmax.d[i]))
				return	false;
		}
	}
	ray.CheckHit(t, this);

	return	true;
}

void Box::Texture(Ray &ray, Color &diffuse)
{
	return ;
}

Vector Box::ComputeNormal( Ray &ray ) const
{
	Vector norm(0.0, 0.0, 0.0);
	Point HitPoint = ray.GetHitPoint();
	
	if (fabs(HitPoint.d[0] - Bmin.d[0]) < EPSILON)
		norm.d[0] = -1.0;
	else if (fabs(HitPoint.d[0] - Bmax.d[0]) < EPSILON)
		norm.d[0] = 1.0;
	else if (fabs(HitPoint.d[1] - Bmin.d[1]) < EPSILON)
		norm.d[1] = -1.0;
	else if (fabs(HitPoint.d[1] - Bmax.d[1]) < EPSILON)
		norm.d[1] = 1.0;
	else if (fabs(HitPoint.d[2] - Bmin.d[2]) < EPSILON)
		norm.d[2] = -1.0;
	else if (fabs(HitPoint.d[2] - Bmax.d[2]) < EPSILON)
		norm.d[2] = 1.0;
	
	return norm;
}

void Box::CalculateCloseBox()
{
	CBPmin = Bmin;
	CBPmax = Bmax;
	CBPcenter = (Bmin + Bmax) / 2.0;
}
