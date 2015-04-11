#include "stdafx.h"
#include "InterfaceManager.h"

#include <string>
#include <assert.h>
using namespace std;

InterfaceManager::InterfaceManager()
{
	m_pDefmesh		 = NULL;
	m_iFrameNo		 = 0;
	animation_time	 = 0.;
	m_RenderType	 = CPURENDER;
	m_bEnable		 = false;
	deformed		 = false;
}

InterfaceManager::~InterfaceManager()
{
	if (m_pDefmesh)
		delete m_pDefmesh;
	//for (size_t i = 0; i < m_pScenes.size(); i++)
	//	if (m_pScenes[i])	delete	m_pScenes[i];
}

void InterfaceManager::StoreScene(CSceneGL* pScene)
{
	m_pScenes.push_back(pScene);

	return ;
}

void InterfaceManager::SetRenderContext(int type)
{
	m_RenderType = type;
}

void InterfaceManager::ShareRenderContext()
{
	HGLRC curhg = ::wglGetCurrentContext();
	HGLRC hg[2];
	hg[0] = GetScene(0)->m_hRC;
	hg[1] = GetScene(1)->m_hRC;
	if (hg[0] == curhg)
		wglShareLists(curhg, hg[1]);
	else
		wglShareLists(curhg, hg[0]);
}


void InterfaceManager::LoadMesh(string file)
{
	if (m_pDefmesh)
		delete m_pDefmesh;
	m_pDefmesh = new DefMesh();
	m_pDefmesh->ImportMesh(file);
	InitScene();

	ShareRenderContext();
	m_pScenes[0]->PaintGL();
	m_pScenes[1]->PaintGL();

	return ;
}

void InterfaceManager::InitScene()
{
	//Trackball *tb;
	Mesh mesh = m_pDefmesh->GetMesh();
	Vector3 minpos = mesh.getMinBound();
	Vector3 maxpos = mesh.getMaxBound();
	Vector3 center = (minpos + maxpos) * mesh.scale / 2.;
	for (size_t i = 0; i < m_pScenes.size(); i++)	{
		m_pScenes[i]->m_pcam->SetEye(center + Vector3(0, 0, 2));
		m_pScenes[i]->m_pcam->SetCenter(center);
	}
}

void InterfaceManager::LoadMotion(string file)
{
	if ( !m_pDefmesh )	return ;

	m_pDefmesh->ImportMotion(file);
	return ;
}

void InterfaceManager::LoadWeight(string file)
{
	if ( !m_pDefmesh )	return;

	m_pDefmesh->ImportWeight(file);
	m_bEnable = true;
	return ;
}

void InterfaceManager::ExportMotion(string file)
{
	if ( !m_pDefmesh || !m_pDefmesh->GetClip())
		return;

	m_pDefmesh->SaveMotion(file);
	return ;
}

void InterfaceManager::Play()
{
	if ( !m_bEnable )	return;

	int frame_num = m_pDefmesh->GetFrameNum();
	if (frame_num == 0)	return;
	
//#ifdef  WIN32
//	static DWORD  last_time = 0;
//	DWORD  curr_time = timeGetTime();
//	float  delta = ( curr_time - last_time ) * 0.001f;
//	if ( delta > 0.03f )
//		delta = 0.03f;
//	last_time = curr_time;
//	animation_time += delta;
//#else
//	animation_time += 0.03f;
//#endif
//	m_iFrameNo = animation_time / m_Clip.getInterval();
//	m_iFrameNo = m_iFrameNo % m_Clip.getNumFrames();

	deformed = true;
	startTime = clock();

	assert(m_iFrameNo >= 0 && m_iFrameNo < frame_num);
	m_pDefmesh->DeformMesh(m_iFrameNo, m_RenderType);

	m_iFrameNo = (++m_iFrameNo) % frame_num;
}

void InterfaceManager::ReSet()
{
	if ( !m_pDefmesh )	return;
	
	m_iFrameNo = 0;
	m_pDefmesh->Reset();
	m_pScenes[0]->PaintGL();
	m_pScenes[1]->PaintGL();
}