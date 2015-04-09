#include "Ray.h"
#include "Material.h"
#include "Triangle.h"
#include <math.h>

Triangle::Triangle( Material *matl, Point* pos, Vector* norm ) :
	Primitive(matl)
{
	for (int i = 0; i < 3; i++) {
		varray[i] = pos[i];
		vnormal[i] = norm[i];
	}
	interNormal = Vector(0, 0, 0);

	Vector v1 = varray[1] - varray[0];
	Vector v2 = varray[2] - varray[0];
	planeCoefs = v1.Cross(v2);
	planeCoefs.Normalize();

	planeCoefs = Vector( planeCoefs.X(), planeCoefs.Y(), planeCoefs.Z(), -planeCoefs.Dot(varray[0]) );
}

Triangle::~Triangle(void)
{
	//if (matl)	delete	matl;
}

bool Triangle::Intersect( Ray &ray, double &tmin, double &tmax )
{
	//Vector edge1 = varray[1] - varray[0];
	//Vector edge2 = varray[2] - varray[0];
	//if ((fabs(edge1.X()) < EPSILON && fabs(edge1.Z()) < EPSILON) || (fabs(edge1.Y()) < EPSILON && fabs(edge1.Z()) < EPSILON) ||
	//	(fabs(edge2.X()) < EPSILON && fabs(edge2.Z()) < EPSILON) || (fabs(edge2.Y()) < EPSILON && fabs(edge2.Z()) < EPSILON))
	//{
	//	int tt = 56;
	//}

	////method 1
	//double a, b, t, ang, sum;
	//Vector v1, v2;
	//Point orig = ray.GetOrigin();
	//Vector direction = ray.GetDirection();

	////double dep = direction.Dot(planeCoefs);
	////if (dep > 0)	return false;			// 平面法向量逆着视点，则剔除

	//a = planeCoefs.Dot(direction);
	//if (fabs(a) < EPSILON)	return	false;		// 光线与平面平行
	//b = -(planeCoefs.Dot(orig) + planeCoefs.W());
	//t = b / a;
	//if (t < EPSILON || t - tmax > EPSILON)	return	false;

	//// 判断交点是否在多边形内
	//sum = 0.0;
	//Point hitpos = orig + direction * t;
	//for (int i = 0; i < 3; i++) {
	//	v1 = varray[i] - hitpos;
	//	v2 = varray[(i+1) % 3] - hitpos;
	//	v1.Normalize();
	//	v2.Normalize();
	//	ang = acos(v1.Dot(v2));
	//	sum += ang;
	//}
	//if (fabs(sum - 2 * M_PI)  < 0.1) 
	//	ray.CheckHit(t, this);

	//return	true;

	// method2
	Vector edge1, edge2, tvec, pvec, qvec;
	Point orig = ray.GetOrigin();
	Vector dir = ray.GetDirection();
	double det,inv_det;
	double u,v;

	//double dep = dir.Dot(planeCoefs);
	//if (dep > 0)	return false;			// 平面法向量逆着视点，则剔除
	
	// find vectors for two edges sharing vert0
	edge1 = varray[1] - varray[0];
	edge2 = varray[2] - varray[0];

	// begin calculating determinant - also used to calculate U parameter
	pvec = dir.Cross(edge2);
	// if determinant is near zero, ray lies in plane of triangle 
	det =edge1.Dot(pvec); 
	inv_det = 1.0 / det;

#ifdef CULL_
	if (det > EPSILON)  
	{
		// calculate distance from vert0 to ray origin
		tvec = orig - varray[0];
	
		// calculate U parameter and test bounds 
		u = tvec.Dot(pvec);
		if (u < 0.0f || u > det)
			return false;

		// prepare to test V parameter
		qvec = tvec.Cross(edge1);

		// calculate V parameter and test bounds 
		v = dir.Dot(qvec);
		if (v < 0.0f || u + v > det)
			return false;

	}
	else if(det < -EPSILON)
	{
		// calculate distance from vert0 to ray origin 
		tvec = orig - varray[0];
		// calculate U parameter and test bounds 
		u = tvec.Dot(pvec);
		if (u > 0.0f || u < det)
			return false;

		// prepare to test V parameter 
		qvec = tvec.Cross(edge1);

		// calculate V parameter and test bounds 
		v = dir.Dot(qvec);
		if (v > 0.0f || u + v < det)
			return false;
	}
	else 
		return false;  // ray is parallel to the plane of the triangle 

#else
	if (fabs(det) < EPSILON)
		return	false;

	tvec = orig - varray[0];
	u = tvec.Dot(pvec) * inv_det;
	if (u < 0.0 || u > 1.0)
		return false;
	
	qvec = tvec.Cross(edge1);
	v = dir.Dot(qvec) * inv_det;
	if (v < 0.0 || u + v > 1.0)
		return false;
#endif

	// calculate t, ray intersects triangle 
	double t = edge2.Dot(qvec) * inv_det;
	if (t - tmax > EPSILON || tmin - t > EPSILON)	return false;
	ray.CheckHit(t, this);

	return true;
}

void Triangle::Texture(Ray &ray, Color &diffuse)
{
	return ;
}

Vector Triangle::ComputeNormal( Ray &ray ) const
{
	// method1
	Point hitpos = ray.GetHitPoint();
	Vector N = vnormal[0] + vnormal[1] + vnormal[2];
	N = Vector(fabs(N.X()), fabs(N.Y()), fabs(N.Z()));
	int mainAxis = N.MainAxis();

	int id1 = (mainAxis + 1) % 3;
	int id2 = (mainAxis + 2) % 3;
	double x = hitpos.d[id1];
	double y = hitpos.d[id2];
	double x1 = varray[0].d[id1];
	double y1 = varray[0].d[id2];
	double x2 = varray[1].d[id1];
	double y2 = varray[1].d[id2];
	double x3 = varray[2].d[id1];
	double y3 = varray[2].d[id2];

	// 求出投影后交点的重心坐标
	double denominator = x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2;
	double u = (x * y2 + x2 * y3 + x3 * y - x * y3 - x2 * y - x3 * y2) / denominator;
	double v = (x1 * y + x * y3 + x3 * y1 - x1 * y3 - x * y1 - x3 * y) / denominator;
	double w = (x1 * y2 + x2 * y + x * y1 - x1 * y - x2 * y1 - x * y2) / denominator;
	N = vnormal[0] * u + vnormal[1] * v + vnormal[2] * w;
	N.Normalize();
	return N;

	// method2
	// 计算平面法向量分量中绝对值最大的方向
	//Vector norm = Vector(fabs(planeCoefs.X()), fabs(planeCoefs.Y()), fabs(planeCoefs.Z()));
	//int mainAxis = norm.MainAxis();

	//// 对法向进行双线性插值
	//Vector tv;
	//double beta[3];
	//Vector N = Vector(0, 0, 0);
	//Point hitpos = ray.GetHitPoint();
	//for (int i = 0; i < 3; i++)	{
	//	tv = (varray[(i+2)%3] - varray[(i+1)%3]).Cross(hitpos - varray[(i+1)%3]);
	//	beta[i] = tv.d[mainAxis] / planeCoefs.d[mainAxis];
	//}

	//for (int i = 0; i < 3; i++)
	//	N += vnormal[i] * beta[i];
	//N.Normalize();
	//return N;

	// method3
	/*Point VA, VB;
	Point hitpos = ray.GetHitPoint();
	double x = hitpos.X();
	double x1 = varray[0].X();
	double y1 = varray[0].Y();
	double z1 = varray[0].Z();
	double x2 = varray[1].X();
	double y2 = varray[1].Y();
	double z2 = varray[1].Z();
	double x3 = varray[2].X();
	double y3 = varray[2].Y();
	double z3 = varray[2].Z();
	double coef1 = (x - x2) / (x1 - x2);
	double coef2 = (x1 - x) / (x1 - x2);
	VA.d[0] = x;
	VA.d[1] = coef1 * y1 + coef2 * y2;
	VA.d[2] = coef1 * z1 + coef2 * z2;
	coef1 = (x - x3) / (x1 - x3);
	coef2 = (x1 - x) / (x1 - x3);
	VB.d[0] = x;
	VB.d[1] = coef1 * y1 + coef2 * y3;
	VB.d[2] = coef1 * z1 + coef2 * z3;
	Vector V1V2 = varray[0] - varray[1];
	Vector V1V3 = varray[0] - varray[2];
	Vector VAV2 = VA - varray[1];
	Vector VBV3 = VB - varray[2];
	double u = VAV2.Length() / V1V2.Length();
	double v = VBV3.Length() / V1V3.Length();
	Vector NA = u * vnormal[0] + (1 - u) * vnormal[1];
	Vector NB = v * vnormal[0] + (1 - v) * vnormal[2];
	Vector VAVB = VA - VB;
	Vector VPVB = hitpos - VB;
	double w = VPVB.Length() / VAVB.Length();
	Vector N = w * NA + (1 - w) * NB;

	N.Normalize();
	return N;*/
}

void Triangle::CalculateCloseBox()
{
	CBPmin = CBPmax = CBPcenter = varray[0];

	for (int i = 1; i < 3; i++) {
		CBPmin = Min(CBPmin, varray[i]);
		CBPmax = Max(CBPmax, varray[i]);
		CBPcenter += varray[i];
	}
	CBPcenter = CBPcenter / 3.0;

	return	;
}
