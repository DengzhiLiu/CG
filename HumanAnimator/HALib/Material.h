/**
* $Id: Material.h
*
*	Texture Info
*
*	Material Info
*	
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef MATERIAL_H
#define MATERIAL_H

typedef unsigned char uchar;

//Material: Structure that defines a material in a model. 
typedef struct Texturebuffer
{
	enum	MapType { MAP_NULL = 0, MAP_KA, MAP_KD, MAP_BUMP, MAP_REFL };

	MapType type;
	int		components;		// map channel
	int		width, height;	// texture size
	uchar*	buffer;			// texture buffer

	Texturebuffer* clone() const { return new Texturebuffer(*this); }

	Texturebuffer()
	{ 
		type = MAP_NULL;
		components = 0;
		width = height = 0;
		buffer = NULL;
	}

	Texturebuffer(const Texturebuffer &tex)
	{
		type = tex.type;
		components = tex.components;
		width = tex.width;
		height = tex.height;
		buffer = new uchar[width * height * components];
		memcpy(buffer, tex.buffer, width * height * components);
	}

	~Texturebuffer() {
		if (buffer)
			delete[] buffer;
	}

} Texturebuffer;

typedef struct Material
{
	string			name;
	unsigned int	texID;			// binded texture index
	float			alpha;
	float			shininess;		// phong value
	float			ambient[4];		// ambient material
	float			diffuse[4];		// diffuse material
	float			specular[4];	// specular material
	float			emission[4];	// emission material
	Texturebuffer*	ptexbuffer;		// texture buffer

	Material* clone() { return new Material(*this); }

	Material():texID(0), ptexbuffer(NULL) {};

	Material(const Material &mtl)
	{
		name = mtl.name;
		texID = mtl.texID;
		alpha = mtl.alpha;
		shininess = mtl.shininess;
		for (int i = 0; i < 4; i++)	{
			ambient[i] = mtl.ambient[i];
			diffuse[i] = mtl.diffuse[i];
			specular[i] = mtl.specular[i];
			emission[i] = mtl.emission[i];
		}

		if (mtl.ptexbuffer)
			ptexbuffer = mtl.ptexbuffer->clone();
		else
			ptexbuffer = NULL;
	}

	~Material() { delete ptexbuffer; }

} Material;

#endif