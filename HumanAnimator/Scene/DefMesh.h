/**
* $Id: DefMesh.h
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

#ifndef _DEFMESH_H
#define _DEFMESH_H

#pragma warning(disable: 4244)

#define  CPURENDER 0
#define  GPURENDER 1

#include "GLDraw.h"
#include "HWDraw.h"
#include "../HALib/Mesh.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

class DefMesh
{
public:
	DefMesh();
	~DefMesh();

	Mesh	GetMesh() { return curMesh; }
	Mesh&	GetOrigMesh() { return mesh; }

	Bone*		getBoneByID(int id);
	BoneVector	getBoneByName(string name);
	int			getBoneNum() { return (int)m_pBones.size(); }

	void	ComputeBindPose(Joint *);
	void	ComputeMotion();
	void	ComputeMotion(Joint *pjoint, int frame_no);
	void    InsertMotion(int frame);
	
	bool	ImportMesh(string fileName);
	bool	ImportMotion(string fileName);
	bool	ImportWeight(string fileName);

	void	OutputJoint(Joint* pjoint, int depth, FILE* fp);	
	bool	SaveMotion(string fileName);

	void	Reset();
	void	DeformMesh(int frame, int renderType);
	void	RenderMesh(GLW::DrawMode dm, GLW::ShadeMode sm, bool texture, bool shadow, int renderType);

	void	RenderSkeleton(GLW::DrawMode dm);
	void	UpdateSkeleton(int frame);
	void	BuildSkelList(bool sphere) { glw.SetSkelMode(sphere); glw.BuildSkelList(); }

	Clip*	GetClip()  { return m_pClip; }
	int		GetFrameNum() { return m_pClip->getNumFrames(); }

private:
	Mesh				mesh;
	GLDraw				glw;
	HWDraw				hwd;
	mutable Mesh		curMesh;
	Clip*				m_pClip;
	map<int, Bone *>	m_pBones;
	
public:
	Vector3			HipPos;
	Vector3			HipRad;
	Vector3			toAdd;
};
#endif	//_DEFMESH_H