#pragma once

#ifndef _OBJ_INFO_H_
#define _OBJ_INFO_H_

#include "Vector3.h"
#include <vector>
using namespace std;

//  ��Ľṹ����
struct tFace
{
	int vertIndex[3];			// ��������
	int coordIndex[3];			// ������������
	int normalIndex[3];			// ���㷨��
};

//  ������Ϣ�ṹ��
struct tMaterialInfo
{
	char  strName[255];			// ��������
	char  strFile[255];			// �����������ӳ�䣬���ʾ�����ļ�����
	BYTE  color[3];				// �����RGB��ɫ
	int   texureId;				// ����ID
	float uTile;				// u �ظ�
	float vTile;				// v �ظ�
	float uOffset;			    // u ����ƫ��
	float vOffset;				// v ����ƫ��
} ;

//  ������Ϣ�ṹ��
struct t3DObject				
{
	int  numOfVerts;			// ģ���ж������Ŀ
	int  numTexVertex;			// ģ���������������Ŀ
	int  numVertNormal;			// ģ���ж��㷨�ߵ���Ŀ
	int  numOfFaces;			// ģ���������Ŀ
	int  materialID;			// ����ID
	bool bHasTexture;			// �Ƿ��������ӳ��
	char strName[255];			// ���������
	CFVector3  *pVerts;			// ����Ķ���
	CFVector3  *pNormals;		// ���󶥵�ķ�����
	CFVector2  *pTexVerts;		// ����UV����
	tFace *pFaces;				// ���������Ϣ
};

//  ģ����Ϣ�ṹ��
struct t3DModel 
{
	int numOfObjects;					// ģ���ж������Ŀ
	int numOfMaterials;					// ģ���в��ʵ���Ŀ
	vector<tMaterialInfo> pMaterials;	// ����������Ϣ
	vector<t3DObject> pObject;			// ģ���ж���������Ϣ
};

#endif