#ifndef _CONSTANTCOLORMATERIAL_H
#define _CONSTANTCOLORMATERIAL_H

#include "Color.h"
#include "Material.h"

class ConstantColorMaterial : public Material {
private:
        Color matlColor;
public:
        ConstantColorMaterial( const Color &matlColor );
        virtual ~ConstantColorMaterial() {}
        virtual Color Shade( Ray &ray, Scene &scene ) const;
};

#endif
