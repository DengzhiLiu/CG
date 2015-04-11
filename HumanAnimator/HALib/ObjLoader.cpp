#include "ObjLoader.h"
#include "../Common/utils.h"

extern "C" {
//#define XMD_H
//#define HAVE_BOOLEAN
#include "jpeglib.h"
}
#include <fstream>
#include <iostream>

using namespace std;

CObjLoader::CObjLoader(void)
{
}

CObjLoader::~CObjLoader(void)
{
}

bool CObjLoader::ImportObj(Mesh *pmesh, string name)
{
	pmesh->ModelFileName = name;
	ifstream obj(name.c_str());
	if(!obj.is_open())
		return false;

	ReadObjFile(pmesh, obj);
	obj.close();
	if (pmesh->totvert == 0)
		return false;
	
	pmesh->normalizeBoundBox();
	pmesh->computeVertexNormals();
	
	return true;
}

void CObjLoader::ReadObjFile(Mesh *pmesh, istream &strm)
{
	int mtlID;
	bool hasUV = false;
	int lineNum = 0;

	while(!strm.eof()) {
		++lineNum;
		vector<string> words = readWords(strm);

		if(words.size() == 0)
			continue;

		switch (words[0][0]) {
			case 'm':								// mtl file
				pmesh->MtlFileName = words[1];
				ReadMtlFile(pmesh, words[1]);
				break;

			case 'g':								// vertex group
				//pmesh->totgroup++;
				//pmesh->m_Groups.resize(pmesh->totgroup);
				//pmesh->m_Groups.back().GName = words[1];
				break;

			case 'u':								// find the material ID
				mtlID = FindMaterial(pmesh, words[1]);
				if (mtlID == -1)	break;
				if (pmesh->totgroup)
					pmesh->m_Groups.back().endf = pmesh->totface - 1;

				pmesh->totgroup++;
				pmesh->m_Groups.resize(pmesh->totgroup);
				pmesh->m_Groups.back().mtlID = mtlID;
				pmesh->m_Groups.back().startf = pmesh->totface;
				if (pmesh->m_pMaterial[mtlID]->ptexbuffer)
					pmesh->m_Groups.back().bTexture = true;
				break;

			case 'v':								// vertex/uv/normals
				if (words[0].size() == 1)	{
					float x, y, z;
					sscanf(words[1].c_str(), "%f", &x);
					sscanf(words[2].c_str(), "%f", &y);
					sscanf(words[3].c_str(), "%f", &z);

					MVert vert;
					vert.coord = Vector3(x, y, z);
					pmesh->totvert++;
					pmesh->m_Vertices.push_back(vert);
				}
				else if (words[0][1] == 't') {
					float x, y, z;
					sscanf(words[1].c_str(), "%f", &x);
					sscanf(words[2].c_str(), "%f", &y);
					if (words.size() == 4)
						sscanf(words[3].c_str(), "%f", &z);
	
					pmesh->totcoord++;
					pmesh->m_Coords.push_back(Vector3(x, y, z));
				}
				else
					break;

				break;

			case 'f':			 //	v/vt/vn		// a[0]~a[15] stores vertexes, a[16]~a[31] stores normals
				int pos;
				int a[32];
				for (size_t i = 0; i < words.size()-1; i++) {
					sscanf(words[i+1].c_str(), "%d", a+i);
					pos = words[i+1].find('/');
					if (pos == -1)	continue;
					string tex = string(words[i+1], pos+1);
					if (tex.size())	{
						hasUV = true;
						sscanf(tex.c_str(), "%d", a+16+i);
					}					
				}
				
				for(size_t j = 2; j < words.size() - 1; ++j) {
					MFace face;
					face.v[0] = a[0] - 1;
					face.v[1] = a[j - 1] - 1;
					face.v[2] = a[j] - 1;
					if (hasUV) {
						face.uv[0] = a[16] - 1;
						face.uv[1] = a[16+j-1] - 1;
						face.uv[2] = a[16+j] -1;
					}

					pmesh->totface++;
					pmesh->m_Faces.push_back(face);
				}

				hasUV = false;
				//m_bJustReadAFace = true;
				break;

			default :
				break;
		}
	}

	if (pmesh->totgroup)
		pmesh->m_Groups.back().endf = pmesh->totface - 1;
}

int CObjLoader::FindMaterial(Mesh *pmesh, string name)
{
	int idx = -1;

	for (size_t i = 0; i < pmesh->m_pMaterial.size(); i++)
		if (pmesh->m_pMaterial[i]->name == name) {
			idx = i;
			break;
		}

	return idx;
}

void CObjLoader::ReadMtlFile(Mesh *pmesh, string mtlname)
{
	int lineNum = 0;
	Material* pmaterial;

	ifstream mtl(mtlname.c_str());
	if (!mtl.is_open())
		return ;
	
	while(!mtl.eof()) {
		lineNum++;
		vector<string> words = readWords(mtl);

		if(words.size() == 0)
			continue;

		if(words[0] == string("newmtl")) {
			pmaterial = new Material();
			pmaterial->name = words[1];
			pmesh->m_pMaterial.push_back(pmaterial);

			continue;
		}
		else if(words[0] == string("Ns")) {
			sscanf(words[1].c_str(), "%f", &(pmaterial->shininess));

			continue;
		}
		else if(words[0] == string("d")) {
			float alpha;
			sscanf(words[1].c_str(), "%f", &alpha);
			pmaterial->alpha = alpha;
			pmaterial->ambient[3] = alpha;
			pmaterial->diffuse[3] = alpha;
			pmaterial->specular[3] = alpha;
			pmaterial->emission[3] = alpha;

			continue;
		}
		else if(words[0] == string("Ka")) {
			for(int i=0; i<3; i++)
				sscanf(words[i+1].c_str(), "%f", &(pmaterial->ambient[i]));

			continue;
		}
		else if(words[0] == string("Kd")) {
			for(int i=0; i<3; i++)
				sscanf(words[i+1].c_str(), "%f", &(pmaterial->diffuse[i]));

			continue;
		}
		else if(words[0] == string("Ks")) {
			for(int i=0; i<3; i++)
				sscanf(words[i+1].c_str(), "%f", &(pmaterial->specular[i]));

			continue;
		}
		else if(words[0] == string("Ke")) {
			for(int i=0; i<3; i++)
				sscanf(words[i+1].c_str(), "%f", &(pmaterial->emission[i]));

			continue;
		}
		else if(words[0] == string("map_Kd")) {
			ReadImage(pmesh->m_pMaterial.back(), words[1]);

			continue;
		}
		else	continue;
	}

	mtl.close();
}

Texturebuffer* CObjLoader::ReadJPG(string name)
{
	struct jpeg_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	Texturebuffer* tbuffer = new Texturebuffer();

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	FILE *fp = fopen(name.c_str(), "rb");
	if (fp == NULL)	{
		printf("Open file error!\n");
		return NULL;
	}
	jpeg_stdio_src(&cinfo, fp);

	jpeg_read_header(&cinfo, TRUE);
	tbuffer->buffer = new uchar[cinfo.image_width*cinfo.image_height*cinfo.num_components];

	jpeg_start_decompress(&cinfo);
	JSAMPROW row_pointer[1];
	while (cinfo.output_scanline < cinfo.output_height)
	{
		row_pointer[0] = &tbuffer->buffer[(cinfo.output_height - cinfo.output_scanline-1)*cinfo.image_width*cinfo.num_components];
		jpeg_read_scanlines(&cinfo,row_pointer ,	1);
	}
	jpeg_finish_decompress(&cinfo);

	tbuffer->width = cinfo.image_width;
	tbuffer->height = cinfo.image_height;
	tbuffer->components = cinfo.num_components;

	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	return tbuffer;
}

Texturebuffer* CObjLoader::ReadBMP(string name)
{
	//Texturebuffer* tbuffer = new Texturebuffer();

	/*int m = 0;
	uchar* pData;
	BITMAPINFOHEADER bmpInfo;
	BITMAPFILEHEADER bmpFileHead;

	FILE* fp = fopen(name.c_str(), "rb");
	if (!fp)	return NULL;

	fread(&bmpFileHead, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);

	// Error Checking!
	if (bmpFileHead.bfType != 0x4d42)
		return NULL;
	if (bmpInfo.biBitCount != 24)
		return NULL;

	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int	sizeBit = bmpInfo.biBitCount;
	int delta = sizeBit / 8;
	//int lineBytes = (width * bmpInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	int lFileSize = bmpFileHead.bfSize - bmpFileHead.bfOffBits;
	tbuffer->buffer = new uchar[width * height * 3];

	pData = new uchar[lFileSize];
	if (!pData)		return NULL;
	int f = fread(pData, sizeof(uchar), lFileSize, fp);

	for(int j = height - 1; j >= 0; j--){
		for(int i = 0; i < width; i++){
			tbuffer->buffer[(j * width + i) * 3] = pData[m + 2];
			tbuffer->buffer[(j * width + i) * 3 + 1] = pData[m + 1];
			tbuffer->buffer[(j * width + i) * 3 + 2] = pData[m];	
			m += delta;	
		}
	}
	fclose(fp);
	free(pData);
	
	tbuffer->width = width;
	tbuffer->height = height;
	tbuffer->components = 3;*/

	return NULL;
}

void CObjLoader::ReadImage(Material *pmaterial, string imagename)
{
	Texturebuffer* texbuff;
	string surfix = string(imagename, imagename.rfind('.')+1);
	if ((_stricmp(surfix.c_str(), "jpg") == 0) || 
		(_stricmp(surfix.c_str(), "jpeg") == 0)) 
	{
		
		texbuff = ReadJPG(imagename);
	}

	if (_stricmp(surfix.c_str(), "bmp") == 0) {
		texbuff = ReadBMP(imagename);
	}

	if (texbuff) {
		texbuff->type = Texturebuffer::MAP_KD;
		pmaterial->ptexbuffer = texbuff;
	}
		
	return;
}
