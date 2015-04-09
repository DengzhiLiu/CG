#include "Vector.h"
#include "Point.h"

Vector& Vector::operator=(const Point& p)
{
  d[0] = p.d[0];
  d[1] = p.d[1];
  d[2] = p.d[2];
  return *this;
}

double Vector::Dot(const Point& p) const
{
    return d[0]*p.d[0]+d[1]*p.d[1]+d[2]*p.d[2];
}

double Vector::Dot4(const Point& p) const
{
    return d[0]*p.d[0]+d[1]*p.d[1]+d[2]*p.d[2]+d[3]*p.d[3];
}
