#pragma once

#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include "stdafx.h"
#include "Vector3.h"
#include "ObjInfo.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

class CObjLoader
{
// Attributes
private:

	// 需要读入的文件指针
	FILE *m_FilePointer;

	// 顶点链表
	vector< CFVector3 >  m_pVertices;

	// UV坐标链表
	vector<CFVector2>  m_pTextureCoords;

	// 顶点法线链表
	vector< CFVector3 > m_pVertNormals;

	// 面链表
	vector<tFace> m_pFaces;

	// 当前对象是否具有纹理坐标
	bool m_bObjectHasUV;

	// 当前对象是否具有顶点法线
	bool m_bObjectHasNoraml;

	// 当前读入的对象是否是面
	bool m_bJustReadAFace;

// Operations

public:
	CObjLoader(void);
	~CObjLoader(void);
	//  将obj文件中的信息读入到模型中
	BOOL ImportObj(t3DModel *pModel, CString strFileName);

	// 读入对象，在ImportObj()中将调用该函数
	void ReadObjFile(t3DModel *pModel);

	// 读入顶点信息，在ReadObjFile()中调用该函数
	void ReadVertexInfo();

	// 读入面信息，在ReadObjFile()中调用该函数
	void ReadFaceInfo();

	// 完成面信息的读入之后调用该函数
	void FillInObjectInfo(t3DModel *pModel);

	// 计算顶点的法向量
	void ComputeNormals(t3DObject *pObject);

	void GetObjInfo(t3DModel model, CFVector3 &center, float &scale);
};

#endif
