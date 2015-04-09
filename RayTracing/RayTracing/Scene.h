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
	/* 改进时可以在每个包围盒中设定物体数量的阈值
	改进的时候可以考虑将vector数组结构改为链表结构，vector结构有时可能会不太稳定*/
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
	// 记录上一条由视点出发的光线以及它产生的每条发射光线、折射光线所交的第一个景物,以减少求交的次数
	Object* FirstInterObj[1025];
	KdTreeAccel* KDTool;
	BSPTree* BSPtool;

	double thresh;				// 光的贡献率的最小阈值
	u_int maxRayDepth;			// 每条光线的最大跟踪层数
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
