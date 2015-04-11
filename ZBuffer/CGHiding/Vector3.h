#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <gl/gl.h>
#include <gl/glu.h>

template <typename T>
inline T sqr(const T &x) {return x*x;}

// 定义2D点类，用于保存模型的UV纹理坐标等
class CFVector2 
{
public:
	float x, y;
};

// 定义3D点类
template <typename T>  // expects T to be POD
struct CVector3 {
	T x, y, z;
	CVector3() {x = 0; y = 0; z = 0;}
	CVector3(T tx, T ty, T tz) : x(tx), y(ty), z(tz) {}
	CVector3<T>& operator+=(const CVector3<T> &rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	CVector3<T> operator+(const CVector3<T> &rhs) {
		return CVector3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	CVector3<T> operator-(const CVector3<T> &rhs) {
		return CVector3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	CVector3<T>& operator*(const T& m) {
		return CVector3<T>(x * m, y * m, z * m);
	}
	CVector3<T> operator*(const CVector3<T> &rhs) {
		return CVector3<T>(y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x);
	}
	CVector3<T>& operator/(const T& m) {
		return CVector3<T>(x / m, y / m, z / m);
	}
	T Norm() const {return sqr(x) + sqr(y) + sqr(z);}
};

//	定义4D点类
class CFVector4
{
public:
	float	a, b, c, d;
};

//
typedef	CVector3< float >	CFVector3;
typedef CVector3< double >  CDVector3;

#endif