#include "StdAfx.h"
#include "ObjLoader.h"
#include "ObjInfo.h"

CObjLoader::CObjLoader(void):
			m_bObjectHasUV(FALSE),
			m_bObjectHasNoraml(FALSE),
			m_bJustReadAFace(FALSE)
{
}

CObjLoader::~CObjLoader(void)
{
	//m_pVertices.clear();
	//m_pTextureCoords.clear();
	//m_pVertNormals.clear();

}

BOOL CObjLoader::ImportObj(t3DModel *pModel, CString strFileName)
{
	CString strMessage;				// ������ʾ������Ϣ

	// �ж��Ƿ���һ���Ϸ���ģ�ͺ��ļ�����
	if(!pModel || strFileName.IsEmpty()) return false;

	// ��ֻ����ʽ���ļ��������ļ�ָ��
	m_FilePointer = fopen(strFileName, "r");

	// �ж��ļ�ָ���Ƿ���ȷ
	if(!m_FilePointer) {
		// ����ļ�ָ�벻��ȷ������ʾ������Ϣ
		strMessage.Format("Unable to find or open the file: %s", strFileName.GetString());
		AfxMessageBox(strMessage);
		return false;
	}

	// �����ļ���Ϣ
	ReadObjFile(pModel);

	// �رմ򿪵��ļ�
	fclose(m_FilePointer);

	return true;
}

//  ����obj�ļ��еĶ���ģ����
void CObjLoader::ReadObjFile(t3DModel *pModel)
{
	char strLine[255]		= {0};
	char ch					= 0;

	while(!feof(m_FilePointer))
	{
		float x = 0.0f, y = 0.0f, z = 0.0f;

		// ���obj�ļ��еĵ�ǰ�еĵ�һ���ַ�
		ch = fgetc(m_FilePointer);

		switch(ch)
		{
		case 'v':						// �������'v' (���������ݿ����Ƕ���/������/��������)

			// �����ǰ������������У���ô���ڽ��������һ����������ڶ�����һ������֮ǰ��
			//  ��Ҫ�������������д�뵽ģ�ͽṹ����
			if(m_bJustReadAFace) 
			{
				// �����������Ϣ���浽ģ�ͽṹ��
				FillInObjectInfo(pModel);
			}

			// ��������Ϣ��Ҫ���ֶ��� ("v")�������� ("vn")���������� ("vt")
			ReadVertexInfo();
			break;

		case 'f':						// �������'f'

			//  ���������Ϣ
			ReadFaceInfo();
			break;

		default:

			// �Թ����е�����
			fgets(strLine, 100, m_FilePointer);
			break;
		}
	}

	// ����������Ķ���
	FillInObjectInfo(pModel);
}

//  ����ĺ������붥����Ϣ('v'��ָ���㣬'vt'ָUV����)
void CObjLoader::ReadVertexInfo()
{
	char ch = 0;
	char strLine[255] = {0};
	CFVector2 vNewTexCoord = {0};
	CFVector3 vNewVertex,	vNewNormal;

	// �����һ���ַ����ж϶�����Ƿ񶥵�/������/UV����
	ch = fgetc(m_FilePointer);

	if(ch == ' ')				// ����ǿո�����Ƕ���("v")
	{
		// ���붥�����꣬��ʽ��"v x y z"
		fscanf(m_FilePointer, "%f %f %f", &vNewVertex.x, &vNewVertex.y, &vNewVertex.z);

		// ������������µ����ݣ����ļ�ָ�뵽��һ��
		fgets(strLine, 100, m_FilePointer);

		// ���һ���µĶ��㵽����������
		m_pVertices.push_back(vNewVertex);
	}
	else if(ch == 't')			// �����'t'����ض�����������("vt")
	{
		// �����������꣬��ʽ��"vt u v"
		fscanf(m_FilePointer, "%f %f", &vNewTexCoord.x, &vNewTexCoord.y);

		// ����������µ����ݣ����ļ�ָ��ָ����һ��
		fgets(strLine, 100, m_FilePointer);

		// ���һ���µ��������굽������
		m_pTextureCoords.push_back(vNewTexCoord);

		//  ���ö��������������Ϊtrue
		m_bObjectHasUV = TRUE;
	}
	else if (ch == 'n')			// �����'n', ��ض��Ƕ��㷨��("vn")
	{
		// ���뷨����������ʽ��"vn x y z"
		fscanf(m_FilePointer, "%f %f %f", &vNewNormal.x, &vNewNormal.y, &vNewNormal.z);
		
		// ����������µ����ݣ����ļ�ָ��ָ����һ��
		fgets(strLine, 100, m_FilePointer);

		// ���һ���µķ������굽������
		m_pVertNormals.push_back(vNewNormal);

		//���ö�����ж��㷨��
		m_bObjectHasNoraml = TRUE;
	}
	else					// �������
	{
		// �������������ֱ��������һ��
		fgets(strLine, 100, m_FilePointer);
	}
}

//  ����ĺ�����������Ϣ
//	�����Ϣ "f ��������/uv������/��������"
void CObjLoader::ReadFaceInfo()
{
	tFace newFace			= {0};
	char strLine[255]		= {0};

	if(!m_bObjectHasUV && !m_bObjectHasNoraml)		// �����������޶��㷨��
	{
		// �������Ķ�����������ʽ��"����1 ����2 ����3"
		fscanf(m_FilePointer, "%d %d %d", &newFace.vertIndex[0],
			&newFace.vertIndex[1],
			&newFace.vertIndex[2]);				
	}
	else if(m_bObjectHasUV && !m_bObjectHasNoraml)		// �����������޶��㷨��
	{
		// �������Ķ��������������������ʽ��"����1/��������1 ����2/��������2 ����3/��������3"
		fscanf(m_FilePointer, "%d/%d %d/%d %d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0],
			&newFace.vertIndex[1], &newFace.coordIndex[1],
			&newFace.vertIndex[2], &newFace.coordIndex[2]);				
	}
	else if(!m_bObjectHasUV && m_bObjectHasNoraml)		// �����������ж��㷨��
	{
		// �������Ķ��������������������ʽ��"����1//���㷨��1 ����2//���㷨��2 ����3//���㷨��3"
		fscanf(m_FilePointer, "%d//%d %d//%d %d//%d", &newFace.vertIndex[0], &newFace.normalIndex[0],
			&newFace.vertIndex[1], &newFace.normalIndex[1],
			&newFace.vertIndex[2], &newFace.normalIndex[2]);				
	}
	else if (m_bObjectHasUV && m_bObjectHasNoraml)		// �����������ж��㷨��
	{
		// �������Ķ��㡢��������Ͷ��㷨��������
		// ��ʽ��"����1/��������1/���㷨��1 ����2/��������2/���㷨��2 ����3/��������3//���㷨��3"
		fscanf(m_FilePointer, "%d/%d/%d %d/%d/%d %d/%d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0], &newFace.normalIndex[0],
			&newFace.vertIndex[1], &newFace.coordIndex[1], &newFace.normalIndex[1],
			&newFace.vertIndex[2], &newFace.coordIndex[2], &newFace.normalIndex[2]);	
	}

	// ����������µ����ݣ����ļ�ָ��ָ����һ��
	fgets(strLine, 100, m_FilePointer);

	// �����0��ʼ������ȥ1
	for (int i = 0; i < 3; i++)
	{
		newFace.vertIndex[i]	-= 1;
		newFace.coordIndex[i]	-= 1;
		newFace.normalIndex[i]	-= 1;
	}
	// ���һ�����浽��������
	m_pFaces.push_back(newFace);

	//  ���øղŶ��������
	m_bJustReadAFace = true;
}

//  ����ĺ���������������Ϣд��ģ�ͽṹ����
void CObjLoader::FillInObjectInfo(t3DModel *pModel)
{
	t3DObject newObject = {0};
	int i = 0;

	// ģ���ж������������
	pModel->numOfObjects++;

	// ���һ���¶���ģ�͵Ķ���������
	pModel->pObject.push_back(newObject);

	// ��õ�ǰ�����ָ��
	t3DObject *pObject = &(pModel->pObject[pModel->numOfObjects - 1]);

	// ������������������������������������
	pObject->numOfVerts   = m_pVertices.size();
	pObject->numTexVertex = m_pTextureCoords.size();
	pObject->numVertNormal= m_pVertNormals.size();
	pObject->numOfFaces   = m_pFaces.size();

	// ��������˵�
	if(pObject->numOfVerts) {
		// ���䱣���Ĵ洢�ռ�
		pObject->pVerts = new CFVector3 [pObject->numOfVerts];

		// ���������е����е�
		for(i = 0; i < pObject->numOfVerts; i++)
		{
			// ����ǰ�Ķ������ʱ�����п�����ģ��������
			pObject->pVerts[i] = m_pVertices[i];
		}
	}	

	// �����������������
	if(m_bObjectHasUV) {
		pObject->pTexVerts = new CFVector2 [pObject->numTexVertex];
		//pObject->bHasTexture = true;

		// �������������е���������
		for(i = 0; i < pObject->numTexVertex; i++)
		{
			// ����ǰ�������������ʱ�����п�����ģ��������
			pObject->pTexVerts[i] = m_pTextureCoords[i];
		}
	}

	// �����������
	if(pObject->numOfFaces) 
	{
		// ���䱣����Ĵ洢�ռ�
		pObject->pFaces = new tFace [pObject->numOfFaces];

		// �������е���
		for(i = 0; i < pObject->numOfFaces; i++)
		{
			// ������ʱ��������ģ��������
			pObject->pFaces[i] = m_pFaces[i];
		}
	}

	// ���㶥���ķ�����,���ڹ���
	// ��������˶���ķ���
	if (m_bObjectHasNoraml)
	{
		pObject->pNormals = new CFVector3 [pObject->numVertNormal];

		// �������������еĶ��㷨��
		for(i = 0; i < pObject->numVertNormal; i++)
		{
			// ����ǰ�Ķ��㷨���������ʱ��������ģ��������
			pObject->pNormals[i] = m_pVertNormals[i];
		}
	}
	else // û�ж��붥�㷨������������ÿ������ķ���
	{
		ComputeNormals(pObject);
	}

	//  ����OBJ�ļ���û�в��ʣ���˽�materialID����Ϊ-1�������ֶ����ò���
	pObject->materialID = -1;

	//  ������е���ʱ����
	m_pVertices.clear();
	m_pFaces.clear();
	m_pTextureCoords.clear();

	// �������еĲ���ֵΪfalse
	m_bObjectHasUV		= false;
	m_bObjectHasNoraml	= false;
	m_bJustReadAFace	= false;
}

// ������Ҫ�������㶥��ķ�����������ķ�������Ҫ�����������
void CObjLoader::ComputeNormals(t3DObject *pObject)
{
	// ������󲻴��ڣ��򷵻�
	if(!pObject)		return;

	CFVector3 vVector1, vVector2, vNormal, vPoly[3];

	// ������Ҫ�Ĵ洢�ռ�
	//CFVector3 *pNormals		= new CFVector3 [pObject->numOfFaces];
	CFVector3 *pTempNormals	= new CFVector3 [pObject->numOfFaces];
	pObject->pNormals		= new CFVector3 [pObject->numOfVerts];

	// ���������������
	for(int i=0; i < pObject->numOfFaces; i++)
	{												
		vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
		vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
		vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

		// ������ķ�����

		vVector1 = vPoly[0] - vPoly[1];		// ��ö���ε�ʸ��
		vVector2 = vPoly[0] - vPoly[2];		// ��ö���εĵڶ���ʸ��

		vNormal  = vVector1 * vVector2;		// �������ʸ���Ĳ��
		pTempNormals[i] = vNormal;					// ����ǹ淶��������
		//pNormals[i] = vNormal / sqrt(vNormal.Norm());			// ���淶�������ӵ��������б���
	}

	//  �����󶥵㷨��
	int shared=0;
	CFVector3 vZero, vSum;
	// �������еĶ���
	for (int i = 0; i < pObject->numOfVerts; i++)			
	{
		for (int j = 0; j < pObject->numOfFaces; j++)	// �������е���������
		{												// �жϸõ��Ƿ����������湲��
			if (pObject->pFaces[j].vertIndex[0] == i || 
				pObject->pFaces[j].vertIndex[1] == i || 
				pObject->pFaces[j].vertIndex[2] == i)
			{
				vSum += pTempNormals[j];
				//vSum += pNormals[j];
				shared++;								
			}
		}      

		pObject->pNormals[i] = vSum / (float)shared;

		// �淶�����Ķ��㷨��
		pObject->pNormals[i] = pObject->pNormals[i] / sqrt(pObject->pNormals[i].Norm());	
		vSum = vZero;
	}

	// �ͷŴ洢�ռ䣬��ʼ��һ������
	delete [] pTempNormals;
	//delete [] pNormals;
}

void CObjLoader::GetObjInfo(t3DModel model, CFVector3 &object_center, float &object_scale)
{
	int	vertex_num = 0;
	t3DObject	object;
	CFVector3 object_min_xyz(1000.0, 1000.0, 1000.0);
	CFVector3 object_max_xyz(-1000.0, -1000.0, -1000.0);
	CFVector3 dis_xyz;
	object_min_xyz.x = object_min_xyz.y = object_min_xyz.z = 1000;
	object_max_xyz.x = object_max_xyz.y = object_max_xyz.z = -1000;
	if (model.numOfObjects == 0)			//ģ����û��obj�����򷵻�
		return	;
	for (int i = 0; i < model.numOfObjects; i++)
	{
		object = model.pObject[i];
		vertex_num += object.numOfVerts;
		for (int j = 0; j < object.numOfVerts; j++)
		{
			object_center += object.pVerts[j];
			object_min_xyz.x = min(object_min_xyz.x, object.pVerts[j].x);
			object_min_xyz.y = min(object_min_xyz.y, object.pVerts[j].y);
			object_min_xyz.z = min(object_min_xyz.z, object.pVerts[j].z);
			object_max_xyz.x = max(object_max_xyz.x, object.pVerts[j].x);
			object_max_xyz.y = max(object_max_xyz.y, object.pVerts[j].y);
			object_max_xyz.z = max(object_max_xyz.z, object.pVerts[j].z);
		}
	}
	dis_xyz = object_max_xyz - object_min_xyz;
	object_center = object_center / (float)vertex_num;
	object_scale = sqrt(dis_xyz.Norm());
}