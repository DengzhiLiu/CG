//#include "stdafx.h"
#include "ObjReader.h"



void ObjReader::LoadMaterial(char *szFileName,char * materialStr,ObjObject * obj)
{
   FILE *file = fopen(szFileName, "rb");
  
   char      buf[MAX_PATH];//存储读入信息
  
   while(fscanf(file, "%s", buf) != EOF) 
   {
	
	 if(stricmp(buf, "newmtl") == 0)
	 {
       fscanf(file, "%s", buf);
          
       if(stricmp(materialStr,buf)!=0) continue;//未找到
       
       
	   while(fscanf(file, "%s", buf) != EOF) 
	   {
	    //读取数据

        if( 0 == stricmp(buf, "Ka") )
        {  
		  fscanf(file,"%f",&obj->vAmbient.X());// 环境光属性
		  fscanf(file,"%f",&obj->vAmbient.Y());
		  fscanf(file,"%f",&obj->vAmbient.Z());
        }
        else if(0== stricmp(buf, "Kd" ) )
        {
	      fscanf(file,"%f",&obj->vDiffuse.X());// 散射光属性
		  fscanf(file,"%f",&obj->vDiffuse.Y());
		  fscanf(file,"%f",&obj->vDiffuse.Z());
        }
		else if( 0 == stricmp(buf,"Ks") )
		{ 
		  fscanf(file,"%f",&obj->vSpecular.X());// 反射光属性
		  fscanf(file,"%f",&obj->vSpecular.Y());
		  fscanf(file,"%f",&obj->vSpecular.Z());
		}
		else if( 0 == stricmp(buf, "Ns") )
		{	
		 fscanf(file,"%f",&obj->fShiness);//反射因子
		}
        else if( 0 == stricmp(buf, "map_Kd" ) )
        {  
		 fscanf(file,"%s",&obj->texName); // 纹理	
        }
		else if( 0 == stricmp(buf, "#" ) )
        {  
		 fclose(file);
		 return;//处理结束

        }

	   }

	 }
   }
	fclose(file);


}
bool ObjReader::LoadObj(char *szFileName)
{
	FILE *file = fopen(szFileName, "rb");

	ObjObject * obj = NULL;//模型

	char materialFile[MAX_PATH];
	char materialStr[MAX_PATH];
	char tempStr[MAX_PATH];
	char      buf[MAX_PATH];//存储读入信息
	
	Point3f position;//顶点信息
    Point3f texCoord;
    Point3f normal; 
    
    int posIndex;//三角形信息
	int texIndex;
	int norIndex;
	
	while(fscanf(file, "%s", buf) != EOF) 
	{
	  //读取数据
	  switch(buf[0]) 
	  {
	   case 'm'://材质库
	    for(int i=0;i<MAX_PATH;i++)materialFile[i] = 0;
		for(int i=0;i<MAX_PATH;i++)tempStr[i] = 0;
        fscanf(file,"%s",tempStr);
		for(int i=0;i<MAX_PATH;i++)
		{
         if(szFileName[i] =='.')
		 {
		  while(szFileName[i]!='\\'&&i>=0)i--;
          for(int j=0;j<=i;j++)
          materialFile[j] = szFileName[j];//路径
          for(int j=i+1;j<MAX_PATH;j++)
          materialFile[j] = tempStr[j-i-1];//文件名
		  break;
		 }

		}
		
		break;

	  case 'u'://材质信息
		for(int i=0;i<MAX_PATH;i++)materialStr[i] = 0;
		fscanf(file, "%s", materialStr);

		LoadMaterial(materialFile,materialStr,obj);//读取材质信息
		break;

	   case 'g'://物体
	   if(obj == NULL) 
	   {
		obj = new ObjObject;//构造新模型  
	    m_Object.push_back(obj);//存入物体
	   }
	   if(obj!=NULL && (int)obj->posIndices.size()>0)//构造新模型  
	   { 
		 obj = new ObjObject;//构造新模型 
		 m_Object.push_back(obj);
	   }
	   break;

	   case 'v':  //如果读取到 v,vt,vn 数据
		   
		 switch(buf[1]) 
	     {
		   case '\0':			//顶点坐标
			  fscanf(file, "%f %f %f", &position.X(),&position.Y(), &position.Z());  
			  Positions.push_back(position);//存储顶点信息
			  break;

		   case 't'://纹理坐标
              fscanf(file, "%f %f %f", &texCoord.X(),&texCoord.Y(), &texCoord.Z());  
			  TexCoords.push_back(texCoord);//存储纹理信息
			   break;
		   case 'n'://法线
              fscanf(file, "%f %f %f", &normal.X(),&normal.Y(), &normal.Z());  
			  Normals.push_back(normal);//存储法线信息
			  break;
		    
		  }
		 break;
		
		case 'f':	//如果读取到面数据			
		
		  fscanf(file, "%s", buf);
          //obj文件索引从1开始计数
		  if(sscanf(buf, "%d//%d", &posIndex, &norIndex)==2) //顶点,法线
		  {	
            obj->posIndices.push_back(posIndex-1);
			obj->norIndices.push_back(norIndex-1);

			fscanf(file, "%d//%d", &posIndex, &norIndex);
			obj->posIndices.push_back(posIndex-1);
			obj->norIndices.push_back(norIndex-1);

			fscanf(file, "%d//%d", &posIndex, &norIndex);
			obj->posIndices.push_back(posIndex-1);
			obj->norIndices.push_back(norIndex-1);
		   }
		  else if (sscanf(buf, "%d/%d/%d", &posIndex, &texIndex, &norIndex) == 3) //顶点 纹理 法线 
		  {
			obj->posIndices.push_back(posIndex-1);
			obj->texIndices.push_back(texIndex-1);
			obj->norIndices.push_back(norIndex-1);

			fscanf(file, "%d/%d/%d",  &posIndex, &texIndex, &norIndex);
			obj->posIndices.push_back(posIndex-1);
			obj->texIndices.push_back(texIndex-1);
			obj->norIndices.push_back(norIndex-1);

			fscanf(file, "%d/%d/%d",  &posIndex, &texIndex, &norIndex);	
			obj->posIndices.push_back(posIndex-1);
			obj->texIndices.push_back(texIndex-1);
			obj->norIndices.push_back(norIndex-1);
		  }
		  else if (sscanf(buf, "%d/%d", &posIndex, &texIndex) == 2) //顶点 纹理
		  {	
			obj->posIndices.push_back(posIndex-1);
			obj->texIndices.push_back(texIndex-1);

			fscanf(file, "%d/%d",&posIndex, &texIndex);
			obj->posIndices.push_back(posIndex-1);
			obj->texIndices.push_back(texIndex-1);
				
			fscanf(file, "%d/%d", &posIndex, &texIndex);	
			obj->posIndices.push_back(posIndex-1);
			obj->texIndices.push_back(texIndex-1);
		   } 
		   else  if(sscanf(buf, "%d", &posIndex) == 1) //顶点 
		   {
			obj->posIndices.push_back(posIndex-1);

			fscanf(file, "%d", &posIndex);
			obj->posIndices.push_back(posIndex-1);

			fscanf(file, "%d", &posIndex);
			obj->posIndices.push_back(posIndex-1); 
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

void ObjReader::Release()
{
 for(int i=0;i<m_Object.size();i++)
 {
  
   m_Object[i]->norIndices.clear();
   m_Object[i]->posIndices.clear();
   m_Object[i]->texIndices.clear();

   delete m_Object[i];
 }
  m_Object.clear();

   TexCoords.clear();
   Normals.clear();
   Positions.clear();

}
