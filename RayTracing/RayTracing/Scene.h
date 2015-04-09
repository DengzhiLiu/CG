#ifndef _SCENE_H_
#define _SCENE_H_

#include "Color.h"
#include "Group.h"
#include "Object.h"
#include "BSPTree.h"
#include "FinitePlane.h"
#include "Sphere.h"
#include "Definitions.h"
#include "Camera.h"
#include "KDTree.h"

class Ray;
class Light;
class Group;

typedef struct BVHTreeNode 
{
	/* �Ľ�ʱ������ÿ����Χ�����趨������������ֵ
	�Ľ���ʱ����Կ��ǽ�vector����ṹ��Ϊ����ṹ��vector�ṹ��ʱ���ܻ᲻̫�ȶ�*/
	Group* geometry;
	BVHTreeNode* lchild;
	BVHTreeNode* rchild;
} BVHTree;

class Scene {
public:
    Camera* camera;
	Group* geometry;
	vector<Sphere *> spherelist;
	vector<FinitePlane *> planelist;
	BSPNode* BSPRoot;
    vector<Light *> light;
	// ��¼��һ�����ӵ�����Ĺ����Լ���������ÿ��������ߡ�������������ĵ�һ������,�Լ����󽻵Ĵ���
	Object* FirstInterObj[1025];
	KdTreeAccel* KDTool;
	BSPTree* BSPtool;

	double thresh;				// ��Ĺ����ʵ���С��ֵ
	u_int maxRayDepth;			// ÿ�����ߵ������ٲ���
	Color ambient;
    Color backgroundColor;
public:
    Scene();
    ~Scene();

    // Get/set the camera
    inline Camera *GetCamera( void ) { return camera; }
    void SetCamera( void );

    // Get the width and height of the image
    inline int GetWidth( void ) { return camera->GetScreenWidth(); }
    inline int GetHeight( void ) { return camera->GetScreenHeight(); }

    // Get some default scene data (e.g., bgColor, rayDepth)
    inline Color GetBackgroundColor( void ) const { return backgroundColor; }
    inline int GetMaxRayDepth( void ) const { return maxRayDepth; }

    // Get/set the geometry.  Note, this is usually a group-type object, not a primitive!
    inline Group *GetGeometry( void ) const { return geometry; }
    inline void SetGeometry( Group *obj ) { geometry = obj; }
	inline void SetFace( Group *obj ) { planelist = obj->planes; }
	inline void AddSphere(Sphere *obj) { spherelist.push_back(obj); }

    // Deal with scene lights
    inline int GetNumLights( void ) { return (int)light.size(); }
    inline void AddLight( Light *lght ) { light.push_back( lght ); }
    inline Light *GetLight( int i ) { return light[i]; }
	inline Color GetAmbient( void ) { return ambient; }

	void BuildTree();

    // Trace a ray through the scene and return a color
    Color RayTrace(Ray &r, int idx);
};

#endif
