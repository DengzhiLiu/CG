#include "BSPTree.h"
#include "Material.h"
#include "Triangle.h"

BSPTree::BSPTree()
{
	minnum = 8;
	maxdepth = 10;
}

BSPTree::~BSPTree(void)
{
}

// �ж�һ���Ƿ���������
inline bool BSPTree::IsInBox(Point pos, Point Bmin, Point Bmax)
{
	if (pos.X() < Bmin.X() || pos.X() > Bmax.X())
		return	false;
	if (pos.Y() < Bmin.Y() || pos.Y() > Bmax.Y())
		return	false;
	if (pos.Z() < Bmin.Z() || pos.Z() > Bmax.Z())
		return	false;
	return	true;
}

// ������Ƭ�Ƿ��������ཻ
inline bool BSPTree::IsIntersectBox(Point* pos, Point Bmin, Point Bmax)
{
	Vector e1 = pos[1] - pos[0];
	Vector e2 = pos[2] - pos[0];
	Vector d = Bmin - pos[0];
	Vector L = Bmax - Bmin;

	for (int i = 0; i < 3; i++)	{
		if (fabs(e1.d[i]) < fabs(e2.d[i])) {	// ��֤e1i > e2i
			Vector temp = e1;
			e1 = e2;
			e2 = temp;
		}
		if ( SolutionExists(e1, e2, d, L, i) )
			return	true;
		d.d[i] += L.d[i];
		if ( SolutionExists(e1, e2, d, L, i) )
			return	true;
	}

	return	false;
}

inline bool BSPTree::SolutionExists(Vector e1, Vector e2, Vector d, Vector L, int axis)
{
	double min = 0, max = 0;
	double tempMin, tempMax;

	double e1i = e1.d[axis];
	double e1j = e1.d[(axis+1)%3];
	double e1k = e1.d[(axis+2)%3];
	double e2i = e2.d[axis];
	double e2j = e2.d[(axis+1)%3];
	double e2k = e2.d[(axis+2)%3];
	double di = d.d[axis];
	double dj = d.d[(axis+1)%3];
	double dk = d.d[(axis+2)%3];
	double Li = L.d[axis];
	double Lj = L.d[(axis+1)%3];
	double Lk = L.d[(axis+2)%3];

	double deti = e1i - e2i;
	double detj = (e1i * e2j) - (e1j * e2i);
	double detk = (e1i * e2k) - (e1k * e2i);
	double b0 = 1.0 / e2i;
	double bi = 1.0 / deti;
	double bj = e1j / detj;
	double bk = e1k / detk;
	
	double a0 = e1i / e2i;
	double ai = e1i / deti;
	double aj = e1i / detj;
	double ak = e1i / detk;
	double C0 = b0 * di;
	double Ci = -bi * di;
	double Cj = aj * dj - bj * di;
	double Ck = ak * dk - bk * di;

	/* ����һ������ʽ */
	if (fabs(e2i) > EPSILON)	{			// e2i != 0
		tempMax = C0;
		if (a0 > 0.0) {
			if (tempMax < min)
				return false;
			if (tempMax < max)
				max = tempMax;
		}
		else {
			tempMin = tempMax;
			if (tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
		}
	}
	else {								// �쳣���1��e2i == 0
		if (di / e1i < 0.0)
			return false;
	}

	/* ���ڶ�������ʽ */
	if (fabs(deti) > EPSILON) {			// deti != 0
		tempMax = Ci + ai;
		if (ai > 0.0) {
			if (tempMax < min)
				return false;
			if (tempMax < max)
				max = tempMax;
		}
		else {
			tempMin = tempMax;
			if (tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
		}
	}
	else {								// �쳣���2��deti == 0
		if (e1i > 0.0) {
			if (e1i - di < 0.0)
				return false;
		}
		else {
			if (e1i - di > 0.0)
				return false;
		}
	}

	/* ������������ʽ */
	if (fabs(detj) > EPSILON) {		// detj != 0
		tempMin = Cj;
		tempMax = tempMin + aj * Lj;
		if (aj > 0.0) {
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
		else {
			double temp = tempMin;
			tempMin = tempMax;
			tempMax = temp;
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
	}
	else {								// �쳣���3��detj == 0
		if (e1i > 0.0) {
			if (dj * e1i - e1j * di > 0.0)
				return false;
			if (dj * e1i - e1j * di + Lj * e1i < 0.0)
				return false;
		}
		else {
			if (dj * e1i - e1j * di < 0.0)
				return false;
			if (dj * e1i - e1j * di + Lj * e1i > 0.0)
				return false;
		}
	}

	/* �����ĸ�����ʽ */
	if (fabs(detk) > EPSILON) {			// detk != 0
		tempMin = Ck;
		tempMax = tempMin + ak * Lk;
		if (ak > 0.0) {
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
		else {
			double temp = tempMin;
			tempMin = tempMax;
			tempMax = temp;
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
	}
	else {								// �쳣���4��detk == 0
		if (e1i > 0) {
			if (dk * e1i - e1k * di > 0.0)
				return false;
			if (dk * e1i - e1k * di + Lk * e1i < 0.0)
				return false;
		}
		else {
			if (dk * e1i - e1k * di < 0.0)
				return false;
			if (dk * e1i - e1k * di + Lk * e1i > 0.0)
				return false;
		}
	}

	return true;
}

BSPNode* BSPTree::BuildBSPTree(Group* group, int depth, int axis)
{
	if (group == NULL)	return	NULL;
	
	BSPNode* root = new BSPNode;
	Group *leftchild, *rightchild;

	root->subAxis = axis;
	root->lchild = root->rchild = NULL;
	// ��������Ƭ��������ֵ�򳬳����ʱ
	int num = group->GetSize();
	if (num <= minnum || depth >= maxdepth) {
		if (num > 0) {
			root->objList = group;
			return	root;
		}
		else
			return	NULL;
	}

	bool f[6];
	Point pos[3];
	int nextAxis;				// ��һ���ʷֵ�����
	Point Bmin[2], Bmax[2];

	Bmin[0] = Bmin[1] = group->CBPmin;
	Bmax[0] = Bmax[1] = group->CBPmax;
	Bmin[1].d[axis] = (Bmin[0].d[axis] + Bmax[0].d[axis]) * 0.5;
	Bmax[0].d[axis] = Bmin[1].d[axis];
	nextAxis = (axis + 1) % 3;

	root->objList = group;
	leftchild = new Group();
	leftchild->CBPmin = Bmin[0];
	leftchild->CBPmax = Bmax[0];
	rightchild = new Group();
	rightchild->CBPmin = Bmin[1];
	rightchild->CBPmax = Bmax[1];

	// ֻ������������
	for (vector<Triangle *>::const_iterator iter = group->objs.begin(); iter != group->objs.end(); ++iter) {
		for (int i = 0; i < 3; i++)
			pos[i] = ((Triangle *)(*iter))->varray[i];
		for (int i = 0; i < 6; i++)
			f[i] = IsInBox(pos[i%3], Bmin[i/3], Bmax[i/3]);
		
		if (f[0] && f[1] && f[2]) {				// �������㶼��������
			leftchild->Add(*iter);
			continue;
		}
		if (f[3] && f[4] && f[5]) {				// �������㶼���Һ�����
			rightchild->Add(*iter);
			continue;
		}

		if (f[0] || f[1] || f[2]) {				// һ�����������ӣ�����������
			leftchild->Add(*iter);
		}
		else if ( IsIntersectBox(pos, Bmin[0], Bmax[0]) ) {
			leftchild->Add(*iter);
		}

		if (f[3] || f[4] || f[5]) {				// һ���������Һ��ӣ��������Һ���
			rightchild->Add(*iter);
		}
		else if ( IsIntersectBox(pos, Bmin[1], Bmax[1]) ) {
			rightchild->Add(*iter);  
		}
	}

	root->lchild = BuildBSPTree(leftchild, depth+1, nextAxis);
	root->rchild = BuildBSPTree(rightchild, depth+1, nextAxis);

	return	root;
}

bool BSPTree::Intersect(Ray &ray, BSPNode* root, double tmin, double tmax, bool flag, Color &factor)
{
	bool found = false;
	BSPNode *nearNode, *farNode;

	// ����Ĵ��뻹Ҫ�޸�һ��
	if (root == NULL)	return	false;
	Group* group = root->objList;
	if ((group == NULL) || (group->GetSize() <= 0))		return	false;
	// �ȶ԰�Χ�н����󽻲���, ���趨����ķ�Χ����tmin~tmax
	//if ( !group->CheckHit(ray) )
	//	return	false;

	if ((root->lchild == NULL) && (root->rchild == NULL)) {
		// �뾰����Ƭ�����󽻲���
		for (vector<Triangle *>::const_iterator iter = group->objs.begin(); iter != group->objs.end(); ++iter) {
			if ( !(*iter)->Intersect(ray, tmin, tmax) )		continue;
			if ( !flag ) {
				factor = factor * (*iter)->GetMaterial()->kt;
				if (factor.r < EPSILON || factor.g < EPSILON || factor.b < EPSILON)	return	true;
			}
		}
		if (flag && ray.WasIntersection())	found = true;
		return found;
	}

	// �������������Ľ������tdist
	double tdist;
	Point Bmin, Bmax;
	int axis = root->subAxis;			// ��ǰ�����ʷ���
	Bmin = group->CBPmin;
	Bmax = group->CBPmax;
	double startCood = ray.GetOrigin().d[axis];
	double dirlen = ray.GetDirection().d[axis];
	double subCoord = (Bmin.d[axis] + Bmax.d[axis]) * 0.5;
	if (fabs(subCoord - startCood) < EPSILON)
		tdist = 0.0;
	else if ((dirlen < EPSILON) && (dirlen > 0.0))
		tdist = MAXCOORD;
	else if ((dirlen > -EPSILON) && (dirlen < 0.0))
		tdist = MINCOORD;
	else
		tdist = (subCoord - startCood) / dirlen;
	if (fabs(subCoord - startCood) < EPSILON) {
		double temp = startCood + dirlen;
		if (temp < subCoord) {
			nearNode = root->rchild;		// ��һ�δ��ҽ�㿪ʼ
			farNode = root->lchild;
		}
		else {
			nearNode = root->lchild;
			farNode = root->rchild;
		}
	}
	else {
		if (startCood < subCoord) {
			nearNode = root->lchild;
			farNode = root->rchild;
		}
		else {
			nearNode = root->rchild;
			farNode = root->lchild;
		}
	}

	if ((tdist > tmax) || (tdist < 0.0))
		found = Intersect(ray, nearNode, tmin, tmax, flag, factor);
	else if (tdist < tmin)
		found = Intersect(ray, farNode, tmin, tmax, flag, factor);
	else {
		found = Intersect(ray, nearNode, tmin, tdist, flag, factor);
		if ( found && ray.hitDist <= tmax && ray.hitDist >= tmin )	return	found;
		found = Intersect(ray, farNode, tdist, tmax, flag, factor);
	}
	return	found;
}
