#ifndef _OBJREADER_
#define _OBJREADER_

#include<vector>
#include"../Math/Point3.h"

#define MAX_PATH 100

/*��ȡ������壬�Ұ���������Ϣ*
 *                            *
 *                            *
 *                            */

//�м����ݴ洢��ʽ
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

 
  std::vector<int> posIndices;//�������б�
  std::vector<int> texIndices;
  std::vector<int> norIndices;

  //����
  char texName[MAX_PATH];//��������
  Point3f vAmbient;//������
  Point3f vDiffuse;//ɢ���
  Point3f vSpecular;//�����
  float fShiness;//����ϵ��
};


class ObjReader
{
 public:

   std::vector<Point3f> Positions;//��������
   std::vector<Point3f> TexCoords;
   std::vector<Point3f> Normals; 
   std::vector<ObjObject *> m_Object;//�м�����

   void LoadMaterial(char *szFileName,char * materialStr,ObjObject * obj);//���������Ϣ

   bool LoadObj(char *szFileName);//���ļ��е��뼸����Ϣ
   
   void Release();//�ͷ���Դ

};


#endif