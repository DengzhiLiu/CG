#ifndef SKINNING_H
#define SKINNING_H

#include "Clip.h"
#include "Vector3.h"
#include "Matrix4.h"

using namespace std;

const static Matrix4 Max2OpenGL = Matrix4( 1.0f,  0.0f,  0.0f,  0.0f,
				   0.0f,  0.0f,  1.0f,  0.0f,
				   0.0f,  -1.0f, 0.0f,  0.0f, 
				   0.0f,  0.0f,  0.0f,  1.0f );

enum RenderMode{ Wire=1, Gray, Texture };
typedef std::map<int ,float > mapBoneID_Weight;
typedef std::vector< mapBoneID_Weight* > Weight_Table; 

class Bone
{
public:
	Bone(std::string n, int id){ name = n; ID = id;}
	~Bone();

	std::string getName(){ return name;}
	int getID(){ return ID;}
	void setResetRad(Vector3 r){ ResetRad =r;}
	Vector3 getResetRad(){ return ResetRad ;}
	void setITM(Matrix4 m){ inv_ResetMatrix = m;}
	Matrix4 getIRM(){ return inv_ResetMatrix; }
	void setNewMat(Matrix4 m){ new_Mat = m;}
	Matrix4 getNewMat(){ return new_Mat;}
protected:
private:
	std::string		name;
	int             ID;
	Matrix4         inv_ResetMatrix;//重置姿势下，该骨骼的变换矩阵逆矩阵
	Vector3         ResetRad;   //重置姿势下，改骨骼相对于父关节的旋转量
	Matrix4         new_Mat;//该骨头在每一帧的变换矩阵
};
class Face
{
public:
	int vertex1_ID;
	int vertex2_ID;
	int vertex3_ID;
	int normal1_ID;
	int normal2_ID;
	int normal3_ID;
	int uv1_ID;
	int uv2_ID;
	int uv3_ID;
	Face();
	~Face();
	Face(int ,int,int);
	Face(int,int,int,int,int,int);
	Face(int,int,int,int,int,int,int);
	Face(int,int,int,int,int,int,int,int,int);
protected:
private:
};

class Vertex
{
public:

	mapBoneID_Weight weights;//map<int BoneID,float weight>
	
	Vertex(int id,Vector3 resetPos)
	{
		ID=id;
		resetPosition = resetPos;		

		weights.clear();

		new_pos = Vector3::ZERO;
	}
	~Vertex(){}

	void setID(int id){ ID = id ; }
	int getID(){return ID;}

	Vector3 getResetPosition(){return resetPosition;}	

	Vector3 getNewPos(){return new_pos;}
	void setNewPos(Vector3 pos){ new_pos = pos;}

protected:

private:	
	int ID;
	Vector3 resetPosition;//ResetPosition;	
	Vector3 new_pos;//每一帧都需要计算，用于绘制
};

class Skin
{
public:

	Skin(Clip* c, RenderMode mode,bool en)
	{
		curClip = c;
		Bones.clear();
		Faces.clear();
		Normals.clear();
		UVs.clear();
		Vertexes.clear();
		weight_table.clear();

		enable = en;
		renderMode = mode;
	}
	~Skin()
	{
		Bones.clear();
		Faces.clear();
		Normals.clear();
		UVs.clear();
		Vertexes.clear();
		weight_table.clear();
	}

	//int readMeshInfo(const std::string filename);//bones and bones weight
	int readObjFile(const std::string filename);//bones and bones weight
	int readWeightInfo(const std::string filename);//vertexes and faces
	void calBonesInvResetMatrix(Joint* joint, float scale);//怎么计算？　在3dmax中查看各关节旋转角度,写到xml中
	
	Bone* getBoneByID(int id);
	Bone* getBoneByName(std::string name);
	std::string getBoneNameByID(int id);

	Vertex* getVertexByID(int id);
	Vector3 getNormalByID(int id);
	Vector3 getUVByID(int id);

	void setCurClip(Clip* c);

	void calBonesMatrix(int frame_no,float scale);//计算蒙皮需要的骨骼当前帧的变换矩阵
	void calBonesMatrix(Joint* joint,int frame_no,float scale);
	void RenderSkin(int frame_no,float scale);

	bool getEnable(){ return enable; }
	void setSkinOnOff(bool on);
	void ShowWeightTable();
	void setRenderMode(RenderMode mode);
	
protected:
private:
	
	Clip* curClip;

	std::map<int ,Bone* > Bones;//map<int boneID, Bone* b>
	std::map<int ,Vertex* > Vertexes;//map<int vertexID,Vertex* v>
	std::map<int ,Vector3 > Normals;
	std::map<int ,Vector3 > UVs;
	std::vector<Face* > Faces;	

	bool enable;
	RenderMode renderMode;
	Weight_Table weight_table;

	Vector3 HipPos;
	Vector3 HipRad;
};

#endif //SKINNING_H