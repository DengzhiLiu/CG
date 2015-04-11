#include "stdafx.h"
#include "TextureArray.h"

#include <gl/glew.h>
#include <gl/glu.h>

GLenum array_texture_target = GL_TEXTURE_2D;

CTextureArray::CTextureArray(void)
{
}

CTextureArray::~CTextureArray(void)
{
}

void CTextureArray::MakeRGBATexture(const Texturebuffer* pTexture, bool mipmap /* = false */)
{
	glTexParameteri(array_texture_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(array_texture_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(array_texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(array_texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (mipmap)
		gluBuild2DMipmaps(array_texture_target, GL_RGBA8, pTexture->width, pTexture->height, GL_RGBA,
			GL_UNSIGNED_BYTE, pTexture->buffer);
	else
		glTexImage2D(array_texture_target, 0, GL_RGBA, pTexture->width, pTexture->height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, pTexture->buffer);
}

void CTextureArray::MakeRGBTexture(const Texturebuffer* pTexture, bool mipmap /* = false */)
{
	glTexParameteri(array_texture_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(array_texture_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(array_texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(array_texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (mipmap)
		gluBuild2DMipmaps(array_texture_target, GL_RGB8, pTexture->width, pTexture->height, GL_RGB,
		GL_UNSIGNED_BYTE, pTexture->buffer);
	else
		glTexImage2D(array_texture_target, 0, GL_RGB, pTexture->width, pTexture->height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pTexture->buffer);
}