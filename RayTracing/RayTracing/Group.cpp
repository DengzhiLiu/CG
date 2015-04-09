#include "Group.h"
#include "Box.h"
#include "Triangle.h"
#include "FinitePlane.h"
#include "LambertianMaterial.h"
#include <assert.h>

using namespace std;

Group::Group()
{
}

Group::~Group()
{
	if (!objs.empty())
		objs.clear();
	if (!planes.empty())
		planes.clear();
}

void Group::Add(Triangle* obj)
{
	objs.push_back(obj);
}

Triangle* Group::Get(u_int i )
{
	assert( i < objs.size() );
	return objs[i];
}

int Group::GetSize( void ) const
{
	return (int)(objs.size());
}

bool Group::AddModel(Mmodel* pmodel)
{
	//Face end = pmodel->surface.end();
	for (vector<Face>::const_iterator iter = pmodel->surface.begin(); iter != pmodel->surface.end(); ++iter) {
		Point* pos = new Point[3];
		Vector* norm = new Vector[3];
		for (int i = 0; i < 3; i ++) {
			pos[i] = pmodel->vertices[iter->vertIndex[i]];
			norm[i] = pmodel->normals[iter->normalIndex[i]];
		}
		LambertianMaterial* matl;
		if (pmodel->nummaterials)
			matl = new LambertianMaterial(&pmodel->materials[iter->MaterialID]);
		else
			matl = new LambertianMaterial();
		Triangle* tri = new Triangle(matl, pos, norm);
		Add(tri);
	}

	return	true;
}

// 在场景中加一个包围面
void Group::SetSceneFace(Mmodel* pmodel)
{
	double minx = CBPmin.X() - 200;
	double miny = CBPmin.Y() - 1;
	double minz = CBPmin.Z() - 300;
	double maxx = CBPmax.X() + 200;
	double maxy = CBPmax.Y() + 500;
	double maxz = CBPmax.Z() + 700;

	Point pos[8];
	pos[0] = Point(minx, miny, maxz);
	pos[1] = Point(maxx, miny, maxz);
	pos[2] = Point(maxx, maxy, maxz);
	pos[3] = Point(minx, maxy, maxz);
	pos[4] = Point(minx, miny, minz);
	pos[5] = Point(maxx, miny, minz);
	pos[6] = Point(maxx, maxy, minz);
	pos[7] = Point(minx, maxy, minz);

	int mtlID;
	// add back
	Point* back = new Point[4];
	back[0] = pos[4];
	back[1] = pos[5];
	back[2] = pos[6];
	back[3] = pos[7];
	//CDIB* pDib = new CDIB();
	//pDib->readBMP("./Data/wall.bmp");
	mtlID = m_objLoader.FindMaterial(pmodel, "back");
	LambertianMaterial* mtlback = new LambertianMaterial(&pmodel->materials[mtlID]);
	//mtlback->bTexture = true;
	FinitePlane* bp = new FinitePlane(mtlback, NULL, 4, back);
	planes.push_back(bp);

	Point* left = new Point[4];
	left[0] = pos[0];
	left[1] = pos[4];
	left[2] = pos[7];
	left[3] = pos[3];
	Point* right = new Point[4];
	right[0] = pos[1];
	right[1] = pos[2];
	right[2] = pos[6];
	right[3] = pos[5];
	mtlID = m_objLoader.FindMaterial(pmodel, "side");
	LambertianMaterial* mtlside = new LambertianMaterial(&pmodel->materials[mtlID]);
	FinitePlane* lp = new FinitePlane(mtlside, NULL, 4, left);
	planes.push_back(lp);
	FinitePlane* rp = new FinitePlane(mtlside, NULL, 4, right);
	planes.push_back(rp);

	Point* top = new Point[4];
	top[0] = pos[2];
	top[1] = pos[3];
	top[2] = pos[7];
	top[3] = pos[6];
	mtlID = m_objLoader.FindMaterial(pmodel, "top");
	LambertianMaterial* mtltop = new LambertianMaterial(&pmodel->materials[mtlID]);
	FinitePlane* tp = new FinitePlane(mtltop, NULL, 4, top);
	planes.push_back(tp);

	Point* bottom = new Point[4];
	bottom[0] = pos[0];
	bottom[1] = pos[1];
	bottom[2] = pos[5];
	bottom[3] = pos[4];
	mtlID = m_objLoader.FindMaterial(pmodel, "bottom");
	LambertianMaterial* mtlbot = new LambertianMaterial(&pmodel->materials[mtlID]);
	FinitePlane* mp = new FinitePlane(mtlbot, NULL, 4, bottom);
	planes.push_back(mp);

}

void Group::ImportOBJ(char* filename)
{
	Mmodel* pmodel = new Mmodel();
	m_objLoader.ImportObj(pmodel, filename);
	if ( !AddModel(pmodel) )
		fprintf(stderr, "ImportOBJ(): Add model failed!\n");
	printf("Scene name: %s\n", filename);
	printf("Vertics: %d\n", pmodel->numvertices);
	printf("Faces: %d\n\n", pmodel->numfaces);

	CalculateCloseBox();
	SetSceneFace(pmodel);
	delete	pmodel;
}

bool Group::CheckHit(Ray &ray)
{
	double t, rt, tmin, tmax;
	double bmin, bmax;

	// 先与包围盒求交
	t = INFINITY;
	Point orig = ray.GetOrigin();
	Vector dir = ray.GetDirection();
	for (int i = 0; i < 3; i++) {
		if (fabs(dir.d[i]) > 0) {
			bmin = dir.d[i] > 0 ? CBPmin.d[i] : CBPmax.d[i];
			bmax = dir.d[i] > 0 ? CBPmax.d[i] : CBPmin.d[i];
			tmin = (bmin - orig.d[i]) / dir.d[i];
			tmax = (bmax - orig.d[i]) / dir.d[i];
			if (tmin > EPSILON)
				rt = tmin;
			else if (tmax > EPSILON)
				rt = tmax;
			else
				return	false;
			t = MIN(t, rt);
		}
		else {
			if ((orig.d[i] < CBPmin.d[i]) && (orig.d[i] > CBPmax.d[i]))
				return	false;
		}
	}
	if ((t > EPSILON) && (t < ray.hitDist))		//// 光线与包围盒有交且其交不在已求物体交的后面
		return true;

	return	false;
}

bool Group::Intersect(Ray &ray, double &tmin, double &tmax)
{
	double coef = 1.1;
	Point start = ray.GetOrigin();
	Vector direction = ray.GetDirection();
	double t0, t1;
	double minX = CBPmin.X() * coef;
	double maxX = CBPmax.X() * coef;
	double minY = CBPmin.Y() * coef;
	double maxY = CBPmax.Y() * coef;
	double minZ = CBPmin.Z() * coef;
	double maxZ = CBPmax.Z() * coef;
	/* 首先计算与YZ平面的相交情况 */
	if (fabs(direction.X()) < EPSILON)
		if (start.X() < minX || start.X() > maxX)
			return false;
	tmin = (minX - start.X()) / direction.X();
	tmax = (maxX - start.X()) / direction.X();

	if (tmin > tmax)
	{
		double temp = tmin;
		tmin = tmax;
		tmax = temp;
	}

	if (tmax < 0.0)
		return false;
	//tmin = MAX(tmin, 0);

	/* 其次计算与XZ平面的相交情况 */
	if (fabs(direction.Y()) < EPSILON)
		if (start.Y() < minY || start.Y() > maxY)
			return false;
	t0 = (minY - start.Y()) / direction.Y();
	t1 = (maxY - start.Y()) / direction.Y();

	if (t0 > t1)
	{
		double temp = t0;
		t0 = t1;
		t1 = temp;
	}
	if (t1 < 0.0)
		return false;
	
	if (t0 > tmin)
		tmin = t0;
	if (t1 < tmax)
		tmax = t1;

	/* 最后计算与XY平面的相交情况 */
	if (fabs(direction.Z()) < EPSILON)
		if (start.Z() < minZ || start.Z() > maxZ)
			return false;
	t0 = (minZ - start.Z()) / direction.Z();
	t1 = (maxZ - start.Z()) / direction.Z();

	if (t0 > t1)
	{
		double temp = t0;
		t0 = t1;
		t1 = temp;
	}
	if (t1 < 0.0)
		return false;

	if (t0 > tmin)
		tmin = t0;
	if (t1 < tmax)
		tmax = t1;

	return true;

	//double t1, t2;
	//double bmin, bmax;

	//double rtmin = INFINITY;
	//double rtmax = INFINITY;
	//Point orig = ray.GetOrigin();
	//Vector dir = ray.GetDirection();
	//for (int i = 0; i < 3; i++) {
	//	if (fabs(dir.d[i]) > EPSILON) {
	//		bmin = dir.d[i] > 0 ? CBPmin.d[i] : CBPmax.d[i];
	//		bmax = dir.d[i] > 0 ? CBPmax.d[i] : CBPmin.d[i];
	//		t1 = (bmin - orig.d[i]) / dir.d[i];
	//		t2 = (bmax - orig.d[i]) / dir.d[i];
	//		if (t2 < EPSILON)	return false;
	//		if (t1 > EPSILON)
	//			rtmin = MIN(t1, rtmin);
	//		if (tmax > EPSILON)
	//			rtmax = MIN(t2, rtmax);
	//	}
	//	else {
	//		if ((orig.d[i] < CBPmin.d[i]) && (orig.d[i] > CBPmax.d[i]))
	//			return	false;
	//	}
	//}
	//if (rtmin > rtmax)
	//	rtmin = 0;
	//tmin = rtmin;
	//tmax = rtmax;
	//return true;
	
 }

void Group::CalculateCloseBox(void)
{
	if (objs.empty())	return;

	CBPmin = Point(MAXCOORD, MAXCOORD, MAXCOORD);
	CBPmax = Point(MINCOORD, MINCOORD, MINCOORD);
	CBPcenter = Point(0, 0, 0);
	for(vector<Triangle *>::const_iterator iter = objs.begin(); iter != objs.end(); ++iter) {
		(*iter)->CalculateCloseBox();
		CBPmin = Min(CBPmin, (*iter)->CBPmin);
		CBPmax = Max(CBPmax, (*iter)->CBPmax);
		CBPcenter += (*iter)->CBPcenter;
	}
	CBPcenter = CBPcenter / (double) objs.size();

	return;
}

