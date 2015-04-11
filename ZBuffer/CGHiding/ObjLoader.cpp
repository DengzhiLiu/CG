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
	CString strMessage;				// 用于显示错误信息

	// 判断是否是一个合法的模型和文件类型
	if(!pModel || strFileName.IsEmpty()) return false;

	// 以只读方式打开文件，返回文件指针
	m_FilePointer = fopen(strFileName, "r");

	// 判断文件指针是否正确
	if(!m_FilePointer) {
		// 如果文件指针不正确，则显示错误信息
		strMessage.Format("Unable to find or open the file: %s", strFileName.GetString());
		AfxMessageBox(strMessage);
		return false;
	}

	// 读入文件信息
	ReadObjFile(pModel);

	// 关闭打开的文件
	fclose(m_FilePointer);

	return true;
}

//  读入obj文件中的对象到模型中
void CObjLoader::ReadObjFile(t3DModel *pModel)
{
	char strLine[255]		= {0};
	char ch					= 0;

	while(!feof(m_FilePointer))
	{
		float x = 0.0f, y = 0.0f, z = 0.0f;

		// 获得obj文件中的当前行的第一个字符
		ch = fgetc(m_FilePointer);

		switch(ch)
		{
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
	CFVector2 vNewTexCoord = {0};
	CFVector3 vNewVertex,	vNewNormal;

	// 读入第一个字符，判断读入的是否顶点/法向量/UV坐标
	ch = fgetc(m_FilePointer);

	if(ch == ' ')				// 如果是空格，则必是顶点("v")
	{
		// 读入顶点坐标，格式是"v x y z"
		fscanf(m_FilePointer, "%f %f %f", &vNewVertex.x, &vNewVertex.y, &vNewVertex.z);

		// 读入该行中余下的内容，则文件指针到下一行
		fgets(strLine, 100, m_FilePointer);

		// 添加一个新的顶点到顶点链表中
		m_pVertices.push_back(vNewVertex);
	}
	else if(ch == 't')			// 如果是't'，则必定是纹理坐标("vt")
	{
		// 读入纹理坐标，格式是"vt u v"
		fscanf(m_FilePointer, "%f %f", &vNewTexCoord.x, &vNewTexCoord.y);

		// 读入该行余下的内容，则文件指针指向下一行
		fgets(strLine, 100, m_FilePointer);

		// 添加一个新的纹理坐标到链表中
		m_pTextureCoords.push_back(vNewTexCoord);

		//  设置对象具有纹理坐标为true
		m_bObjectHasUV = TRUE;
	}
	else if (ch == 'n')			// 如果是'n', 则必定是顶点法线("vn")
	{
		// 读入法线向量，格式是"vn x y z"
		fscanf(m_FilePointer, "%f %f %f", &vNewNormal.x, &vNewNormal.y, &vNewNormal.z);
		
		// 读入该行余下的内容，则文件指针指向下一行
		fgets(strLine, 100, m_FilePointer);

		// 添加一个新的法线坐标到链表中
		m_pVertNormals.push_back(vNewNormal);

		//设置对象具有顶点法线
		m_bObjectHasNoraml = TRUE;
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
	tFace newFace			= {0};
	char strLine[255]		= {0};

	if(!m_bObjectHasUV && !m_bObjectHasNoraml)		// 对象无纹理、无顶点法线
	{
		// 读入对象的顶点索引，格式是"顶点1 顶点2 顶点3"
		fscanf(m_FilePointer, "%d %d %d", &newFace.vertIndex[0],
			&newFace.vertIndex[1],
			&newFace.vertIndex[2]);				
	}
	else if(m_bObjectHasUV && !m_bObjectHasNoraml)		// 对象有纹理，无顶点法线
	{
		// 读入对象的顶点和纹理坐标索引，格式是"顶点1/纹理坐标1 顶点2/纹理坐标2 顶点3/纹理坐标3"
		fscanf(m_FilePointer, "%d/%d %d/%d %d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0],
			&newFace.vertIndex[1], &newFace.coordIndex[1],
			&newFace.vertIndex[2], &newFace.coordIndex[2]);				
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

	// 坐标从0开始，都减去1
	for (int i = 0; i < 3; i++)
	{
		newFace.vertIndex[i]	-= 1;
		newFace.coordIndex[i]	-= 1;
		newFace.normalIndex[i]	-= 1;
	}
	// 添加一个新面到面链表中
	m_pFaces.push_back(newFace);

	//  设置刚才读入的是面
	m_bJustReadAFace = true;
}

//  下面的函数将读入对象的信息写入模型结构体中
void CObjLoader::FillInObjectInfo(t3DModel *pModel)
{
	t3DObject newObject = {0};
	int i = 0;

	// 模型中对象计数器递增
	pModel->numOfObjects++;

	// 添加一个新对象到模型的对象链表中
	pModel->pObject.push_back(newObject);

	// 获得当前对象的指针
	t3DObject *pObject = &(pModel->pObject[pModel->numOfObjects - 1]);

	// 获得面的数量、顶点的数量和纹理坐标的数量
	pObject->numOfVerts   = m_pVertices.size();
	pObject->numTexVertex = m_pTextureCoords.size();
	pObject->numVertNormal= m_pVertNormals.size();
	pObject->numOfFaces   = m_pFaces.size();

	// 如果读入了点
	if(pObject->numOfVerts) {
		// 分配保存点的存储空间
		pObject->pVerts = new CFVector3 [pObject->numOfVerts];

		// 遍历对象中的所有点
		for(i = 0; i < pObject->numOfVerts; i++)
		{
			// 将当前的顶点从临时链表中拷贝到模型链表中
			pObject->pVerts[i] = m_pVertices[i];
		}
	}	

	// 如果读入了纹理坐标
	if(m_bObjectHasUV) {
		pObject->pTexVerts = new CFVector2 [pObject->numTexVertex];
		//pObject->bHasTexture = true;

		// 遍历对象中所有的纹理坐标
		for(i = 0; i < pObject->numTexVertex; i++)
		{
			// 将当前的纹理坐标从临时链表中拷贝到模型链表中
			pObject->pTexVerts[i] = m_pTextureCoords[i];
		}
	}

	// 如果读入了面
	if(pObject->numOfFaces) 
	{
		// 分配保存面的存储空间
		pObject->pFaces = new tFace [pObject->numOfFaces];

		// 遍历所有的面
		for(i = 0; i < pObject->numOfFaces; i++)
		{
			// 拷贝临时的面链表到模型链表中
			pObject->pFaces[i] = m_pFaces[i];
		}
	}

	// 计算顶点心法向量,用于光照
	// 如果读入了顶点的法线
	if (m_bObjectHasNoraml)
	{
		pObject->pNormals = new CFVector3 [pObject->numVertNormal];

		// 遍历对象中所有的顶点法线
		for(i = 0; i < pObject->numVertNormal; i++)
		{
			// 将当前的顶点法线坐标从临时链表拷贝到模型链表中
			pObject->pNormals[i] = m_pVertNormals[i];
		}
	}
	else // 没有读入顶点法线则根据面计算每个顶点的法线
	{
		ComputeNormals(pObject);
	}

	//  由于OBJ文件中没有材质，因此将materialID设置为-1，必须手动设置材质
	pObject->materialID = -1;

	//  清除所有的临时链表
	m_pVertices.clear();
	m_pFaces.clear();
	m_pTextureCoords.clear();

	// 设置所有的布尔值为false
	m_bObjectHasUV		= false;
	m_bObjectHasNoraml	= false;
	m_bJustReadAFace	= false;
}

// 下面主要用来计算顶点的法向量，顶点的法向量主要用来计算光照
void CObjLoader::ComputeNormals(t3DObject *pObject)
{
	// 如果对象不存在，则返回
	if(!pObject)		return;

	CFVector3 vVector1, vVector2, vNormal, vPoly[3];

	// 分配需要的存储空间
	//CFVector3 *pNormals		= new CFVector3 [pObject->numOfFaces];
	CFVector3 *pTempNormals	= new CFVector3 [pObject->numOfFaces];
	pObject->pNormals		= new CFVector3 [pObject->numOfVerts];

	// 遍历对象的所有面
	for(int i=0; i < pObject->numOfFaces; i++)
	{												
		vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
		vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
		vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

		// 计算面的法向量

		vVector1 = vPoly[0] - vPoly[1];		// 获得多边形的矢量
		vVector2 = vPoly[0] - vPoly[2];		// 获得多边形的第二个矢量

		vNormal  = vVector1 * vVector2;		// 获得两个矢量的叉积
		pTempNormals[i] = vNormal;					// 保存非规范化法向量
		//pNormals[i] = vNormal / sqrt(vNormal.Norm());			// 将规范化叉积添加到法向量列表中
	}

	//  下面求顶点法线
	int shared=0;
	CFVector3 vZero, vSum;
	// 遍历所有的顶点
	for (int i = 0; i < pObject->numOfVerts; i++)			
	{
		for (int j = 0; j < pObject->numOfFaces; j++)	// 遍历所有的三角形面
		{												// 判断该点是否与其它的面共享
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

		// 规范化最后的顶点法向
		pObject->pNormals[i] = pObject->pNormals[i] / sqrt(pObject->pNormals[i].Norm());	
		vSum = vZero;
	}

	// 释放存储空间，开始下一个对象
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
	if (model.numOfObjects == 0)			//模型中没有obj对象则返回
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