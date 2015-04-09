#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include <iostream>

class Point;

class Vector
{
    friend class Point;
public:
	double d[4];
public:
    inline Vector(double x, double y, double z, double w=0);
    inline Vector(const Vector& v);
        inline Vector() {}
    inline double Length( void ) const;
        inline double LengthSqr( void ) const;
    inline Vector& operator=(const Vector& v);
    Vector& operator=(const Point& p);
    inline bool operator==(const Vector& v) const;
    inline Vector operator*(double s) const;
	inline Vector operator/(double s) const;
    inline Vector operator*(const Vector& v) const;
    inline Vector operator/(const Vector& v) const;
    inline Vector& operator*=(double s);
    inline Vector operator+(const Vector& v) const;
    inline Vector& operator+=(const Vector& v);
    inline Vector operator-() const;
    inline Vector operator-(const Vector& v) const;
    inline Vector Normalize();
    inline Vector Cross(const Vector& v) const;
    inline double Dot(const Vector& v) const;
        inline double Dot4(const Vector& v) const;
    double Dot(const Point& p) const;
        double Dot4(const Point& p) const;
    inline double X() const;
    inline double Y() const;
    inline double Z() const;
        inline double W() const;
	inline int MainAxis() const;
    inline bool operator != (const Vector& v) const;
};





inline Vector::Vector(double x, double y, double z, double w)
{
    d[0]=x;
    d[1]=y;
    d[2]=z;
        d[3]=w;
}

inline Vector::Vector(const Vector& v)
{
    d[0]=v.d[0];
    d[1]=v.d[1];
    d[2]=v.d[2];
        d[3]=v.d[3];
}

inline double Vector::Length( void ) const
{
    return sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
}

inline double Vector::LengthSqr( void ) const
{
    return d[0]*d[0]+d[1]*d[1]+d[2]*d[2];
}

inline Vector& Vector::operator=(const Vector& v)
{
    d[0]=v.d[0];
    d[1]=v.d[1];
    d[2]=v.d[2];
        d[3]=v.d[3];
    return *this;
}

inline Vector Vector::operator*(double s) const
{
    return Vector(d[0]*s, d[1]*s, d[2]*s);
}

inline Vector Vector::operator/(double s) const
{
	return Vector(d[0]/s, d[1]/s, d[2]/s);
}

inline Vector operator*(double s, const Vector& v)
{
    return v*s;
}

inline Vector Vector::operator*(const Vector& v) const
{
    return Vector(d[0]*v.d[0], d[1]*v.d[1], d[2]*v.d[2]);
}

inline Vector Vector::operator/(const Vector& v) const
{
    return Vector(d[0]/v.d[0], d[1]/v.d[1], d[2]/v.d[2]);
}

inline Vector Vector::operator+(const Vector& v) const
{
    return Vector(d[0]+v.d[0], d[1]+v.d[1], d[2]+v.d[2]);
}

inline Vector& Vector::operator+=(const Vector& v)
{
    d[0]=d[0]+v.d[0];
    d[1]=d[1]+v.d[1];
    d[2]=d[2]+v.d[2];
    return *this;
}

inline Vector& Vector::operator*=(double s)
{
    d[0]=d[0]*s;
    d[1]=d[1]*s;
    d[2]=d[2]*s;
    return *this;
}

inline Vector Vector::operator-() const
{
    return Vector(-d[0], -d[1], -d[2]);
}

inline Vector Vector::operator-(const Vector& v) const
{
    return Vector(d[0]-v.d[0], d[1]-v.d[1], d[2]-v.d[2]);
}

inline Vector Vector::Normalize()
{
    double l=Length();
    d[0]=d[0]/l;
    d[1]=d[1]/l;
    d[2]=d[2]/l;
    return *this;
}

inline Vector Vector::Cross(const Vector& v) const
{
    return Vector(d[1]*v.d[2]-d[2]*v.d[1],
              d[2]*v.d[0]-d[0]*v.d[2],
              d[0]*v.d[1]-d[1]*v.d[0]);
}

inline double Vector::Dot(const Vector& v) const
{
    return d[0]*v.d[0]+d[1]*v.d[1]+d[2]*v.d[2];
}

inline double Vector::Dot4(const Vector& v) const
{
    return d[0]*v.d[0]+d[1]*v.d[1]+d[2]*v.d[2]+d[3]*v.d[3];
}

inline double Vector::X() const
{
    return d[0];
}

inline double Vector::Y() const
{
    return d[1];
}

inline double Vector::Z() const
{
    return d[2];
}

inline double Vector::W() const
{
    return d[3];
}

inline int Vector::MainAxis() const
{
	double tmax = d[0] > d[1] ? d[0] : d[1];
	int axis = d[0] > d[1] ? 0 : 1;
	axis = tmax > d[2] ? axis : 2;
	return	axis;
}

inline bool Vector::operator != (const Vector& v) const {
    return d[0] != v.d[0] || d[1] != v.d[1] || d[2] != v.d[2] || d[3] != v.d[3];
}

inline bool Vector::operator == (const Vector& v) const {
    return d[0] == v.d[0] && d[1] == v.d[1] && d[2] == v.d[2] && d[3] == v.d[3];
}


#endif
