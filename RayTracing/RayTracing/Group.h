#ifndef _GROUP_H
#define _GROUP_H

#include <vector>
#include "Ray.h"
#include "Triangle.h"
#include "FinitePlane.h"
#include "ObjInfo.h"
#include "ObjLoader.h"
using namespace std;

class Group : public Object {
public:
	CObjLoader m_objLoader;
    vector<Triangle *> objs;
	vector<FinitePlane *> planes;
public:
    Group();
    virtual ~Group();

	Triangle *Get( unsigned int i );
    void Add( Triangle *obj );
	int GetSize( void ) const;
	bool AddModel(Mmodel* pmodel);
	void ImportOBJ(char* filename);
	void SetSceneFace(Mmodel* pmodel);

	bool CheckHit(Ray &ray);
	virtual bool Intersect( Ray &ray, double &tmin, double &tmax );
	virtual void CalculateCloseBox();
};


#endif
