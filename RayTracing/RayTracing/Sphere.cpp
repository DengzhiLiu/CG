#include "Sphere.h"
#include "Definitions.h"
#include <math.h>

Sphere::Sphere( Material *matl, CDIB *bitmap, const Point &center, float radius ) :
        Primitive(matl), p_bitmap(bitmap), center(center), radius(radius)
{
}

Sphere::~Sphere()
{
	if (p_bitmap)
		delete p_bitmap;
}

// 把相交的参数信息都存在光线ray中
bool Sphere::Intersect( Ray &ray, double &tl, double &tr )
{
	double a, b, c, delta;
	double t, tmin, tmax;
	Point orig = ray.GetOrigin();
	Vector dir = ray.GetDirection();
	
	a = dir.Dot(dir);
	b = dir.Dot(orig - center) * 2;
	c = (orig - center).Dot(orig - center) - radius * radius;
	delta = b * b - 4 * a * c;
	if (delta < 0)	return false;
	else if (delta > 0) {
		tmin = (-b - sqrt(delta)) / (2 * a);
		tmax = (-b + sqrt(delta)) / (2 * a);
		if (tmin - tl > EPSILON && tr - tmin > EPSILON)
			t = tmin;
		else if (tmax - tl > EPSILON && tr - tmax > EPSILON)
			t = tmax;
		else	
			return false;
		// 更新光线与场景相交信息，若最近则更改
		ray.CheckHit(t, this);
	}
	else {
		tmin = -b / (2 * a);
		if (tmin - tl > EPSILON && tr - tmin > EPSILON) {
			t = tmin;
			ray.CheckHit(t, this);
		}
		else
			return false;
	}
	
	return	true;
}

//texture function for ball
void Sphere::Texture(Ray &ray, Color &diffuse)
{
	double u, v;
	double cos_u,cos_v;
	Point hitpos = ray.GetHitPoint();

	cos_v = (hitpos.Z() - center.Z()) / radius;
	if(cos_v < -1)
		cos_v = -1;
	if(cos_v > 1)
		cos_v = 1;
	v = acos(cos_v);
	cos_u = (hitpos.X() - center.X()) / (radius*sin(v));
	if(cos_u < -1)
		cos_u = -1;
	if(cos_u > 1)
		cos_u = 1;
	u = acos(cos_u);
	if(hitpos.Z() < center.Z())
		u += M_PI;
	u = u / (2 * M_PI);
	v = v / M_PI;
	if(u < 0.0 || u > 1.0)
		u = 0.0;
	if(v < 0.0 || v > 1.0)
		v = 0.0;

	int r, c;
	int width, height;
	u_char* lpSrc;

	width = p_bitmap->width;
	height = p_bitmap->height;
	c = (int)(u * width);
	r = (int)(v * height);
	lpSrc = p_bitmap->image + r * width * 3 + c *3;		// 每个像素点有三个字节

	diffuse.r = *lpSrc / (double)255;
	diffuse.g = *(lpSrc + 1) / (double)255;
	diffuse.b = *(lpSrc + 2) / (double)255;
}

Vector Sphere::ComputeNormal( Ray &ray ) const
{
	Vector norm = (ray.GetHitPoint() - center) / radius;
	norm.Normalize();

	return norm;
}

void Sphere::CalculateCloseBox()
{
	CBPcenter = center;

	CBPmin.d[0] = center.d[0] - radius;
	CBPmin.d[1] = center.d[1] - radius;
	CBPmin.d[2] = center.d[2] - radius;

	CBPmax.d[0] = center.d[0] + radius;
	CBPmax.d[1] = center.d[1] + radius;
	CBPmax.d[2] = center.d[2] + radius;
}

