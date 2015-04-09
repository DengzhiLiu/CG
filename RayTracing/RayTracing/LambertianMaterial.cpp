#include "LambertianMaterial.h"
#include "Ray.h"
#include "Primitive.h"
#include "Scene.h"
#include "Light.h"
#include <math.h>

LambertianMaterial::LambertianMaterial( void )
{
	ka = Color(0.8, 0.8, 0.8);
	kd = Color(0.2, 0.2, 0.2);
	ks = Color(0.0, 0.0, 0.0);
	kt = Color(0.0, 0.0, 0.0);
	illum = 2;
	shineness = 0;
	transparent = 0;
	bTexture = false;
}

LambertianMaterial::LambertianMaterial( Mmaterial* pmtl )
{
	ka = Color(pmtl->ambient);
	kd = Color(pmtl->diffuse);
	ks = Color(pmtl->specular);
	//kt = Color(1.0, 1.0, 1.0) - ks;
	kt = Color(pmtl->trans);
	illum = pmtl->illum;
	shineness = pmtl->shininess;
	transparent = pmtl->transparent;
	bTexture = pmtl->bTexture;
}

Color LambertianMaterial::Shade( Ray &ray, Scene &scene )
{
	bool result;
	Light* light;
	Vector L, H, V;
	double costheta, cosalpha;
	Color ambient, diffuse, specular, color;
	Primitive* hitobj = ray.GetHitObject();
	Vector norm = hitobj->ComputeNormal(ray);
	Point hitpos = ray.GetHitPoint();
	Point start = hitpos + 0.1 * ray.GetDirection();
	int lightnum = scene.GetNumLights();

	if (HasTexture()) {
		Color c;
		hitobj->Texture(ray, c);
		ka = Color(c);
		kd = Color(c);
	}
	ambient = scene.GetAmbient() * ka;
	V = ray.GetOrigin() - hitpos;
	V.Normalize();
	for (int i = 0; i < lightnum; i++) {
		light = scene.GetLight(i);
		L = light->GetPosition() - hitpos;
		
		Color factor = Color(1.0, 1.0, 1.0);
		Ray r(start, L);
		result = scene.BSPtool->Intersect(r, scene.BSPRoot, 0.0, 1.0, false, factor);
		result = scene.KDTool->Intersect(r, false, factor);
		if ( !result ) {
			L.Normalize();
			H = L + V;
			H.Normalize();

			costheta = norm.Dot(L);
			cosalpha = norm.Dot(H);
			if (costheta > 0)
				diffuse += light->GetColor() * factor * kd * costheta;
			if (cosalpha > 0)
				specular += light->GetColor() * factor * ks * pow(cosalpha, illum);	
		}
	}
	color = ambient + diffuse + specular;
	color.truncate();

	return color;
}

