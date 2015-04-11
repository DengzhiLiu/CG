#ifndef _OBJREADER_
#define _OBJREADER_

#include<vector>
#include"../Math/Point3.h"

#define MAX_PATH 100

/*读取多个物体，且包含材质信息*
 *                            *
 *                            *
 *                            */

//中间数据存储格式
struct ObjObject
{
  ObjObject()
  {
   for(int i=0;i<MAX_PATH;i++)texName[i] = 0;
   vAmbient = Point3f(0,0,0);
   vDiffuse = Point3f(0,0,0);
   vSpecular = Point3f(0,0,0);
   fShiness = 0.0;
  }

 
  std::vector<int> posIndices;//三角形列表
  std::vector<int> texIndices;
  std::vector<int> norIndices;

  //材质
  char texName[MAX_PATH];//纹理名称
  Point3f vAmbient;//环境光
  Point3f vDiffuse;//散射光
  Point3f vSpecular;//反射光
  float fShiness;//反射系数
};


class ObjReader
{
 public:

   std::vector<Point3f> Positions;//顶点数据
   std::vector<Point3f> TexCoords;
   std::vector<Point3f> Normals; 
   std::vector<ObjObject *> m_Object;//中间数据

   void LoadMaterial(char *szFileName,char * materialStr,ObjObject * obj);//导入材质信息

   bool LoadObj(char *szFileName);//从文件中导入几何信息
   
   void Release();//释放资源

};


#endif