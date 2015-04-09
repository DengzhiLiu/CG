#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Definitions.h"
#include "Color.h"

class Ray;
class Scene;

class Material {
public:
	bool bTexture;
	Color ka, kd, ks, kt;				// ���⣬�����䣬���������͸��ϵ��
	double illum, shineness, transparent;		// ����ĸ߹�ָ��, �����ʺ�͸����(͸����)
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

