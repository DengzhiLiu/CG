#include "ObjLoader.h"
#include <string>
using namespace std;

CObjLoader::CObjLoader(void):
MaterialID(-1),
m_bObjectHasUV(false),
m_bObjectHasNoraml(false),
m_bJustReadAFace(false)
{
}

CObjLoader::~CObjLoader(void)
{
	if (m_pVertices.size())
		m_pVertices.clear();
	if (m_pTextureCoords.size())
		m_pTextureCoords.clear();
	if (m_pVertNormals.size())
		m_pVertNormals.clear();
}

bool CObjLoader::ImportObj(Mmodel* pModel, char* filename)
{
	// ��ֻ����ʽ���ļ��������ļ�ָ��
	m_FilePointer = fopen(filename, "r");
	pModel->pathname = _strdup(filename);

	// �ж��ļ�ָ���Ƿ���ȷ
	if(!m_FilePointer) {
		fprintf(stderr, "Unable to find or open file: %s", filename);
		return false;
	}

	// �����ļ���Ϣ
	ReadObjFile(pModel);

	// �رմ򿪵��ļ�
	fclose(m_FilePointer);

	return true;
}

char* CObjLoader::GetDirname(char* path)
{
	char* dir;
	char* s;

	dir = _strdup(path);
	s = strrchr(dir, '/');
	if (s)
		s[1] = '\0';
	else
		dir[0] = '\0';

	return dir;
}

// ��������ļ�
void CObjLoader::ReadMtlFile(Mmodel* pModel, char* name)
{
	FILE* file;
	char* dir;
	char* filename;
	char buf[128];
	int nummaterials;

	dir = GetDirname(pModel->pathname);			//�õ�Ŀ¼��λ��
	filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
	strcpy(filename, dir);
	strcat(filename, name);
	free(dir);

	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "ReadMtlFile() failed: can't open material file \"%s\".\n",
			filename);
		exit(1);
	}
	free(filename);

	/* count the number of materials in the file */
	nummaterials = 0;
	while(fscanf(file, "%s", buf) != EOF) {
		switch(buf[0]) {
		case '#':               /* comment */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		case 'n':               /* newmtl */
			fgets(buf, sizeof(buf), file);
			nummaterials++;
			break;
		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

	rewind(file);//Repositions the file pointer to the beginning of a file.

	pModel->materials = (Mmaterial *)malloc(sizeof(Mmaterial) * nummaterials);
	pModel->nummaterials = nummaterials;

	/* now, read in the data */
	int idx = -1;
	while(fscanf(file, "%s", buf) != EOF) {
		switch(buf[0]) {
		case '#':               /* comment */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		case 'n':               /* newmtl */
			idx++;	
			fgets(buf, sizeof(buf), file);
			sscanf(buf, "%s %s", buf, buf);
			pModel->materials[idx].name = _strdup(buf);
			pModel->materials[idx].bTexture = false;
			break;
		case 'N':			// �����
			fscanf(file, "%lf", &pModel->materials[idx].shininess);
			/* wavefront shininess is from [0, 1000], so scale for OpenGL */
			//pModel->materials[idx].shininess /= 1000.0;
			//pModel->materials[idx].shininess *= 128.0;
			break;
		case 'd':
		case 'T':
			fscanf(file, "%lf", &pModel->materials[idx].transparent);
			break;
		case 'i':			// �߹�ָ��
			fscanf(file, "%lf", &pModel->materials[idx].illum);
			break;
		case 'K':
		case 'k':
			switch(buf[1]) {
			case 'd':
				fscanf(file, "%lf %lf %lf",
					&((pModel->materials[idx]).diffuse[0]),
					&((pModel->materials[idx]).diffuse[1]),
					&((pModel->materials[idx]).diffuse[2]));
				break;
			case 's':
				fscanf(file, "%lf %lf %lf",
					&((pModel->materials[idx]).specular[0]),
					&((pModel->materials[idx]).specular[1]),
					&((pModel->materials[idx]).specular[2]));
				break;
			case 'a':
				fscanf(file, "%lf %lf %lf",
					&((pModel->materials[idx]).ambient[0]),
					&((pModel->materials[idx]).ambient[1]),
					&((pModel->materials[idx]).ambient[2]));
				break;
			case 't':
				fscanf(file, "%lf %lf %lf",
					&((pModel->materials[idx]).trans[0]),
					&((pModel->materials[idx]).trans[1]),
					&((pModel->materials[idx]).trans[2]));
				break;
			default:
				/* eat up rest of line */
				fgets(buf, sizeof(buf), file);
				break;
			}
			break;
		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

	fclose(file);
}

int CObjLoader::FindMaterial(Mmodel* pModel, char* name)
{
	int idx = -1;

	for (int i = 0; i < pModel->nummaterials; i++) {
		if (!strcmp(pModel->materials[i].name, name)) {
			idx = i;
			break;
		}
	}

	return idx;
}

//  ����obj�ļ��еĶ���ģ����
void CObjLoader::ReadObjFile(Mmodel* pModel)
{
	char strLine[255]		= {0};
	char ch					= 0;

	while(!feof(m_FilePointer))
	{
		// ���obj�ļ��еĵ�ǰ�еĵ�һ���ַ�
		ch = fgetc(m_FilePointer);

		switch(ch)
		{
		case 'm':

			fgets(strLine, sizeof(strLine), m_FilePointer);
			sscanf(strLine, "%s %s", strLine, strLine);
			pModel->mtllibname = _strdup(strLine+1);
			ReadMtlFile(pModel, pModel->mtllibname);
			break;

		case 'u':

			fgets(strLine, sizeof(strLine), m_FilePointer);
			sscanf(strLine, "%s %s", strLine, strLine);
			MaterialID = FindMaterial(pModel, strLine+1);
			break;

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

		case '\n':

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
	//int vNewTexCoord[2];
	Point  vNewVertex;
	Vector vNewNormal;

	// �����һ���ַ����ж϶�����Ƿ񶥵�/������/UV����
	ch = fgetc(m_FilePointer);

	if(ch == ' ')				// ����ǿո�����Ƕ���("v")
	{
		// ���붥�����꣬��ʽ��"v x y z"
		fscanf(m_FilePointer, "%lf %lf %lf", &vNewVertex.d[0], &vNewVertex.d[1], &vNewVertex.d[2]);

		// ������������µ����ݣ����ļ�ָ�뵽��һ��
		fgets(strLine, 100, m_FilePointer);

		// ���һ���µĶ��㵽����������
		m_pVertices.push_back(vNewVertex);
	}
	else if(ch == 't')			// �����'t'����ض�����������("vt")
	{
		// �����������꣬��ʽ��"vt u v"
		//fscanf(m_FilePointer, "%f %f", &vNewTexCoord.d[0], &vNewTexCoord.d[1]);

		// ����������µ����ݣ����ļ�ָ��ָ����һ��
		fgets(strLine, 100, m_FilePointer);

		// ���һ���µ��������굽������
		//m_pTextureCoords.push_back(vNewTexCoord);

		//  ���ö��������������Ϊtrue
		m_bObjectHasUV = true;
	}
	else if (ch == 'n')			// �����'n', ��ض��Ƕ��㷨��("vn")
	{
		// ���뷨����������ʽ��"vn x y z"
		fscanf(m_FilePointer, "%lf %lf %lf", &vNewNormal.d[0], &vNewNormal.d[1], &vNewNormal.d[2]);
		//vNewNormal = -vNewNormal;

		// ����������µ����ݣ����ļ�ָ��ָ����һ��
		fgets(strLine, 100, m_FilePointer);

		// ���һ���µķ������굽������
		m_pVertNormals.push_back(vNewNormal);

		//���ö�����ж��㷨��
		m_bObjectHasNoraml = true;
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
	Face newFace			= {0};
	char strLine[255]		= {0};

	if(!m_bObjectHasUV && !m_bObjectHasNoraml)		// �����������޶��㷨��
	{
		// �������Ķ�����������ʽ��"����1 ����2 ����3"
		fscanf(m_FilePointer, "%d %d %d", &newFace.vertIndex[0],
			&newFace.vertIndex[1],
			&newFace.vertIndex[2]);
		for (int i = 0; i < 3; i++)
			newFace.normalIndex[i] = newFace.vertIndex[i];
	}
	else if(m_bObjectHasUV && !m_bObjectHasNoraml)		// �����������޶��㷨��
	{
		// �������Ķ��������������������ʽ��"����1/��������1 ����2/��������2 ����3/��������3"
		fscanf(m_FilePointer, "%d/%d %d/%d %d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0],
			&newFace.vertIndex[1], &newFace.coordIndex[1],
			&newFace.vertIndex[2], &newFace.coordIndex[2]);	
		for (int i = 0; i < 3; i++)
			newFace.normalIndex[i] = newFace.vertIndex[i];
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
	newFace.MaterialID = MaterialID;

	// �����0��ʼ������ȥ1   (�ں����)
	for (int i = 0; i < 3; i++)
	{
		newFace.vertIndex[i]	-= 1;
		//newFace.coordIndex[i]	-= 1;
		newFace.normalIndex[i]	-= 1;
	}
	// ���һ�����浽��������
	m_pFaces.push_back(newFace);

	//  ���øղŶ��������
	m_bJustReadAFace = true;
}

//  ����ĺ���������������Ϣд��ģ�ͽṹ����
void CObjLoader::FillInObjectInfo(Mmodel *pModel)
{
	// ������������������������������������
	int numVert = 0, numNorm = 0, numFace = 0;
	numVert  = m_pVertices.size();
	//pObject->numTexVertex = m_pTextureCoords.size();
	numNorm = m_pVertNormals.size();
	numFace = m_pFaces.size();


	// ��������˵�
	if( numVert ) {
		for (int i = 0; i < numVert; i++)
			pModel->vertices.push_back(m_pVertices[i]);
	}	

	// �����������������

	// �����������
	if( numFace )  {
		for (int i = 0; i <numFace; i++)
			pModel->surface.push_back(m_pFaces[i]);
		pModel->numfaces += numFace;
	}

	// ��������˶���ķ���
	if ( numNorm ) {
		for (int i = 0; i < numNorm; i++)
			pModel->normals.push_back(m_pVertNormals[i]);
		pModel->numnormals += numNorm;
	}
	else // û�ж��붥�㷨������������ÿ������ķ���
	{
		if ( numVert )
			ComputeNormals(pModel);
		pModel->numnormals += numVert;
	}
	pModel->numvertices += numVert;		// ����ټӶ������Ŀ����Ϊ�ڼ��㷨����ʱҪ�õ��������Ŀ
	
	//  ������е���ʱ����
	m_pVertices.clear();
	m_pFaces.clear();
	m_pVertNormals.clear();
	m_pTextureCoords.clear();

	// �������еĲ���ֵΪfalse
	m_bObjectHasUV		= false;
	m_bObjectHasNoraml	= false;
	m_bJustReadAFace	= false;
}


// ���㶥�㷨����
void CObjLoader::ComputeNormals(Mmodel* pModel)
{
	// ������󲻴��ڣ��򷵻�
	if( !pModel )		return;

	int numFace = m_pFaces.size();
	Vector vNormal, vPoly[3];

	// ƽ�淨�����洢�ռ�
	Vector* pTempNormals	= new Vector [numFace];

	// ���������������
	for(int i = 0; i < numFace; i++)
	{	
		vPoly[0] = pModel->vertices[pModel->surface[i].vertIndex[0]];
		vPoly[1] = pModel->vertices[pModel->surface[i].vertIndex[1]];
		vPoly[2] = pModel->vertices[pModel->surface[i].vertIndex[2]];

		// �����淨����
		vNormal  = (vPoly[1] - vPoly[0]).Cross(vPoly[2] - vPoly[0]);
		pTempNormals[i] = vNormal.Normalize();
	}

	//  �����󶥵㷨��
	int shared;
	Vector vSum;
	int numVert = m_pVertices.size();
	int end = pModel->numvertices + numVert;
	// �������еĶ���
	for (int i = pModel->numvertices; i < end; i++)			
	{
		shared = 0;
		vSum = Vector(0, 0, 0);
		for (int j = 0; j < numFace; j++)	// �������е���������
		{												// �жϸõ��Ƿ����������湲��
			if (m_pFaces[j].vertIndex[0] == i ||
				m_pFaces[j].vertIndex[1] == i ||
				m_pFaces[j].vertIndex[2] == i)
			{
				vSum += pTempNormals[j];
				shared++;								
			}
		}      
		vNormal = vSum / (double) shared;
		pModel->normals.push_back(vNormal.Normalize());
	}

	delete [] pTempNormals;
}