#ifndef _OBJINFO_H_
#define _OBJINFO_H_

#pragma once
#include "vector.h"
#include "Point.h"
#include "Color.h"
#include <vector>
using namespace std;

//  ��Ľṹ����
typedef struct _Face
{
	int MaterialID;				 // ��������
	int vertIndex[3];			// ��������
	int coordIndex[3];			// ������������
	int normalIndex[3];			// ���㷨��
} Face;

//GLMmaterial: Structure that defines a material in a model. 
typedef struct _Mmaterial
{
	char* name;						  /* name of material */
	double ambient[3];				  /* KA	ambient component */
	double diffuse[3];				  /* KD diffuse component */
	double specular[3];				  /* KS specular component */
	double trans[3];				 /* KT transparent	  */
	double illum;					  /* ILLUM �߹�ָ�� */	
	double shininess;				  /* NS ����� */			
	double transparent;				  // d/Tr
	bool   bTexture;
} Mmaterial;

class Mmodel{
public:
	char*    pathname;				 /* ģ������ */
	char*    mtllibname;			 /* name of the material library ģ�Ͳ����ļ�������*/

	int   numvertices;				/* ��������*/
	int   numnormals;				/* number of normals in model ���㷨�ߵ�����*/
	int   numfaces;					/* ������� */
	int   nummaterials;				/* number of materials in model */

	vector < Point >    vertices;	 /* array of vertices*/
	vector < Vector >   normals;	 /* array of normals ���㷨�ߵ�ֵ*/
	vector < Face >     surface;     /* ����Ϣ�б� */
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