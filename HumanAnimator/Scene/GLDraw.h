/**
* $Id: GLDraw.h
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

#ifndef _GLDRAW_H
#define _GLDRAW_H

#include "../HALib/Joint.h"
class DefMesh;

class GLW
{
public:
	enum DrawMode	{ DMPoints, DMWire, DMHidden, DMFill, DMFillWire };
	enum ShadeMode  { SMFlat , SMSmooth };

	virtual void DrawMesh() = 0;
	virtual void DrawSkel() = 0;

	virtual void DrawFill()		= 0;		//face
	virtual void DrawPoints()	= 0;		//point
	virtual void DrawWire()		= 0;		//line
	virtual void DrawHidden()	= 0;		//cull face
	virtual void DrawFillWire()	= 0;		//wire

	void setTexture(bool flag) { m_bTextureOn = flag; }
	void setDrawMode(DrawMode drawMode) { m_DrawMode = drawMode; }
	void setShadeMode(ShadeMode shadeMode) { m_ShadeMode = shadeMode; }

public:
	DefMesh*  m_pDefmesh;

public:
	bool	  m_bTextureOn;
	DrawMode  m_DrawMode; 
	ShadeMode m_ShadeMode;
};

class GLDraw: public GLW
{
public:

	GLDraw();
	~GLDraw();

	void BuildList();
	void BindTexture();

	void BuildSkelList();
	void RenderBone(Joint* root);
	void SetSkelMode(bool sphere);
	void DrawBone(Vector3 head, Vector3 tail);

	virtual void DrawMesh();
	virtual void DrawSkel();

	virtual void DrawFill();		//face
	virtual void DrawPoints();		//point
	virtual void DrawWire();		//line
	virtual void DrawHidden();		//cull face
	virtual void DrawFillWire();	//wire

private:
	bool m_SkelMode;

	int m_FaceDisplayList;     //smooth face
	int m_FaceFlatDisplayList; //flat face
	int m_BoneDisplayList;	   //bone list
};

#endif