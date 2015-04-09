#include "Ray.h"
#include "FinitePlane.h"
#include "Definitions.h"
#include <math.h>
#include <malloc.h>
#include <iostream>


FinitePlane::FinitePlane( Material *matl, CDIB* pmap, const int &num, Point* pos ) :
	Primitive(matl), pmap(pmap), vnum(num), varray(pos)
{
	varray = new Point[vnum];
	for (int i = 0; i < vnum; i++)
		varray[i] = pos[i];
	
	Vector v1 = varray[1] - varray[0];
	Vector v2 = varray[2] - varray[0];
	planeCoefs = v1.Cross(v2);
	planeCoefs.Normalize();
	planeCoefs = Vector( planeCoefs.X(), planeCoefs.Y(), planeCoefs.Z(), -planeCoefs.Dot(varray[0]) );
}

FinitePlane::~FinitePlane()
{
	if (varray)		free(varray);
}

bool FinitePlane::Intersect( Ray &ray, double &tmin, double &tmax )
{
	double a, b, t, ang, sum;
	Vector v1, v2;
	Point orig = ray.GetOrigin();
	Vector direction = ray.GetDirection();

	a = planeCoefs.Dot(direction);
	if (a > -EPSILON)	return	false;		// 光线与平面平行或背向平面
	b = -(planeCoefs.Dot(orig) + planeCoefs.W());
	t = b / a;
	if (t < EPSILON)	return	false;
	
	// 判断交点是否在多边形内
	sum = 0.0;
	Point hitpos = orig + direction * t;
	for (int i = 0; i < vnum; i++) {
		v1 = varray[i] - hitpos;
		v2 = varray[(i+1) % vnum] - hitpos;
		v1.Normalize();
		v2.Normalize();
		ang = acos(v1.Dot(v2));
		sum += ang;
	}
	if (fabs(sum - 2 * M_PI)  < 0.01) 
		ray.CheckHit(t, this);
	
	return	true;
}

void FinitePlane::Texture(Ray &ray, Color &diffuse)
{
	double u, v;
	double x, y, z, x0, x1, x3, y0, y1, y3, z0, z1, z3;
	double D,D1,D2;
	Point hitpos = ray.GetHitPoint();

	x = hitpos.X();
	y = hitpos.Y();
	z = hitpos.Z();
	x0 = varray[0].X();
	y0 = varray[0].Y();
	z0 = varray[0].Z();
	x1 = varray[1].X();
	y1 = varray[1].Y();
	z1 = varray[1].Z();
	x3 = varray[3].X();
	y3 = varray[3].Y();
	z3 = varray[3].Z();
	D = (x1 - x0) * (y0 - y3) - (x0 - x3) * (y1 - y0);
	if(fabs(D) < EPSILON)
	{
		D = (x1 - x0) * (z0 - z3) - (x0 - x3) * (z1 - z0);
		if(fabs(D) < EPSILON)
		{
			D = (y1 - y0) * (z0 - z3) - (y0 - y3) * (z1 - z0);
			D1 = (y0 - y) * (z0 - z3) - (y0 - y3) * (z0 - z);
			D2 = (y1 - y0) * (z0 - z) - (y0 - y) * (z1 - z0);
		}else{
			D1 = (x0 - x) * (z0 - z3) - (x0 - x3) * (z0 - z);
			D2 = (x1 - x0) * (z0 - z) - (x0 - x) * (z1 - z0);
		}
	}else{
		D1 = (x0 - x) * (y0 - y3) - (x0 - x3) * (y0 - y);
		D2 = (x1 - x0) * (y0 - y) - (x0 - x) * (y1 - y0);
	}
	u = D1 / D;
	v = D2 / D;
	//if(u<0)
	//	u=0;
	//if(v<0)
	//	v=0;
	u = -u;

	int r, c;
	int width, height;
	u_char* lpSrc;

	width = pmap->width;
	height = pmap->height;
	r = (int)(u * height);
	c = (int)(v * width);
	lpSrc = pmap->image + r * width * 3 + c * 3;

	diffuse.r = *lpSrc / (double)255;
	diffuse.g = *(lpSrc + 1) / (double)255;
	diffuse.b = *(lpSrc + 2) / (double)255;
	return ;
}

Vector FinitePlane::ComputeNormal( Ray &ray ) const
{
	return planeCoefs;
}

void FinitePlane::CalculateCloseBox()
{
	if (vnum <=2)	return;
	CBPmin = CBPmax = CBPcenter = varray[0];

	for (int i = 1; i < vnum; i++) {
		CBPmin = Min(CBPmin, varray[i]);
		CBPmax = Max(CBPmax, varray[i]);
		CBPcenter += varray[i];
	}
	CBPcenter = CBPcenter / (double) vnum;

	return	;
}