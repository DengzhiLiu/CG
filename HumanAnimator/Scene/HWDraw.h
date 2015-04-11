/**
* $Id: HWDraw.h
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

#ifndef _HWDRAW_H
#define _HWDRAW_H

#include "GLDraw.h"

#include <gl/glew.h>

class HWDraw : public GLW
{
public:
	HWDraw();
	~HWDraw();

	void	InitShader();
	int		CountSize(const char* fileName);
	char*	ReadShaderFile(const char* filename, int size);
	GLuint	AddShader(GLuint programObject, const GLchar *shaderSource, GLenum shaderType);
	GLuint	GetShaderProgram() { return ProgramObject; }

	void	Reset();
	void	SetPlay(bool flag) { m_bPlay = flag; }
	void	SetShadow(bool flag) { m_bShadow = flag; }
	void	SetUniParam(const GLchar* UnifromName, int value);

	void	SetVSBindMat();
	void	SetVSMotionMat(int frame);

	virtual void DrawMesh();
	virtual void DrawSkel();

	virtual void DrawFill();		//face
	virtual void DrawPoints();		//point
	virtual void DrawWire();		//line
	virtual void DrawHidden();		//cull face
	virtual void DrawFillWire();	//wire

private:
	bool   m_bPlay;
	bool   m_bShadow;
	GLuint ProgramObject;

	GLuint vertexshaderObj;
	GLuint fragShaderObj;

	GLint  normalAttrib;
	GLint  positionAttrib;
	GLint  indexAttrib;
	GLint  weightAttrib;
	GLint  texcoordAttrib;
	GLint  texSampler;

	GLint  playParam;
	GLint  mtlParam;
	GLint  shadowParam;
	GLint  scaleParam;
	GLint  addParam;
	GLint  rootParam;
	GLint  refMatriceParam;
	GLint  motionMatriceParam;
};

#endif
