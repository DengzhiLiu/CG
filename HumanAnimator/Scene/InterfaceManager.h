/**
* $Id: InterfaceManager.h
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

#ifndef _INTERFACEMANAGER_H
#define _INTERFACEMANAGER_H

#include "view.h"
#include "DefMesh.h"
#include "SceneGL.h"

class InterfaceManager
{
public:
	InterfaceManager();
	~InterfaceManager();

	void		InitScene();
	void		SetRenderContext(int type);
	int			GetRenderContext()	{ return m_RenderType; };
	void		ShareRenderContext();

	void		LoadMesh(string file);
	DefMesh*	getDefmesh()		{ return m_pDefmesh; }

	void		StoreScene(CSceneGL* pScene);
	CSceneGL*	GetScene(int idx)	{ return m_pScenes[idx]; }
	
	void		LoadMotion(string file);
	void		LoadWeight(string file);
	void		ExportMotion(string file);

	void		Play();
	void		ReSet();

	bool		IsEnabled()			{ return m_bEnable; }
	void		SetFrameNo(int fn)  { m_iFrameNo = fn; }
	int			GetFrameNo()	    { return m_iFrameNo; }
	
private:
	int					m_iFrameNo;
	bool				m_bEnable;
	float				animation_time;
	int					m_RenderType;
	DefMesh*			m_pDefmesh;
	vector<CSceneGL*>	m_pScenes;

public:
	bool				deformed;
};

#endif //_INTERFACEMANAGER_H