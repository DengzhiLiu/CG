/**
* $Id: TextureArray.h
*
*
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef TEXTUREARRAY_H
#define TEXTUREARRAY_H

#include "../HALib/Mesh.h"

#pragma once

class CTextureArray
{
public:
	CTextureArray(void);
	~CTextureArray(void);

public:
	void MakeRGBATexture(const Texturebuffer* pTexture, bool mipmap = false);

	void MakeRGBTexture(const Texturebuffer* pTexture, bool mipmap = false);
};

#endif