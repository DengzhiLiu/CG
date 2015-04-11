#include "skinning.h"

#include <gl\glut.h>
#include "tinyxml.h"

//#include <qDebug>

//int Skin::readMeshInfo(const std::string filename)//vertexes and faces female.TRIANGLEMESH
//{
//	TiXmlDocument* MeshDoc =new TiXmlDocument(filename.c_str());
//
//	if(!MeshDoc->LoadFile(filename.c_str(),TIXML_DEFAULT_ENCODING)){
//		qDebug("Error: The mesh file does not exist...");
//		return false;
//	}
//
//	//开始按照约定格式解析	
//	TiXmlElement* xml_root = MeshDoc->FirstChild("Mesh")->ToElement();
//	TiXmlElement* xml_VertexInfo = xml_root->FirstChild("VertexInfo")->ToElement();
//
//	int VertexNUM = atoi( xml_VertexInfo->Attribute("VertexNum"));
//	//Vertexes.resize(VertexNUM); ???
//	TiXmlNode* xml_VertexNode = xml_VertexInfo->FirstChild("Vertex");
//	Vertex* curVertex;
//	while ( xml_VertexNode)
//	{
//		//<Vertex ID="0">
//		//	<Position x="-0.025904" y="-0.009170" z="0.110789" />
//		//	<Normal x="1.000000" y="0.000000" z="0.000000" />
//		//	<UV u="0.588690" v="0.315210" />
//		//</Vertex>
//
//		int vertex_id;
//		Vector3 resetpos;
//		Vector3 norm;
//		Vector3 uv;
//
//		TiXmlElement* xml_Vertex = xml_VertexNode->ToElement();
//
//		vertex_id = atoi(xml_Vertex->Attribute("ID"));
//
//		TiXmlElement* xml_Vertex_Pos = xml_VertexNode->FirstChild("Position")->ToElement();
//		resetpos.x = atof( xml_Vertex_Pos->Attribute("x"));
//		resetpos.y = atof( xml_Vertex_Pos->Attribute("y"));
//		resetpos.z = atof( xml_Vertex_Pos->Attribute("z"));
//
//		TiXmlElement* xml_Vertex_Norm = xml_VertexNode->FirstChild("Normal")->ToElement();
//		norm.x = atof( xml_Vertex_Norm->Attribute("x"));
//		norm.y = atof( xml_Vertex_Norm->Attribute("y"));
//		norm.z = atof( xml_Vertex_Norm->Attribute("z"));
//
//		TiXmlElement* xml_Vertex_UV = xml_VertexNode->FirstChild("UV")->ToElement();
//		uv.x = atof( xml_Vertex_UV->Attribute("u"));
//		uv.y = atof( xml_Vertex_UV->Attribute("v"));
//		uv.z = 0;
//
//		//Vertex(int id, Vector3 resetPos,Vector3 norm,Vector3 uv)
//		curVertex = new Vertex(vertex_id,resetpos,norm,uv);
//		Vertexes[vertex_id] = curVertex;
//
//		xml_VertexNode = xml_VertexNode->NextSibling("Vertex");
//	}
//
//	/*
//	<FaceInfo FaceNum="180518">
//		<Face V1="445" V2="74" V3="448" />
//		<Face V1="445" V2="448" V3="212" />
//		<Face V1="446" V2="13" V3="445" />
//	*/
//
//	TiXmlElement* xml_FaceInfo = xml_root->FirstChild("FaceInfo")->ToElement();
//
//	int FaceNUM = atoi( xml_FaceInfo->Attribute("FaceNum"));
//	//Faces.resize(FaceNUM);
//	TiXmlNode* xml_FaceNode = xml_FaceInfo->FirstChild("Face");
//	Face* curFace;
//	while ( xml_FaceNode)
//	{
//		int v1;
//		int v2;
//		int v3;
//
//		TiXmlElement* xml_Face = xml_FaceNode->ToElement();
//
//		v1 = atoi(xml_Face->Attribute("V1"));
//		v2 = atoi(xml_Face->Attribute("V2"));
//		v3 = atoi(xml_Face->Attribute("V3"));
//		//Face(int v1,int v2,int v3 ){ vertex1_ID = v1;vertex2_ID = v2; vertex3_ID = v3;}
//		curFace = new Face(v1,v2,v3);
//		Faces.push_back(curFace);
//
//		xml_FaceNode = xml_FaceNode->NextSibling("Face");
//	}
//	MeshDoc->Clear();
//	delete MeshDoc;
//
//	return true;
//}
int Skin::readObjFile(const std::string filename)
{
#define  BUFFER_LENGTH  100

	FILE *file = fopen(filename.c_str(), "rb");
	
	char      buf[BUFFER_LENGTH];//存储读入信息

	Vector3 position;//顶点信息

	int vertexID=0;
	int normalID=0;
	int uvID = 0;

	int posIndex_1,posIndex_2,posIndex_3;//三角形信息
	int uvIndex_1,uvIndex_2,uvIndex_3;
	int normalIndex_1,normalIndex_2,normalIndex_3;

	Vertex* curVertex;
	Vector3 curNormal;
	Vector3 curUV;
	Face* curFace;

	while(fscanf(file, "%s", buf) != EOF) 
	{
		//读取数据
		switch(buf[0]) 
		{
		case 'm'://材质库
		case 'u'://材质信息
		case 'g'://物体				
			break;

		case 'v':  //如果读取到 v,vt,vn 数据
			switch(buf[1]) 
			{
			case '\0'://顶点坐标
				fscanf(file, "%f %f %f", &position.x,&position.y, &position.z);  
				curVertex = new Vertex(vertexID,position);
				Vertexes[vertexID] = curVertex;
				vertexID++;
				break;

			case 't'://纹理坐标
				fscanf(file, "%f %f %f", &curUV.x,&curUV.y, &curUV.z);  
				UVs[uvID] = curUV;
				uvID++;
				break;
			case 'n'://法线
				fscanf(file, "%f %f %f", &curNormal.x,&curNormal.y, &curNormal.z); 				
				Normals[normalID] = curNormal;
				normalID++;					
				break;
			}
			break;

		case 'f':	//如果读取到面数据			
			fscanf(file, "%s", buf);
			//obj文件索引从1开始计数
			if(sscanf(buf, "%d//%d", &posIndex_1, &normalIndex_1)==2) //顶点,法线
			{
				fscanf(file, "%d//%d", &posIndex_2, &normalIndex_2);
				fscanf(file, "%d//%d", &posIndex_3, &normalIndex_3);
				
				curFace = new Face(posIndex_1-1,posIndex_2-1,posIndex_3-1,normalIndex_1,normalIndex_2,normalIndex_3,0);
				Faces.push_back(curFace);										
			}
			else if (sscanf(buf, "%d/%d/%d", &posIndex_1, &uvIndex_1, &normalIndex_1) ==3) //顶点 纹理 法线 
			{
				fscanf(file, "%d/%d/%d", &posIndex_2, &uvIndex_2, &normalIndex_2);
				fscanf(file, "%d/%d/%d", &posIndex_3, &uvIndex_3, &normalIndex_3);

				curFace = new Face(posIndex_1-1,posIndex_2-1,posIndex_3-1,normalIndex_1-1,normalIndex_2-1,normalIndex_3-1,uvIndex_1-1,uvIndex_2-1,uvIndex_3-1);
				Faces.push_back(curFace);				
			}
			else if (sscanf(buf, "%d/%d", &posIndex_1, &uvIndex_1) == 2) //顶点 纹理
			{	
				fscanf(file, "%d/%d/%d", &posIndex_2, &uvIndex_2);
				fscanf(file, "%d/%d/%d", &posIndex_3, &uvIndex_3);

				curFace = new Face(posIndex_1-1,posIndex_2-1,posIndex_3-1,uvIndex_1-1,uvIndex_2-1,uvIndex_3-1);
				Faces.push_back(curFace);				
			} 
			else  if(sscanf(buf, "%d %d %d", &posIndex_1) == 1) //顶点 
			{
				fscanf(file, "%d", &posIndex_2);
				fscanf(file, "%d", &posIndex_3);

				curFace = new Face(posIndex_1-1,posIndex_2-1,posIndex_3-1);
				Faces.push_back(curFace);				
			}			

			break;
		default:
			fgets(buf, sizeof(buf), file);//读入一行
			break;
		}   		
	}
	fclose(file);//关闭文件
	
	return true;	
}
int Skin::readWeightInfo(const std::string filename)//bones and bones weight
{
	TiXmlDocument* WeightDoc =new TiXmlDocument(filename.c_str());

	if(!WeightDoc->LoadFile(filename.c_str())){
		/*qDebug("Error: The weight file does not exist...");*/
		return false;
	}

	TiXmlElement* xml_root = WeightDoc->FirstChild("Skin")->ToElement();

	TiXmlElement* xml_BoneInfo = xml_root->FirstChild("BoneInfo")->ToElement();
	//int BoneNUM = atoi( xml_BoneInfo->Attribute("Num"));
	//Bones.resize(VertexNUM); ???

	HipPos.x = atof(xml_BoneInfo->Attribute("HipPosX"));
	HipPos.y = atof(xml_BoneInfo->Attribute("HipPosY"));
	HipPos.z = atof(xml_BoneInfo->Attribute("HipPosZ"));
	HipRad.x = atof(xml_BoneInfo->Attribute("HipRadX"));
	HipRad.y = atof(xml_BoneInfo->Attribute("HipRadY"));
	HipRad.z = atof(xml_BoneInfo->Attribute("HipRadZ"));	
	
	TiXmlNode* xml_BoneNode = xml_BoneInfo->FirstChild("Bone");
	Bone* curBone;
	while ( xml_BoneNode)
	{
		//<BoneInfo Num="20">
		//	<Bone Name="lradius" ID="0" />
		std::string name;
		int boneID;
		float Xrad;
		float Yrad;
		float Zrad;

		TiXmlElement* xml_Bone = xml_BoneNode->ToElement();

		name =  xml_Bone->Attribute("Name");
		boneID = atoi(xml_Bone->Attribute("ID"));
	
		//Bone(std::string n, int id){ name = n; ID = id;}		
		curBone = new Bone(name, boneID);
		Bones[boneID] = curBone;

		Xrad = atof(xml_Bone->Attribute("Xrad"));
		Yrad = atof(xml_Bone->Attribute("Yrad"));
		Zrad = atof(xml_Bone->Attribute("Zrad"));

		Vector3 rad = Vector3(Xrad,Yrad,Zrad);
		curBone->setResetRad(rad);		

		xml_BoneNode = xml_BoneNode->NextSibling("Bone");
	}

	TiXmlElement* xml_VertexInfo = xml_root->FirstChild("VertexInfo")->ToElement();
	//int VertexNUM = atoi(xml_VertexInfo->Attribute("Num"));
	
	TiXmlNode* xml_VertexNode = xml_VertexInfo->FirstChild("Vertex");
	Vertex* curVertex;
	while ( xml_VertexNode)
	{	
		int vertexID ;
		int BoneListNum ;
		int boneID;
		float weight;
/*	
	<Vertex ID="12">
		<BoneList Num="2">
			<bone ID="9" weight="0.500000" />
			<bone ID="10" weight="0.500000" />
		</BoneList>
	</Vertex>
*/
		TiXmlElement* xml_Vertex = xml_VertexNode->ToElement();
		vertexID = atoi( xml_Vertex->Attribute("ID"));
		curVertex = Vertexes[vertexID];

		TiXmlElement* xml_BoneList = xml_Vertex->FirstChild("BoneList")->ToElement();
		BoneListNum = atoi( xml_BoneList->Attribute("Num"));

		TiXmlNode* xml_boneNode = xml_BoneList->FirstChild("bone");
		while ( xml_boneNode )
		{
			TiXmlElement* xml_bone = xml_boneNode->ToElement();

			boneID = atoi( xml_bone->Attribute("ID"));
			weight = atof( xml_bone->Attribute("weight"));

			curVertex->weights[boneID] = weight;

			xml_boneNode = xml_boneNode->NextSibling("bone");
		}

		xml_VertexNode = xml_VertexNode->NextSibling( "Vertex" ); 
	}

	this->enable = true;

	WeightDoc->Clear();
	delete WeightDoc;

	glPushMatrix();
	glLoadIdentity();
	calBonesInvResetMatrix(curClip->joints[0], 0.2f);
	glPopMatrix();

	return true;
}

Bone* Skin::getBoneByID(int id)
{
	return Bones[id];
}
Bone* Skin::getBoneByName(std::string name)
{	
	std::map<int , Bone* >::iterator iter;
	for (iter=Bones.begin();iter!=Bones.end();iter++)
	{
		Bone* bone = iter->second;
		if ( strcmp(bone->getName().c_str() ,name.c_str())==0 )	
			return bone;	
	}
	return NULL;
}
std::string Skin::getBoneNameByID(int id)
{
	std::string name = Bones[id]->getName();
	return name;
}

Vertex* Skin::getVertexByID(int id)
{
	return Vertexes[id];
}
Vector3 Skin::getNormalByID(int id)
{
	return Normals[id];
}
Vector3 Skin::getUVByID(int id)
{
	return UVs[id];
}

void Skin::setCurClip(Clip* c)
{
	curClip = c;
}


void Skin::setSkinOnOff(bool on)
{
	enable = on;
}

void Skin::ShowWeightTable()
{

}
void Skin::setRenderMode(RenderMode mode)
{
	renderMode = mode;
}

void Skin::calBonesInvResetMatrix(Joint* joint, float scale)//怎么计算？　在3dmax中查看各关节旋转角度
{
	GLfloat M[16];
	glPushMatrix();
	
	if ( joint->getParent() == NULL )
	{
		glTranslatef( HipPos.x*scale ,HipPos.y*scale,HipPos.z*scale );
	
		glRotatef( HipRad.x , 1, 0, 0);
		glRotatef( HipRad.y , 0, 1, 0);
		glRotatef( HipRad.z , 0, 0, 1);
	}
		
	else
	{
		Vector3 offset=joint->getOffset();
		glTranslatef( offset.x*scale, offset.y*scale,offset.z*scale  );

		Bone* bone = getBoneByName(joint->getName());
		if (bone)
		{
			Vector3 rad = bone->getResetRad();
			glRotatef( rad.x , 1, 0, 0);
			glRotatef( rad.y , 0, 1, 0);
			glRotatef( rad.z , 0, 0, 1);
		}
			
		else
		{
			glRotatef( 0 , 1, 0, 0);
			glRotatef( 0 , 0, 1, 0);
			glRotatef( 0 , 0, 0, 1);
		}		
	}
	
	glGetFloatv(GL_MODELVIEW_MATRIX,M);
	Matrix4 mat = Matrix4(M);
	mat= mat.inverse();

	Bone* bone = getBoneByName( joint->getName());
	if (bone)
	{
		bone->setITM(mat);
/*		Vector3 trans = mat.getTrans();
		qDebug("%s\t %f\t %f\t %f\t" ,joint->getName().c_str(),trans.x,trans.y,trans.z);*/		
	}		

	for (int i=0; i<joint->numOfChildren(); i++ )	
		calBonesInvResetMatrix( joint->getChild(i), scale);

	glPopMatrix();
}
void Skin::calBonesMatrix(int frame_no,float scale)//计算蒙皮需要的各骨骼在第frame_no帧的变换矩阵
{
	glPushMatrix();
	glLoadIdentity();
	calBonesMatrix(curClip->joints[0],frame_no,scale);
	glPopMatrix();
}
void Skin::calBonesMatrix(Joint* joint,int frame_no,float scale)//计算蒙皮需要的各骨骼在第frame_no帧的变换矩阵
{	
	GLfloat M[16];
	glPushMatrix();	

	if ( joint->getParent() == NULL )	
		glTranslatef( joint->getChannel(0)->getData(frame_no)*scale ,joint->getChannel(1)->getData(frame_no)*scale, joint->getChannel(2)->getData(frame_no)*scale );
	else
	{
		Vector3 offset=joint->getOffset();
		glTranslatef( offset.x*scale, offset.y*scale,offset.z*scale  );
	}

	for (int i=0; i<joint->getChannelNumber(); i++ )
	{
		Channel *  channel = joint->getChannel(i);
		if ( channel->getType() == X_ROTATION )		
			glRotatef( channel->getData(frame_no), 1.0f, 0.0f, 0.0f );
		else if ( channel->getType() == Y_ROTATION )
			glRotatef( channel->getData(frame_no), 0.0f, 1.0f, 0.0f );
		else if ( channel->getType() == Z_ROTATION )
			glRotatef( channel->getData(frame_no), 0.0f, 0.0f, 1.0f );		
	}

	glGetFloatv(GL_MODELVIEW_MATRIX,M);
	Matrix4 mat = Matrix4(M);
	
	// 这里计算bone的transform的地方有问题，bone的数量应该比joint的数量要少1
	Bone* bone = getBoneByName( joint->getName());
	if (bone)
	{
		bone->setNewMat(mat);
		//Vector3 trans = mat.getTrans();
		//qDebug("%s\t %f\t %f\t %f\t" ,joint->getName().c_str(),trans.x,trans.y,trans.z);
	}
			

	for (int i=0; i<joint->numOfChildren(); i++ )	
		calBonesMatrix( joint->getChild(i), frame_no ,scale);

	glPopMatrix();
}

void Skin::RenderSkin(int frame_no,float scale)
{
	if ( !enable)//没有开启蒙皮
		return;	

	calBonesMatrix(frame_no,scale);

	Vertex* curVertex;
	for (int i=0; i< Vertexes.size();i++)
	{
		curVertex = Vertexes[i];
		Vector3 new_pos = Vector3::ZERO;
		
		std::map<int ,float >::iterator iter;
		for ( iter=curVertex->weights.begin(); iter!=curVertex->weights.end();iter++)
		{
			int BoneID = iter->first;
			float weight = iter->second;
			Bone* bone = getBoneByID(BoneID);
			Matrix4 invRM = bone->getIRM();
			Matrix4 boneMatrix = bone->getNewMat();

			//可以记录下invRM*curVertex->getResetPosition()，这是定值。

			Vector3 reset_pos = curVertex->getResetPosition();

			/*Vector3 temp_pos = Max2OpenGL*reset_pos;*/
			Vector3 temp_pos1 = scale*reset_pos;
			Vector3 temp_pos2 = invRM*temp_pos1;
			Vector3 temp_pos3 = boneMatrix* temp_pos2;
			Vector3 temp_pos4 = weight*temp_pos3;

			new_pos += temp_pos4;
			//new_pos += weight*( boneMatrix*(invRM*(scale*(Max2OpenGL*curVertex->getResetPosition()))));
			
		}
		/*new_pos = (scale*(Max2OpenGL*curVertex->getResetPosition()));*/
		curVertex->setNewPos(new_pos);
	}

	for (int i=0; i< Faces.size(); i++)
	{
		Face* face = Faces[i];

		int v1_ID = face->vertex1_ID;
		int v2_ID = face->vertex2_ID;
		int v3_ID = face->vertex3_ID;

		int n1_ID = face->normal1_ID;
		int n2_ID = face->normal2_ID;
		int n3_ID = face->normal3_ID;

		int uv1_ID = face->uv1_ID;
		int uv2_ID = face->uv2_ID;
		int uv3_ID = face->uv3_ID;

		Vector3 pos1 = getVertexByID(v1_ID)->getNewPos();
		Vector3 pos2 = getVertexByID(v2_ID)->getNewPos();
		Vector3 pos3 = getVertexByID(v3_ID)->getNewPos();

		Vector3 n1 = getNormalByID(n1_ID);n1.normalise();
		Vector3 n2 = getNormalByID(n2_ID);n2.normalise();
		Vector3 n3 = getNormalByID(n3_ID);n3.normalise();

		Vector3 uv1 = getUVByID(uv1_ID);
		Vector3 uv2 = getUVByID(uv2_ID);
		Vector3 uv3 = getUVByID(uv3_ID);
		 
		switch (renderMode)
		{
		case Wire:
			glBegin(GL_LINE_LOOP);							
				glVertex3f(pos1.x,pos1.y,pos1.z);						
				glVertex3f(pos2.x,pos2.y,pos2.z);						
				glVertex3f(pos3.x,pos3.y,pos3.z);
			glEnd();


			break;
		case Gray:
			glBegin(GL_TRIANGLES);
				glColor3f(1.0f,0.8086f,0.6016f);
				glNormal3f(n1.x,n1.y,n1.z);
				glVertex3f(pos1.x,pos1.y,pos1.z);						
				glNormal3f(n2.x,n2.y,n2.z);
				glVertex3f(pos2.x,pos2.y,pos2.z);
				glNormal3f(n3.x,n3.y,n3.z);
				glVertex3f(pos3.x,pos3.y,pos3.z);
			glEnd();

			break;
		case Texture:
			glBegin(GL_TRIANGLES);									
				glTexCoord2f(uv1.x,uv1.y);
				glNormal3f(n1.x,n1.y,n1.z);
				glVertex3f(pos1.x,pos1.y,pos1.z);
							
				glTexCoord2f(uv2.x,uv2.y);
				glNormal3f(n2.x,n2.y,n2.z);
				glVertex3f(pos2.x,pos2.y,pos2.z);
							
				glTexCoord2f(uv3.x,uv3.y);
				glNormal3f(n3.x,n3.y,n3.z);
				glVertex3f(pos3.x,pos3.y,pos3.z);
			glEnd();
			break;		
		}		
	}
}

Face::Face(int v1,int v2,int v3)
{ 
	vertex1_ID = v1;
	vertex2_ID = v2; 
	vertex3_ID = v3;

	normal1_ID = 0;
	normal2_ID = 0;
	normal3_ID = 0;

	uv1_ID = 0;
	uv2_ID = 0;
	uv3_ID = 0;
}
Face::Face(int v1,int v2,int v3 ,int n1,int n2,int n3, int v_n )//n_v为了重载构造函数，本身没有用处
{ 
	vertex1_ID = v1;
	vertex2_ID = v2; 
	vertex3_ID = v3;

	normal1_ID = n1;
	normal2_ID = n2;
	normal3_ID = n3;

	uv1_ID = 0;
	uv2_ID = 0;
	uv3_ID = 0;
}
Face::Face(int v1,int v2,int v3 ,int uv1,int uv2,int uv3)
{ 
	vertex1_ID = v1;
	vertex2_ID = v2; 
	vertex3_ID = v3;

	normal1_ID = 0;
	normal2_ID = 0;
	normal3_ID = 0;

	uv1_ID = uv1;
	uv2_ID = uv2;
	uv3_ID = uv3;
}
Face::Face(int v1,int v2,int v3 ,int n1,int n2,int n3,int uv1,int uv2,int uv3)
{ 
	vertex1_ID = v1;
	vertex2_ID = v2; 
	vertex3_ID = v3;

	normal1_ID = n1;
	normal2_ID = n2;
	normal3_ID = n3;

	uv1_ID = uv1;
	uv2_ID = uv2;
	uv3_ID = uv3;
}

Face::~Face()
{

}	
Face::Face()
{

}	