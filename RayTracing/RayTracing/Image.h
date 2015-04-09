#ifndef _IMAGE_H_
#define _IMAGE_H_ 1

#include "Color.h"

/* An image is a collection of xres*yres Colors.
**
** Beware!!  This allocates memory -> not thread-safe!  However,
**    presumably, you allocate this ONCE at the beginning of the
**    execution.  However, if this is not the case, you should
**    fix this!
*/
class Image
{
    Color** image;
    int xres, yres;
public:
    Image(int xres, int yres);
    ~Image();
    inline int GetXRes() const
        {
                return xres;
    }
    inline int GetYRes() const
        {
                return yres;
    }
    inline Color& operator()(int x, int y)
        {
                return image[y][x];
        }
    void Save(char* file);
};

#endif
