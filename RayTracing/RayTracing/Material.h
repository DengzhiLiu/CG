#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Definitions.h"
#include "Color.h"

class Ray;
class Scene;

class Material {
public:
	bool bTexture;
	Color ka, kd, ks, kt;				// 泛光，漫反射，环境反射和透射系数
	double illum, shineness, transparent;		// 镜面的高光指数, 折射率和透射率(透明度)
public:
    Material() {}
    virtual ~Material() {}
    virtual Color Shade( Ray &ray, Scene &scene ) = 0;

	bool IsMirror() const
	{
		if (shineness > EPSILON)
			return	true;
		else
			return	false;
	}

	bool IsTransparent() const
	{
		if (transparent > EPSILON)
			return	true;
		else
			return	false;
	}

	bool HasTexture() const
	{
		return bTexture;
	}
};


#endif

