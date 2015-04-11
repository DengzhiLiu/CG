/**
* $Id: ObjLoader.h
*
* This file Read obj or material files, which are legal files 
* exported from 3DS MAX, the obj reader doesn't suported for all
* obj formats, but suitable for use
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#include "Mesh.h"

#include <string>
using namespace std;

#pragma once

class CObjLoader
{
public:
	CObjLoader(void);
	~CObjLoader(void);

	// Read Obj 
	bool ImportObj(Mesh *, string);
	void ReadObjFile(Mesh *, istream &);

	// Read Material
	int  FindMaterial(Mesh *, string);
	void ReadMtlFile(Mesh *, string);

	Texturebuffer* ReadJPG(string);
	Texturebuffer* ReadBMP(string);
	void ReadImage(Material *, string);
	
public:
};
