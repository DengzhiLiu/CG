#include "stdafx.h"
#include "DefMesh.h"
#include "../HALib/tinyxml.h"
#include "../HALib/ObjLoader.h"
#include "../Common/utildefines.h"

DefMesh::DefMesh(void)
{
	m_pBones.clear();
	m_pClip = NULL;
}

DefMesh::~DefMesh(void)
{
	map<int, Bone *>::iterator iter;
	for (iter = m_pBones.begin(); iter != m_pBones.end(); ++iter)
		delete iter->second;
	m_pBones.clear();

	if (m_pClip)
		delete m_pClip;
}

bool DefMesh::ImportMesh(string fileName)
{
	mesh = Mesh();
	string surfix = string(fileName, fileName.rfind('.') + 1);
	if(_stricmp(surfix.c_str(), "obj") == 0) {
		CObjLoader objLoader;
		objLoader.ImportObj(&mesh, fileName);
	}
	
	hwd.m_pDefmesh = this;
	glw.m_pDefmesh = this;
	glw.BindTexture();
	
	curMesh = mesh;
	glw.BuildList();
	hwd.InitShader();

	return true;
}

bool DefMesh::ImportMotion(string fileName)
{
	if (m_pClip)
		delete m_pClip;
	m_pClip = new Clip();

	m_pClip->readMotionCap(fileName);
	Vector3 offset = m_pClip->getJointByID(0)->getOffset();
	toAdd = Vector3(0, offset[1], 0);

	return true;
}

void DefMesh::OutputJoint(Joint* pjoint, int depth, FILE* fp)
{
	for (int i = 0; i < depth; i++)
		fprintf(fp, "\t");

	// JOINT
	if (pjoint->getType() == ROOT)
		fprintf(fp, "ROOT %s\n", pjoint->getName().c_str());
	else if (pjoint->getType() == JOINT)
		fprintf(fp, "JOINT %s\n", pjoint->getName().c_str());
	else if (pjoint->getType() == EFFECTOR)
		fprintf(fp, "End Site\n");

	for (int i = 0; i < depth; i++)
		fprintf(fp, "\t");
	fprintf(fp, "{\n");

	// OFFSET
	for (int i = 0; i <= depth; i++)
		fprintf(fp, "\t");
	Vector3 offset = pjoint->getOffset();
	fprintf(fp, "OFFSET\t%.4f\t%.4f\t%.4f\n", offset[0], offset[1], offset[2]);

	// Channels
	Channel* pchannel;
	int channel_num = pjoint->getChannelNumber();
	if (channel_num) {
		for (int i = 0; i <= depth; i++)
			fprintf(fp, "\t");
		fprintf(fp, "CHANNELS %d ", channel_num);

		for (int i = 0; i < channel_num; i++)	{
			pchannel = pjoint->getChannel(i);
			int type = pchannel->getType();
			switch (type)
			{
			case	X_POSITION:
				fprintf(fp, "Xposition ");	break;
			case	Y_POSITION:
				fprintf(fp, "Yposition ");	break;
			case	Z_POSITION:
				fprintf(fp, "Zposition ");	break;
			case	X_ROTATION:
				fprintf(fp, "Xrotation ");	break;
			case	Y_ROTATION:
				fprintf(fp, "Yrotation ");	break;
			case	Z_ROTATION:
				fprintf(fp, "Zrotation ");	break;
			default:
				break;
			}
		}
		fprintf(fp, "\n");
	}

	int child_num = pjoint->numOfChildren();
	for (int i = 0; i < child_num; i++)
		OutputJoint(pjoint->getChild(i), depth + 1, fp);

	for (int i = 0; i < depth; i++)
		fprintf(fp, "\t");
	fprintf(fp, "}\n");
}

bool DefMesh::SaveMotion(string fileName)
{
	FILE* fp = fopen(fileName.c_str(), "w");

	// OUTPUT SKELETON
	fprintf(fp, "HIERARCHY\n");
	OutputJoint(m_pClip->getJointByID(0), 0, fp);
	
	// OUTPUT MOTION DATA
	fprintf(fp, "MOTION\n");
	fprintf(fp, "Frames: %d\n", m_pClip->getNumFrames());
	fprintf(fp, "Frame Time: %.6f\n", m_pClip->getInterval());

	Joint* pjoint;
	int joint_num = m_pClip->getJointNum();
	for (int i = 0; i < m_pClip->getNumFrames(); i++)	{
		for (int j = 0; j < joint_num; j++)	{
			pjoint = m_pClip->getJointByID(j);
			Transform<> trans = pjoint->GetReMotionPose(i);

			Vector3 offset;
			if (pjoint->getType() == ROOT)
				offset = trans.getTrans() * 2;
			else
				offset = trans.getTrans();

			Quaternion<> rot = trans.getRot();
			float pitch = rot.getPitch(true) / PI * 180.0;			// angle x
			float yaw = rot.getYaw(true) / PI * 180.0;				// angle y
			float roll = rot.getRoll(true) / PI * 180.0;			// angle z
			
			for (int k = 0; k < pjoint->getChannelNumber(); k++)  {
				Channel* pchanel = pjoint->getChannel(k);
				int type = pchanel->getType();
				switch (type)
				{
				case	X_POSITION:
					fprintf(fp, "%.6f ", offset[0]);	break;
				case	Y_POSITION:
					fprintf(fp, "%.6f ", offset[1]);	break;
				case	Z_POSITION:
					fprintf(fp, "%.6f ", offset[2]);	break;
				case	X_ROTATION:
					fprintf(fp, "%.6f ", pitch);		break;
				case	Y_ROTATION:
					fprintf(fp, "%.6f ", yaw);			break;
				case	Z_ROTATION:
					fprintf(fp, "%.6f ", roll);			break;
				default:
					break;
				}
			}

		}
		fprintf(fp, "\n");
	}

	fclose(fp);
	return true;
}

Bone* DefMesh::getBoneByID(int id)
{
	return m_pBones[id];
}

BoneVector DefMesh::getBoneByName(string name)
{	
	map<int , Bone* >::iterator iter;
	BoneVector boneVector;
	boneVector.clear();

	for (iter = m_pBones.begin(); iter != m_pBones.end(); iter++) {
		Bone* bone = iter->second;
		if ( bone->getName() == name )
			boneVector.push_back(bone);
	}

	return boneVector;
}

bool DefMesh::ImportWeight(string fileName)
{
	if (!m_pClip)	return false;

	vector<MVert> verts = mesh.getVerts();

	TiXmlDocument* WeightDoc =new TiXmlDocument(fileName.c_str());

	if(!WeightDoc->LoadFile(fileName.c_str()))	return false;

	TiXmlElement* xml_root = WeightDoc->FirstChild("Skin")->ToElement();
	TiXmlElement* xml_BoneInfo = xml_root->FirstChild("BoneInfo")->ToElement();
	//int BoneNUM = atoi( xml_BoneInfo->Attribute("Num"));
	//Bones.resize(VertexNUM); ???

	HipPos[0] = atof(xml_BoneInfo->Attribute("HipPosX"));
	HipPos[1] = atof(xml_BoneInfo->Attribute("HipPosY"));
	HipPos[2] = atof(xml_BoneInfo->Attribute("HipPosZ"));
	HipRad[0] = atof(xml_BoneInfo->Attribute("HipRadX"));
	HipRad[1] = atof(xml_BoneInfo->Attribute("HipRadY"));
	HipRad[2] = atof(xml_BoneInfo->Attribute("HipRadZ"));	

	TiXmlNode* xml_BoneNode = xml_BoneInfo->FirstChild("Bone");
	Bone* curBone;
	while ( xml_BoneNode)
	{
		//<BoneInfo Num="20">
		//<Bone Name="lradius" ID="0" />
		string name;
		int boneID;
		float Xrad;
		float Yrad;
		float Zrad;
		Joint* pjoint;

		TiXmlElement* xml_Bone = xml_BoneNode->ToElement();

		name =  xml_Bone->Attribute("Name");
		boneID = atoi(xml_Bone->Attribute("ID"));
		xml_BoneNode = xml_BoneNode->NextSibling("Bone");
	
		curBone = new Bone(name, boneID);
		pjoint = m_pClip->getJointByName(name);
		// add error handle later
		if (pjoint == NULL)		continue;
		
		pjoint->setUsed(true);
		curBone->setHead(pjoint);
		m_pBones[boneID] = curBone;

		Xrad = atof(xml_Bone->Attribute("Xrad"));
		Yrad = atof(xml_Bone->Attribute("Yrad"));
		Zrad = atof(xml_Bone->Attribute("Zrad"));

		Quaternion<> qrot = Quaternion<>(1, Vector3(0, 0, 0));
		qrot = qrot * Quaternion<>(Vector3(1, 0, 0), (float)(Xrad/180.0*PI));
		qrot = qrot * Quaternion<>(Vector3(0, 1, 0), (float)(Yrad/180.0*PI));
		qrot = qrot * Quaternion<>(Vector3(0, 0, 1), (float)(Zrad/180.0*PI));
		curBone->setBindRot(qrot);

		//xml_BoneNode = xml_BoneNode->NextSibling("Bone");
	}

	TiXmlElement* xml_VertexInfo = xml_root->FirstChild("VertexInfo")->ToElement();
	//int VertexNUM = atoi(xml_VertexInfo->Attribute("Num"));

	TiXmlNode* xml_VertexNode = xml_VertexInfo->FirstChild("Vertex");
	MVert *curVertex;
	while ( xml_VertexNode )
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
		curVertex = &(verts[vertexID]);

		TiXmlElement* xml_BoneList = xml_Vertex->FirstChild("BoneList")->ToElement();
		BoneListNum = atoi( xml_BoneList->Attribute("Num"));

		map<int, float> weightmap;
		weightmap.clear();
		TiXmlNode* xml_boneNode = xml_BoneList->FirstChild("bone");
		while ( xml_boneNode )
		{
			TiXmlElement* xml_bone = xml_boneNode->ToElement();

			boneID = atoi( xml_bone->Attribute("ID"));
			weight = atof( xml_bone->Attribute("weight"));

			if (weightmap.count(boneID))
				weightmap[boneID] += weight;
			else
				weightmap[boneID] = weight;
			//weightmap[boneID] = weight;
			//curVertex->weights[boneID] = weight;

			xml_boneNode = xml_boneNode->NextSibling("bone");
		}

		// per vertex only bind to four bones
		int index = 0;
		map<int, float>::iterator iter;
		int bonenum = (int)weightmap.size();
		if (bonenum <= 4) {
			for (iter = weightmap.begin(); iter != weightmap.end(); iter++) {
				curVertex->bone[index] = iter->first;
				curVertex->weight[index] = iter->second;
				index++;
			}
			while (index < 4) {
				curVertex->bone[index] = -1;
				curVertex->weight[index] = 0;
				index++;
			}
		}
		else {
			while (weightmap.size() > 4) {
				int   minindex = -1;
				float minweight = 1e10;
				for (iter = weightmap.begin(); iter != weightmap.end(); iter++) {
					if (iter->second < minweight) {
						minweight = iter->second;
						minindex = iter->first;
					}
				}
				weightmap.erase(minindex);
			}

			// now curvertex binded to four bones
			float weightsum = 0;
			for (iter = weightmap.begin(); iter != weightmap.end(); iter++)
				weightsum += iter->second;
			for (iter = weightmap.begin(); iter != weightmap.end(); iter++) {
				curVertex->bone[index] = iter->first;
				curVertex->weight[index] = iter->second / weightsum;
				index++;
			}
		}

		xml_VertexNode = xml_VertexNode->NextSibling( "Vertex" ); 
	}

	WeightDoc->Clear();
	delete WeightDoc;

	mesh.setVerts(verts);
	curMesh.setVerts(verts);
	ComputeBindPose(m_pClip->getJointByID(0));
	ComputeMotion();

	glw.BuildSkelList();
	hwd.SetVSBindMat();

	return true;
}

void DefMesh::ComputeBindPose(Joint* pjoint)
{
	Vector3	trans;
	Joint*	parent;
	Quaternion<> qrot = Quaternion<>(1., Vector3());
	Transform<> transf = Transform<>(qrot, 1, Vector3());

	parent = pjoint->getParent();
	if (parent)
		transf = parent->GetRestPose();

	//if (pjoint->getParent() == NULL)
	//	trans = Vector3(HipPos[0], HipPos[1], HipPos[2]);
	//else
		trans = pjoint->getOffset();			// 根结点的偏移记录在bvh文件中，也存储于Offset

	//Bone* pbone = getBoneByName(pjoint->getName());
	BoneVector pbones = getBoneByName(pjoint->getName());
	if ( pjoint->getParent() == NULL )	{
		qrot = qrot * Quaternion<>(Vector3(1, 0, 0), (float)(HipRad[0]/180.0*PI));
		qrot = qrot * Quaternion<>(Vector3(0, 1, 0), (float)(HipRad[1]/180.0*PI));
		qrot = qrot * Quaternion<>(Vector3(0, 0, 1), (float)(HipRad[2]/180.0*PI));
	}
	else {
		if (pbones.size())
			qrot = pbones[0]->getBindRot();
	}

	transf = transf * Transform<>(qrot, 1, trans);
	pjoint->setPosition(transf.getTrans());
	pjoint->SetRestPose(transf);

	//if (pbone)
	//	pbone->setBindTrans(transf);
	for (size_t i = 0; i < pbones.size(); i++)
		pbones[i]->setBindTrans(transf.inverse());

	for (int i = 0; i < pjoint->numOfChildren(); i++ )	
		ComputeBindPose( pjoint->getChild(i) );
}

void DefMesh::ComputeMotion()
{
	int num_frame = m_pClip->getNumFrames();
	for (int i = 0; i < num_frame; i++)
		ComputeMotion(m_pClip->getJointByID(0), i);
}

void DefMesh::ComputeMotion(Joint *pjoint, int frame_no)
{
	//Bone* pbone = getBoneByName(pjoint->getName());
	BoneVector pbones = getBoneByName(pjoint->getName());
	Transform<> transf = pjoint->GetAbMotionPose(frame_no);

	//if (pbone)
	//	pbone->addMotion(transf);
	for (size_t i = 0; i < pbones.size(); i++)
		pbones[i]->addMotion(transf);

	for (int i = 0; i < pjoint->numOfChildren(); i++ )	
		ComputeMotion(pjoint->getChild(i), frame_no);
}

void DefMesh::InsertMotion(int frame)
{
	if (m_pClip == NULL)	return;
	
	Joint* pjoint;
	int joint_num = m_pClip->getJointNum();
	for (int i = 0; i < joint_num; i++)	{
		pjoint = m_pClip->getJointByID(i);
		pjoint->InsertKey(frame);

		BoneVector pbones = getBoneByName(pjoint->getName());
		for (size_t i = 0; i < pbones.size(); i++)
			pbones[i]->insertMotion(frame+1, pjoint->GetAbMotionPose(frame+1));
	}
	m_pClip->setFrameNum(m_pClip->getNumFrames() + 1);
}

void DefMesh::DeformMesh(int frame, int renderType)
{
	if (getBoneNum() == 0)
		return;

	if (renderType == CPURENDER) {
		MVert curVert;
		vector<MVert> verts = mesh.getVerts();

		for (int i = 0; i < mesh.totvert; i++)	{
			Vector3 newPos, newNormal;
			curVert = verts[i];
			
			for (int j = 0; j < 4; j++)	{				// maximum number of bones binded is four
				int BoneID = curVert.bone[j];
				if (BoneID == -1)	break;

				float weight = curVert.weight[j];
				Bone* bone = getBoneByID(BoneID);
				Transform<> bindtransinv = bone->getBindTrans();
				Transform<> motiontrans = bone->getMotion(frame);

				Vector3 temp_pos = motiontrans * (bindtransinv * curVert.coord) * weight;
				newPos += temp_pos;
				newNormal += motiontrans.getRot() * curVert.normal * weight;
			}
			verts[i].coord = newPos + toAdd;				// hold on the floor
			verts[i].normal = newNormal.normalize();
		}

		curMesh.setVerts(verts);
		glw.BuildList();
	}
	else							//	GPU RENDER
		hwd.SetVSMotionMat(frame);

	UpdateSkeleton(frame);
	glw.BuildSkelList();
}

void DefMesh::Reset()
{
	if (m_pClip) {
		Joint* pjoint;
		Transform<> transf;
		int joint_num = m_pClip->getJointNum();
		for (int i = 0; i < joint_num; i++)	{
			pjoint = m_pClip->getJointByID(i);
			transf = pjoint->GetRestPose();
			pjoint->setPosition(transf.getTrans());
		}
		curMesh.setVerts(mesh.getVerts());
	}

	glw.BuildList();
	glw.BuildSkelList();
	hwd.SetPlay(false);
}

void DefMesh::UpdateSkeleton(int frame)
{
	Joint* pjoint;
	Transform<> transf;
	int joint_num = m_pClip->getJointNum();
	for (int i = 0; i < joint_num; i++) {
		pjoint = m_pClip->getJointByID(i);
		transf = pjoint->GetAbMotionPose(frame);
		pjoint->setPosition(transf.getTrans() + toAdd);			// toAdd holds the foot on the floor
	}
}

void DefMesh::RenderMesh(GLW::DrawMode dm, GLW::ShadeMode sm, bool texture, bool shadow, int renderType)
{
	if (renderType == CPURENDER) {
		glw.setDrawMode(dm);
		glw.setShadeMode(sm);
		glw.setTexture(texture);
		glw.DrawMesh();
	}
	else {
		glDisable(GL_LIGHTING);
		hwd.setDrawMode(dm);
		hwd.setShadeMode(sm);
		hwd.setTexture(texture);
		hwd.SetShadow(shadow);
		hwd.DrawMesh();
		glEnable(GL_LIGHTING);
	}
}

void DefMesh::RenderSkeleton(GLW::DrawMode dm)
{
	glw.setDrawMode(dm);
	glw.DrawSkel();
}