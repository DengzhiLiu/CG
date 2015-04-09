#include "Image.h"
#include "Camera.h"
#include "Vector.h"
#include "Point.h"
#include "Color.h"
#include "Scene.h"
#include "Sphere.h"
#include "BSPTree.h"
#include "FinitePlane.h"
//#include "InfinitePlane.h"+
//#include "ConstantColorMaterial.h"
#include "LambertianMaterial.h"
#include "Light.h"
#include "ObjInfo.h"
#include <stdio.h>
#include <stdlib.h>

// Scene setup for the ray tracing assignment.
void SetupHomeworkScene( Scene *scene )
{
    // Setup some materials to go with the geometry

    // Create a group, add the geometry to this group, then add this group to the scene.
    Group* world = new Group();
	printf("And OBJS.....\n");
	world->ImportOBJ("./Data/scene.obj");
	CDIB* pdib = new CDIB();
	printf("And Spheres and Texture.....\n");
	pdib->readBMP("./Data/worldmap.bmp");
	LambertianMaterial* matl = new LambertianMaterial();
	matl->bTexture = true;
	Sphere *sp = new Sphere(matl, pdib, Point(150, -23, 150), 80);
	scene->AddSphere(sp);

	//world->Add( new Sphere( green, Point(1, 1,-3), 3 ) );
	scene->SetGeometry(world);
	scene->SetFace(world);
	scene->BuildTree();

	// Auto set the scene's camera and lights
	scene->SetCamera();
	// 左面
	Point pos1 = Point(world->CBPmin.X()+100, world->CBPmax.Y()+300, world->CBPmax.Z()+300);
	// 右面
	Point pos2 = Point(world->CBPmax.X()-100, world->CBPmax.Y()+300, world->CBPmax.Z()+300);
	// 后面
	Point pos3 = Point(world->CBPcenter.X(), world->CBPmax.Y()+300, world->CBPmin.Z()+100);
	// 前面
	Point pos4 = Point(world->CBPcenter.X(), world->CBPmax.Y()+300, world->CBPmax.Z()+300);
	//scene->AddLight( new Light(pos1, Color(0.6, 0.6, 0.6)) );
	scene->AddLight( new Light(pos4, Color(0.6, 0.6, 0.6)) );
	return	;
}

