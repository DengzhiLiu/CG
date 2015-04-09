#include "ConstantColorMaterial.h"

ConstantColorMaterial::ConstantColorMaterial( const Color &matlColor ) :
        matlColor(matlColor)
{
}

Color ConstantColorMaterial::Shade( Ray &ray, Scene &scene ) const
{
        return matlColor;
}
