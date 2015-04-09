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

	// ��Ҫ������ļ�ָ��
	FILE* m_FilePointer;

	// ��������
	int MaterialID;

	// ��������
	vector< Point >  m_pVertices;

	// UV��������
	vector< Vector >  m_pTextureCoords;

	// ���㷨������
	vector< Vector > m_pVertNormals;

	// ������
	vector< Face > m_pFaces;

	// ��ǰ�����Ƿ������������
	bool m_bObjectHasUV;

	// ��ǰ�����Ƿ���ж��㷨��
	bool m_bObjectHasNoraml;

	// ��ǰ����Ķ����Ƿ�����
	bool m_bJustReadAFace;

	// Operations

public:
	CObjLoader(void);
	~CObjLoader(void);
	//  ��obj�ļ��е���Ϣ���뵽ģ����
	bool ImportObj(Mmodel* pModel, char* filename);

	//  �����ļ�����Ŀ¼
	char* GetDirname(char* path);

	//  ���Ҳ���ID��
	int FindMaterial(Mmodel* pModel, char* name);

	// ���������ImportObj()�н����øú���
	void ReadObjFile(Mmodel* pModel);

	// ��������ļ�
	void ReadMtlFile(Mmodel* pModel, char* name);

	// ���붥����Ϣ����ReadObjFile()�е��øú���
	void ReadVertexInfo();

	// ��������Ϣ����ReadObjFile()�е��øú���
	void ReadFaceInfo();

	// ���㶥��ķ�����
	void ComputeNormals(Mmodel* pModel);

	// �������Ϣ�Ķ���֮����øú���
	void FillInObjectInfo(Mmodel* pModel);
};

#endif
