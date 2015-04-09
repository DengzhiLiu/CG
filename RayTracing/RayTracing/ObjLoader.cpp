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
	// 以只读方式打开文件，返回文件指针
	m_FilePointer = fopen(filename, "r");
	pModel->pathname = _strdup(filename);

	// 判断文件指针是否正确
	if(!m_FilePointer) {
		fprintf(stderr, "Unable to find or open file: %s", filename);
		return false;
	}

	// 读入文件信息
	ReadObjFile(pModel);

	// 关闭打开的文件
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

// 读入材质文件
void CObjLoader::ReadMtlFile(Mmodel* pModel, char* name)
{
	FILE* file;
	char* dir;
	char* filename;
	char buf[128];
	int nummaterials;

	dir = GetDirname(pModel->pathname);			//得到目录的位置
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
		case 'N':			// 反光度
			fscanf(file, "%lf", &pModel->materials[idx].shininess);
			/* wavefront shininess is from [0, 1000], so scale for OpenGL */
			//pModel->materials[idx].shininess /= 1000.0;
			//pModel->materials[idx].shininess *= 128.0;
			break;
		case 'd':
		case 'T':
			fscanf(file, "%lf", &pModel->materials[idx].transparent);
			break;
		case 'i':			// 高光指数
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

//  读入obj文件中的对象到模型中
void CObjLoader::ReadObjFile(Mmodel* pModel)
{
	char strLine[255]		= {0};
	char ch					= 0;

	while(!feof(m_FilePointer))
	{
		// 获得obj文件中的当前行的第一个字符
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

		case 'v':						// 读入的是'v' (后续的数据可能是顶点/法向量/纹理坐标)

			// 如果在前面读入的是面的行，那么现在进入的是另一个对象，因此在读入下一个对象之前，
			//  需要将最后对象的数据写入到模型结构体中
			if(m_bJustReadAFace) 
			{
				// 将最后对象的信息保存到模型结构中
				FillInObjectInfo(pModel);
			}

			// 读入点的信息，要区分顶点 ("v")、法向量 ("vn")、纹理坐标 ("vt")
			ReadVertexInfo();
			break;

		case 'f':						// 读入的是'f'

			//  读入面的信息
			ReadFaceInfo();
			break;

		case '\n':

			break;

		default:

			// 略过该行的内容
			fgets(strLine, 100, m_FilePointer);
			break;
		}
	}

	// 保存最后读入的对象
	FillInObjectInfo(pModel);
}

//  下面的函数读入顶点信息('v'是指顶点，'vt'指UV坐标)
void CObjLoader::ReadVertexInfo()
{
	char ch = 0;
	char strLine[255] = {0};
	//int vNewTexCoord[2];
	Point  vNewVertex;
	Vector vNewNormal;

	// 读入第一个字符，判断读入的是否顶点/法向量/UV坐标
	ch = fgetc(m_FilePointer);

	if(ch == ' ')				// 如果是空格，则必是顶点("v")
	{
		// 读入顶点坐标，格式是"v x y z"
		fscanf(m_FilePointer, "%lf %lf %lf", &vNewVertex.d[0], &vNewVertex.d[1], &vNewVertex.d[2]);

		// 读入该行中余下的内容，则文件指针到下一行
		fgets(strLine, 100, m_FilePointer);

		// 添加一个新的顶点到顶点链表中
		m_pVertices.push_back(vNewVertex);
	}
	else if(ch == 't')			// 如果是't'，则必定是纹理坐标("vt")
	{
		// 读入纹理坐标，格式是"vt u v"
		//fscanf(m_FilePointer, "%f %f", &vNewTexCoord.d[0], &vNewTexCoord.d[1]);

		// 读入该行余下的内容，则文件指针指向下一行
		fgets(strLine, 100, m_FilePointer);

		// 添加一个新的纹理坐标到链表中
		//m_pTextureCoords.push_back(vNewTexCoord);

		//  设置对象具有纹理坐标为true
		m_bObjectHasUV = true;
	}
	else if (ch == 'n')			// 如果是'n', 则必定是顶点法线("vn")
	{
		// 读入法线向量，格式是"vn x y z"
		fscanf(m_FilePointer, "%lf %lf %lf", &vNewNormal.d[0], &vNewNormal.d[1], &vNewNormal.d[2]);
		//vNewNormal = -vNewNormal;

		// 读入该行余下的内容，则文件指针指向下一行
		fgets(strLine, 100, m_FilePointer);

		// 添加一个新的法线坐标到链表中
		m_pVertNormals.push_back(vNewNormal);

		//设置对象具有顶点法线
		m_bObjectHasNoraml = true;
	}
	else					// 其它情况
	{
		// 忽略其它情况，直接跳到下一行
		fgets(strLine, 100, m_FilePointer);
	}
}

//  下面的函数读入面信息
//	面的信息 "f 顶点索引/uv点索引/法线索引"
void CObjLoader::ReadFaceInfo()
{
	Face newFace			= {0};
	char strLine[255]		= {0};

	if(!m_bObjectHasUV && !m_bObjectHasNoraml)		// 对象无纹理、无顶点法线
	{
		// 读入对象的顶点索引，格式是"顶点1 顶点2 顶点3"
		fscanf(m_FilePointer, "%d %d %d", &newFace.vertIndex[0],
			&newFace.vertIndex[1],
			&newFace.vertIndex[2]);
		for (int i = 0; i < 3; i++)
			newFace.normalIndex[i] = newFace.vertIndex[i];
	}
	else if(m_bObjectHasUV && !m_bObjectHasNoraml)		// 对象有纹理，无顶点法线
	{
		// 读入对象的顶点和纹理坐标索引，格式是"顶点1/纹理坐标1 顶点2/纹理坐标2 顶点3/纹理坐标3"
		fscanf(m_FilePointer, "%d/%d %d/%d %d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0],
			&newFace.vertIndex[1], &newFace.coordIndex[1],
			&newFace.vertIndex[2], &newFace.coordIndex[2]);	
		for (int i = 0; i < 3; i++)
			newFace.normalIndex[i] = newFace.vertIndex[i];
	}
	else if(!m_bObjectHasUV && m_bObjectHasNoraml)		// 对象无纹理，有顶点法线
	{
		// 读入对象的顶点和纹理坐标索引，格式是"顶点1//顶点法线1 顶点2//顶点法线2 顶点3//顶点法线3"
		fscanf(m_FilePointer, "%d//%d %d//%d %d//%d", &newFace.vertIndex[0], &newFace.normalIndex[0],
			&newFace.vertIndex[1], &newFace.normalIndex[1],
			&newFace.vertIndex[2], &newFace.normalIndex[2]);				
	}
	else if (m_bObjectHasUV && m_bObjectHasNoraml)		// 对象有纹理、有顶点法线
	{
		// 读入对象的顶点、纹理坐标和顶点法线索引，
		// 格式是"顶点1/纹理坐标1/顶点法线1 顶点2/纹理坐标2/顶点法线2 顶点3/纹理坐标3//顶点法线3"
		fscanf(m_FilePointer, "%d/%d/%d %d/%d/%d %d/%d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0], &newFace.normalIndex[0],
			&newFace.vertIndex[1], &newFace.coordIndex[1], &newFace.normalIndex[1],
			&newFace.vertIndex[2], &newFace.coordIndex[2], &newFace.normalIndex[2]);	
	}

	// 读入该行余下的内容，则文件指针指向下一行
	fgets(strLine, 100, m_FilePointer);
	newFace.MaterialID = MaterialID;

	// 坐标从0开始，都减去1   (在后面减)
	for (int i = 0; i < 3; i++)
	{
		newFace.vertIndex[i]	-= 1;
		//newFace.coordIndex[i]	-= 1;
		newFace.normalIndex[i]	-= 1;
	}
	// 添加一个新面到面链表中
	m_pFaces.push_back(newFace);

	//  设置刚才读入的是面
	m_bJustReadAFace = true;
}

//  下面的函数将读入对象的信息写入模型结构体中
void CObjLoader::FillInObjectInfo(Mmodel *pModel)
{
	// 获得面的数量、顶点的数量和纹理坐标的数量
	int numVert = 0, numNorm = 0, numFace = 0;
	numVert  = m_pVertices.size();
	//pObject->numTexVertex = m_pTextureCoords.size();
	numNorm = m_pVertNormals.size();
	numFace = m_pFaces.size();


	// 如果读入了点
	if( numVert ) {
		for (int i = 0; i < numVert; i++)
			pModel->vertices.push_back(m_pVertices[i]);
	}	

	// 如果读入了纹理坐标

	// 如果读入了面
	if( numFace )  {
		for (int i = 0; i <numFace; i++)
			pModel->surface.push_back(m_pFaces[i]);
		pModel->numfaces += numFace;
	}

	// 如果读入了顶点的法线
	if ( numNorm ) {
		for (int i = 0; i < numNorm; i++)
			pModel->normals.push_back(m_pVertNormals[i]);
		pModel->numnormals += numNorm;
	}
	else // 没有读入顶点法线则根据面计算每个顶点的法线
	{
		if ( numVert )
			ComputeNormals(pModel);
		pModel->numnormals += numVert;
	}
	pModel->numvertices += numVert;		// 最后再加顶点的数目，因为在计算法向量时要用到顶点的数目
	
	//  清除所有的临时链表
	m_pVertices.clear();
	m_pFaces.clear();
	m_pVertNormals.clear();
	m_pTextureCoords.clear();

	// 设置所有的布尔值为false
	m_bObjectHasUV		= false;
	m_bObjectHasNoraml	= false;
	m_bJustReadAFace	= false;
}


// 计算顶点法向量
void CObjLoader::ComputeNormals(Mmodel* pModel)
{
	// 如果对象不存在，则返回
	if( !pModel )		return;

	int numFace = m_pFaces.size();
	Vector vNormal, vPoly[3];

	// 平面法向量存储空间
	Vector* pTempNormals	= new Vector [numFace];

	// 遍历对象的所有面
	for(int i = 0; i < numFace; i++)
	{	
		vPoly[0] = pModel->vertices[pModel->surface[i].vertIndex[0]];
		vPoly[1] = pModel->vertices[pModel->surface[i].vertIndex[1]];
		vPoly[2] = pModel->vertices[pModel->surface[i].vertIndex[2]];

		// 计算面法向量
		vNormal  = (vPoly[1] - vPoly[0]).Cross(vPoly[2] - vPoly[0]);
		pTempNormals[i] = vNormal.Normalize();
	}

	//  下面求顶点法线
	int shared;
	Vector vSum;
	int numVert = m_pVertices.size();
	int end = pModel->numvertices + numVert;
	// 遍历所有的顶点
	for (int i = pModel->numvertices; i < end; i++)			
	{
		shared = 0;
		vSum = Vector(0, 0, 0);
		for (int j = 0; j < numFace; j++)	// 遍历所有的三角形面
		{												// 判断该点是否与其它的面共享
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