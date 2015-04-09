#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <math.h>
#include "Point.h"
#include "Color.h"

class Light
{
  Point pos;
  Color lcolor;
public:
        Light( const Point &pos, const Color &lcolor ): pos(pos), lcolor(lcolor) {}

        inline Point GetPosition( void ) const { return pos; }
        inline Color GetColor( void ) const { return lcolor; }
};


#endif
