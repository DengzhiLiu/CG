#ifndef _LAMBERTIANMATERIAL_H_
#define _LAMBERTIANMATERIAL_H_

#include "Color.h"
#include "ObjInfo.h"
#include "BSPTree.h"
#include "Material.h"

class LambertianMaterial : public Material {
public:
	LambertianMaterial( void );	
    LambertianMaterial( Mmaterial* pmtl );
    virtual ~LambertianMaterial() {};
    virtual Color Shade( Ray &ray, Scene &scene );
};

#endif

