#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_

#pragma once
#include "ObjInfo.h"
#include "vector.h"
#include <fstream>

class CObjLoader
{
	// Attributes
private:

	// 需要读入的文件指针
	FILE* m_FilePointer;

	// 材质索引
	int MaterialID;

	// 顶点链表
	vector< Point >  m_pVertices;

	// UV坐标链表
	vector< Vector >  m_pTextureCoords;

	// 顶点法线链表
	vector< Vector > m_pVertNormals;

	// 面链表
	vector< Face > m_pFaces;

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
	bool ImportObj(Mmodel* pModel, char* filename);

	//  查找文件所在目录
	char* GetDirname(char* path);

	//  查找材质ID号
	int FindMaterial(Mmodel* pModel, char* name);

	// 读入对象，在ImportObj()中将调用该函数
	void ReadObjFile(Mmodel* pModel);

	// 读入材质文件
	void ReadMtlFile(Mmodel* pModel, char* name);

	// 读入顶点信息，在ReadObjFile()中调用该函数
	void ReadVertexInfo();

	// 读入面信息，在ReadObjFile()中调用该函数
	void ReadFaceInfo();

	// 计算顶点的法向量
	void ComputeNormals(Mmodel* pModel);

	// 完成面信息的读入之后调用该函数
	void FillInObjectInfo(Mmodel* pModel);
};

#endif
