#pragma once

#ifndef _OBJ_INFO_H_
#define _OBJ_INFO_H_

#include "Vector3.h"
#include <vector>
using namespace std;

//  面的结构定义
struct tFace
{
	int vertIndex[3];			// 顶点索引
	int coordIndex[3];			// 纹理坐标索引
	int normalIndex[3];			// 顶点法线
};

//  材质信息结构体
struct tMaterialInfo
{
	char  strName[255];			// 纹理名称
	char  strFile[255];			// 如果存在纹理映射，则表示纹理文件名称
	BYTE  color[3];				// 对象的RGB颜色
	int   texureId;				// 纹理ID
	float uTile;				// u 重复
	float vTile;				// v 重复
	float uOffset;			    // u 纹理偏移
	float vOffset;				// v 纹理偏移
} ;

//  对象信息结构体
struct t3DObject				
{
	int  numOfVerts;			// 模型中顶点的数目
	int  numTexVertex;			// 模型中纹理坐标的数目
	int  numVertNormal;			// 模型中顶点法线的数目
	int  numOfFaces;			// 模型中面的数目
	int  materialID;			// 纹理ID
	bool bHasTexture;			// 是否具有纹理映射
	char strName[255];			// 对象的名称
	CFVector3  *pVerts;			// 对象的顶点
	CFVector3  *pNormals;		// 对象顶点的法向量
	CFVector2  *pTexVerts;		// 纹理UV坐标
	tFace *pFaces;				// 对象的面信息
};

//  模型信息结构体
struct t3DModel 
{
	int numOfObjects;					// 模型中对象的数目
	int numOfMaterials;					// 模型中材质的数目
	vector<tMaterialInfo> pMaterials;	// 材质链表信息
	vector<t3DObject> pObject;			// 模型中对象链表信息
};

#endif