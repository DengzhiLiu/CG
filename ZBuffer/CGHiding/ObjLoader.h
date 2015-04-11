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

	// ��Ҫ������ļ�ָ��
	FILE *m_FilePointer;

	// ��������
	vector< CFVector3 >  m_pVertices;

	// UV��������
	vector<CFVector2>  m_pTextureCoords;

	// ���㷨������
	vector< CFVector3 > m_pVertNormals;

	// ������
	vector<tFace> m_pFaces;

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
	BOOL ImportObj(t3DModel *pModel, CString strFileName);

	// ���������ImportObj()�н����øú���
	void ReadObjFile(t3DModel *pModel);

	// ���붥����Ϣ����ReadObjFile()�е��øú���
	void ReadVertexInfo();

	// ��������Ϣ����ReadObjFile()�е��øú���
	void ReadFaceInfo();

	// �������Ϣ�Ķ���֮����øú���
	void FillInObjectInfo(t3DModel *pModel);

	// ���㶥��ķ�����
	void ComputeNormals(t3DObject *pObject);

	void GetObjInfo(t3DModel model, CFVector3 &center, float &scale);
};

#endif
