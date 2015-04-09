#ifndef _POINT_H_
#define _POINT_H_

#include <math.h>
#include "Vector.h"
#include "Definitions.h"

class Point {
  friend class Vector;
public:
  double d[4];
public:
  inline Point(double x, double y, double z, double w=1);
  inline Point() {};
  inline Vector operator-(const Point& p) const;
  inline Point operator+(const Vector& v) const;
  inline Point operator+(const Point&  p) const;
  inline Point operator-(const Vector& v) const;
  inline double X() const;
  inline double Y() const;
  inline double Z() const;
  inline double W() const;
  inline Point& operator+=(const Vector& v);
   inline Point& operator-=(const Vector& v);
  inline Point& operator+=(const Point& p);
  inline bool operator != (const Point& p) const;
  inline bool operator == (const Point& p) const;
  inline Point Cross(const Point& v) const;
  inline double Dot(const Vector &p) const;
  inline double Dot4(const Vector &p) const;
  inline Point operator/(const double c) const;
  inline Point operator*(const double c) const;
};

inline Point::Point(double x, double y, double z, double w) {
    d[0]=x; d[1]=y; d[2]=z, d[3]=w;
}

inline Vector Point::operator-(const Point& p) const {
    return Vector(d[0]-p.d[0], d[1]-p.d[1], d[2]-p.d[2]);
}

inline Point Point::operator+(const Vector& v) const {
    return Point(d[0]+v.d[0], d[1]+v.d[1], d[2]+v.d[2]);
}

inline Point Point::operator+(const Point& p) const {
    return Point(d[0]+p.d[0], d[1]+p.d[1], d[2]+p.d[2]);
}

inline Point Point::operator-(const Vector& v) const {
    return Point(d[0]-v.d[0], d[1]-v.d[1], d[2]-v.d[2]);
}


inline Point Point::operator/(const double c) const {
  return Point(d[0]/c,d[1]/c,d[2]/c);
}

inline Point Point::operator*(const double c) const {
  return Point(d[0]*c,d[1]*c,d[2]*c);
}

inline double Point::X() const {
    return d[0];
}

inline double Point::Y() const {
    return d[1];
}

inline double Point::Z() const {
    return d[2];
}

inline double Point::W() const {
    return d[3];
}

inline Point& Point::operator+=(const Vector& v) {
    d[0]+=v.d[0];
    d[1]+=v.d[1];
    d[2]+=v.d[2];
    return *this;
}

inline Point& Point::operator-=(const Vector& v) {
	d[0]-=v.d[0];
	d[1]-=v.d[1];
	d[2]-=v.d[2];
	return *this;
}

inline Point& Point::operator+=(const Point& p) {
	d[0]+=p.d[0];
	d[1]+=p.d[1];
	d[2]+=p.d[2];
	return *this;
}

inline bool Point::operator != (const Point& v) const{
    return d[0] != v.d[0] || d[1] != v.d[1] || d[2] != v.d[2];
}

inline bool Point::operator == (const Point& v) const{
    return d[0] == v.d[0] && d[1] == v.d[1] && d[2] == v.d[2];
}

inline Point Point::Cross(const Point& v) const
{
	return Point(d[1]*v.d[2]-d[2]*v.d[1],
		d[2]*v.d[0]-d[0]*v.d[2],
		d[0]*v.d[1]-d[1]*v.d[0]);
}

inline double Point::Dot(const Vector &v) const {
  return d[0]*v.d[0]+d[1]*v.d[1]+d[2]*v.d[2];
}

inline double Point::Dot4(const Vector &v) const {
  return d[0]*v.d[0]+d[1]*v.d[1]+d[2]*v.d[2]+d[3]*v.d[3];
}

inline Point Min(const Point& p1, const Point& p2)
{
	return Point(MIN(p1.d[0], p2.d[0]), MIN(p1.d[1], p2.d[1]), MIN(p1.d[2], p2.d[2]));
}

inline Point Max(const Point& p1, const Point& p2)
{
	return Point(MAX(p1.d[0], p2.d[0]), MAX(p1.d[1], p2.d[1]), MAX(p1.d[2], p2.d[2]));
}

#endif
