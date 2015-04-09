#ifndef _OBJINFO_H_
#define _OBJINFO_H_

#pragma once
#include "vector.h"
#include "Point.h"
#include "Color.h"
#include <vector>
using namespace std;

//  面的结构定义
typedef struct _Face
{
	int MaterialID;				 // 材质索引
	int vertIndex[3];			// 顶点索引
	int coordIndex[3];			// 纹理坐标索引
	int normalIndex[3];			// 顶点法线
} Face;

//GLMmaterial: Structure that defines a material in a model. 
typedef struct _Mmaterial
{
	char* name;						  /* name of material */
	double ambient[3];				  /* KA	ambient component */
	double diffuse[3];				  /* KD diffuse component */
	double specular[3];				  /* KS specular component */
	double trans[3];				 /* KT transparent	  */
	double illum;					  /* ILLUM 高光指数 */	
	double shininess;				  /* NS 反光度 */			
	double transparent;				  // d/Tr
	bool   bTexture;
} Mmaterial;

class Mmodel{
public:
	char*    pathname;				 /* 模型名字 */
	char*    mtllibname;			 /* name of the material library 模型材质文件的名字*/

	int   numvertices;				/* 顶点数量*/
	int   numnormals;				/* number of normals in model 顶点法线的数量*/
	int   numfaces;					/* 面的数量 */
	int   nummaterials;				/* number of materials in model */

	vector < Point >    vertices;	 /* array of vertices*/
	vector < Vector >   normals;	 /* array of normals 顶点法线的值*/
	vector < Face >     surface;     /* 面信息列表 */
	Mmaterial*		    materials;	 /* array of materials */

	//int   MaterialID;				 /* index of materials */
	//Point position;					 /* position of the model */

public:
	Mmodel(void) {
		pathname = NULL;
		mtllibname = NULL;

		numvertices = 0;
		numnormals = 0;
		numfaces = 0;
		nummaterials = 0;

		vertices.clear();
		normals.clear();
		surface.clear();
		materials = NULL;

		//MaterialID = -1;
		//position = Point(0, 0, 0);
	};
	~Mmodel(void) {
		if (pathname)
			delete pathname;
		if (mtllibname)
			delete mtllibname;
		if (materials)
			delete materials;

		vertices.clear();
		normals.clear();
		surface.clear();
	};
};

#endif