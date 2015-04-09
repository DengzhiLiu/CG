#include "Color.h"
#include "Scene.h"
#include "Camera.h"
#include "Group.h"
#include "Primitive.h"
#include "Material.h"
#include "LambertianMaterial.h"

Scene::Scene()
{
        camera = 0;
		thresh = 0.01;
        maxRayDepth = 3;
		BSPRoot = NULL;
		KDTool = NULL;
		ambient = Color(0.6, 0.6, 0.6);
        backgroundColor = Color(0, 0, 0);
		BSPTree* BSPtool = new BSPTree();
		for (int i = 0; i < 1025; i++)
			FirstInterObj[i] = NULL;
}

Scene::~Scene()
{
        if (camera) delete camera;
		if (geometry) delete geometry;
		if (BSPRoot) delete BSPRoot;
		if (BSPtool) delete BSPtool;
		light.clear();
}

void Scene::SetCamera( void )
	{
        if (camera) delete camera;
		if ( geometry ) {
			Point at =  geometry->CBPcenter;
			Point eye = at;
			//eye.d[0] = geometry->CBPmin.X();
			eye.d[1] += 200.0;
			eye.d[2] = geometry->CBPmax.Z() + 1000.0;
			camera = new Camera(eye, at, Vector(0, 1, 0), 60, 1.0, 1000.0, 512, 512);
		}
		else
			camera = new Camera();
}

void Scene::BuildTree()
{
	//geometry->CalculateCloseBox();
	//BSPRoot = BSPtool->BuildBSPTree(geometry, 0, AXISX);
	KDTool = new KdTreeAccel(geometry, spherelist);
}

Color Scene::RayTrace(Ray &ray , int idx)
{
	if ((ray.depth >= maxRayDepth) || (ray.contribution <= thresh))
		return	backgroundColor;

	bool res = false;
	Color useless;
	double tmin = 0, tmax = INFINITY;

	//if ( !geometry->Intersect(ray, tmin, tmax) )
	//	return	backgroundColor;
	//if ( FirstInterObj[idx] ) 
	//	FirstInterObj[idx]->Intersect(ray, tmin, tmax);
	//res = ray.WasIntersection();
	//if ( !res )
		// �Ⱥ����ܰ�Χ����
		for(vector<FinitePlane *>::const_iterator iter = planelist.begin(); iter != planelist.end(); ++iter)
			(*iter)->Intersect(ray, tmin, tmax);
		//for (vector<Sphere *>::const_iterator iter = spherelist.begin(); iter != spherelist.end(); ++iter)
		//	(*iter)->Intersect(ray, tmin, tmax);

		//BSPtool->Intersect(ray, BSPRoot, tmin, tmax, true, useless);
		KDTool->Intersect(ray, true, useless);

	if ( !ray.WasIntersection() ) {
		//FirstInterObj[idx] = NULL;
		return	backgroundColor;
	}
	else {
		//FirstInterObj[idx] = ray.GetHitObject();
		double mark;
		Color ksTrans;
		Color LocalColor, ReflectedColor, TransmittedColor, TransRefColor;
		Vector ReflectedDir, TransmittedDir, TransRefDir;
		
		Primitive* pobj = ray.GetHitObject();
		// �ֲ�������phong�Ĺ����Ȳ�ֵ
		LocalColor = pobj->GetMaterial()->Shade( ray, *this );
		Vector N = pobj->ComputeNormal(ray);
		Vector L = -ray.GetDirection();
		// ���ڲ��ʵ���Ҫ��дһ��
		Material* pmtl = pobj->GetMaterial();
		if (pmtl->IsMirror()) {
			double costheta = N.Dot(L);
			//if (costheta >= 0)
			//	ReflectedDir = 2 * costheta * norm - direction;
			//else
			//	ReflectedDir = -2 * costheta * norm - direction;
			ReflectedDir = 2.0 * costheta * N - L;
			ReflectedDir.Normalize();
			mark = pmtl->ks.min_component();
			// ��������ƽ���Է��ٴ���ģ���ཻ������İߵ�
			Point start = ray.GetHitPoint() + ReflectedDir * 0.1;
			Ray r(start, ReflectedDir, ray.GetDepth()+1, mark*ray.GetContribution());
			ReflectedColor = RayTrace(r, 2*idx+1);
		}
		if (pmtl->IsTransparent()) {
			double costheta1, costheta2;
			double trans = pmtl->transparent;									// ������
			costheta1 = N.Dot(L);
			if (costheta1 >= 0) {												// ���ߴӿ�����������
				costheta2 = sqrt(1.0 - (1.0 - costheta1 * costheta1) / (trans * trans));
				TransmittedDir = -L / trans - (costheta2 - costheta1 / trans) * N;
				TransmittedDir.Normalize();
				mark = pmtl->kt.min_component();
				Point start = ray.GetHitPoint() + TransmittedDir * 0.1;
				Ray r(start, TransmittedDir, ray.GetDepth()+1, mark*ray.GetContribution());
				TransmittedColor = RayTrace(r, 2*idx+2);
			}
			else {																// ���ߴ������������
				costheta1 *= -1.0;
				//double sintheta1 = sqrt(1.0 - costheta1 * costheta1);			// ȫ����ʱ���
				//double sinthetamax = 1.0 / trans;
				//ksTrans = Color(sintheta1/sinthetamax, sintheta1/sinthetamax, sintheta1/sinthetamax);
				//TransRefDir = 2.0 * costheta1 * N - L;
				//TransRefDir.Normalize();
				//Point startref = ray.GetHitPoint() + 0.001 * TransRefDir;
				//mark = pmtl->kt.min_component();
				//Ray r(ray.GetHitPoint(), TransRefDir, ray.GetDepth()+1, mark*ray.GetContribution());
				//TransRefColor = RayTrace(r, 2*idx+2);
		
				if ((1.0 - costheta1 * costheta1) * trans * trans < 1.0) {		// ������ȫ���䣬��������
					costheta2 = sqrt(1.0 - (1.0 - costheta1 * costheta1) * (trans * trans));
					TransmittedDir = -trans * L + (costheta2 - costheta1 * trans) * N;
					TransmittedDir.Normalize();
					mark = pmtl->kt.min_component();
					Point start = ray.GetHitPoint() + TransmittedDir * 0.1;
					Ray r(start, TransmittedDir, ray.GetDepth()+1, mark*ray.GetContribution());
					TransmittedColor = RayTrace(r, 2*idx+2);
				}
			}
		}
		LocalColor = LocalColor + pmtl->ks * ReflectedColor + pmtl->kt * TransmittedColor;
		LocalColor.truncate();

		return LocalColor;
	}
}

