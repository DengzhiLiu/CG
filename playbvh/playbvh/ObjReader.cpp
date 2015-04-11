//#include "stdafx.h"
#include "ObjReader.h"



void ObjReader::LoadMaterial(char *szFileName,char * materialStr,ObjObject * obj)
{
   FILE *file = fopen(szFileName, "rb");
  
   char      buf[MAX_PATH];//�洢������Ϣ
  
   while(fscanf(file, "%s", buf) != EOF) 
   {
	
	 if(stricmp(buf, "newmtl") == 0)
	 {
       fscanf(file, "%s", buf);
          
       if(stricmp(materialStr,buf)!=0) continue;//δ�ҵ�
       
       
	   while(fscanf(file, "%s", buf) != EOF) 
	   {
	    //��ȡ����

        if( 0 == stricmp(buf, "Ka") )
        {  
		  fscanf(file,"%f",&obj->vAmbient.X());// ����������
		  fscanf(file,"%f",&obj->vAmbient.Y());
		  fscanf(file,"%f",&obj->vAmbient.Z());
        }
        else if(0== stricmp(buf, "Kd" ) )
        {
	      fscanf(file,"%f",&obj->vDiffuse.X());// ɢ�������
		  fscanf(file,"%f",&obj->vDiffuse.Y());
		  fscanf(file,"%f",&obj->vDiffuse.Z());
        }
		else if( 0 == stricmp(buf,"Ks") )
		{ 
		  fscanf(file,"%f",&obj->vSpecular.X());// ���������
		  fscanf(file,"%f",&obj->vSpecular.Y());
		  fscanf(file,"%f",&obj->vSpecular.Z());
		}
		else if( 0 == stricmp(buf, "Ns") )
		{	
		 fscanf(file,"%f",&obj->fShiness);//��������
		}
        else if( 0 == stricmp(buf, "map_Kd" ) )
        {  
		 fscanf(file,"%s",&obj->texName); // ����	
        }
		else if( 0 == stricmp(buf, "#" ) )
        {  
		 fclose(file);
		 return;//�������

        }

	   }

	 }
   }
	fclose(file);


}
bool ObjReader::LoadObj(char *szFileName)
{
	FILE *file = fopen(szFileName, "rb");

	ObjObject * obj = NULL;//ģ��

	char materialFile[MAX_PATH];
	char materialStr[MAX_PATH];
	char tempStr[MAX_PATH];
	char      buf[MAX_PATH];//�洢������Ϣ
	
	Point3f position;//������Ϣ
    Point3f texCoord;
    Point3f normal; 
    
    int posIndex;//��������Ϣ
	int texIndex;
	int norIndex;
	
	while(fscanf(file, "%s", buf) != EOF) 
	{
	  //��ȡ����
	  switch(buf[0]) 
	  {
	   case 'm'://���ʿ�
	    for(int i=0;i<MAX_PATH;i++)materialFile[i] = 0;
		for(int i=0;i<MAX_PATH;i++)tempStr[i] = 0;
        fscanf(file,"%s",tempStr);
		for(int i=0;i<MAX_PATH;i++)
		{
         if(szFileName[i] =='.')
		 {
		  while(szFileName[i]!='\\'&&i>=0)i--;
          for(int j=0;j<=i;j++)
          materialFile[j] = szFileName[j];//·��
          for(int j=i+1;j<MAX_PATH;j++)
          materialFile[j] = tempStr[j-i-1];//�ļ���
		  break;
		 }

		}
		
		break;

	  case 'u'://������Ϣ
		for(int i=0;i<MAX_PATH;i++)materialStr[i] = 0;
		fscanf(file, "%s", materialStr);

		LoadMaterial(materialFile,materialStr,obj);//��ȡ������Ϣ
		break;

	   case 'g'://����
	   if(obj == NULL) 
	   {
		obj = new ObjObject;//������ģ��  
	    m_Object.push_back(obj);//��������
	   }
	   if(obj!=NULL && (int)obj->posIndices.size()>0)//������ģ��  
	   { 
		 obj = new ObjObject;//������ģ�� 
		 m_Object.push_back(obj);
	   }
	   break;

	   case 'v':  //�����ȡ�� v,vt,vn ����
		   
		 switch(buf[1]) 
	     {
		   case '\0':			//��������
			  fscanf(file, "%f %f %f", &position.X(),&position.Y(), &position.Z());  
			  Positions.push_back(position);//�洢������Ϣ
			  break;

		   case 't'://��������
              fscanf(file, "%f %f %f", &texCoord.X(),&texCoord.Y(), &texCoord.Z());  
			  TexCoords.push_back(texCoord);//�洢������Ϣ
			   break;
		   case 'n'://����
              fscanf(file, "%f %f %f", &normal.X(),&normal.Y(), &normal.Z());  
			  Normals.push_back(normal);//�洢������Ϣ
			  break;
		    
		  }
		 break;
		
		case 'f':	//�����ȡ��������			
		
		  fscanf(file, "%s", buf);
          //obj�ļ�������1��ʼ����
		  if(sscanf(buf, "%d//%d", &posIndex, &norIndex)==2) //����,����
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
		  else if (sscanf(buf, "%d/%d/%d", &posIndex, &texIndex, &norIndex) == 3) //���� ���� ���� 
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
		  else if (sscanf(buf, "%d/%d", &posIndex, &texIndex) == 2) //���� ����
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
		   else  if(sscanf(buf, "%d", &posIndex) == 1) //���� 
		   {
			obj->posIndices.push_back(posIndex-1);

			fscanf(file, "%d", &posIndex);
			obj->posIndices.push_back(posIndex-1);

			fscanf(file, "%d", &posIndex);
			obj->posIndices.push_back(posIndex-1); 
		   }
		  break;
	
		default:
		  fgets(buf, sizeof(buf), file);//����һ��
		  break;
		}   		
}
 fclose(file);//�ر��ļ�
		
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
