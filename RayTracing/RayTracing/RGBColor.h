#ifndef _RGBCOLOR_H_
#define _RGBCOLOR_H_


/* A Color is a RGB double.
**
** R, G, and B are all in the range [0..1]
**
** This class allows you to add, subtract, and multiply colors,
** It also allows you to get the separate components (e.g., myColor.red() ),
**    use constructions like "myColor += yourColor;"
*/
class RGBColor
{
public:
    double r,g,b;

public:
    inline RGBColor(): r(0), g(0), b(0) {}
    inline RGBColor(double r, double g, double b) : r(r), g(g), b(b){}
	inline RGBColor(double c[3]) : r(c[0]), g(c[1]), b(c[2]) {}
    inline RGBColor(const RGBColor& copy) { r = copy.r; g = copy.g; b = copy.b; }
    inline ~RGBColor() {}
    inline RGBColor operator*(const RGBColor& c) const
    {
                return RGBColor(r*c.r, g*c.g, b*c.b);
    }
    inline RGBColor operator+(const RGBColor& c) const
    {
                return RGBColor(r+c.r, g+c.g, b+c.b);
    }
    inline RGBColor operator-(const RGBColor& c) const
	{
                return RGBColor(r-c.r, g-c.g, b-c.b);
    }
    inline RGBColor operator*(double s) const
    {
                return RGBColor(r*s, g*s, b*s);
    }
    inline RGBColor& operator+=(const RGBColor& c)
    {
                r+=c.r;
                g+=c.g;
                b+=c.b;
                return *this;
    }
	//inline RGBColor& RGBColor::operator=(const RGBColor& c)
	//{
	//	r = c.r;
	//	g = c.g;
	//	b = c.b;
	//	return *this;
	//}
    inline double red() const
    {
                return r;
    }
    inline double green() const
    {
                return g;
    }
    inline double blue() const
    {
                return b;
    }
    inline double luminance() const
    {
        return (double)(0.3*g + 0.6*r + 0.1*b);
    }

    inline double max_component() const
    {
        double temp = (g > r? g : r);
        return (b > temp? b : temp);
    }
	
	inline double min_component() const
	{
		double temp = (g < r? g : r);
		return (b < temp? b : temp);
	}

	inline RGBColor& truncate()
	{
		r = r < 1.0 ? r : 1.0;
		g = g < 1.0 ? g : 1.0;
		b = b < 1.0 ? b : 1.0;
		return *this;
	}
};

#endif
