#include "stdafx.h"
#include "InterfaceManager.h"

CInterfaceManager::CInterfaceManager(void): m_pScene(NULL)
{
	m_TEST = TEST1;
	m_pSurface = new CSurface();
}

CInterfaceManager::~CInterfaceManager(void)
{
	if (m_pSurface)
		delete m_pSurface;
	m_pSurface = NULL;
}

void CInterfaceManager::SetCurScene(CSceneGL *pScene)
{
	m_pScene = pScene;
}

void CInterfaceManager::SetTest(TEST _case)
{
	m_TEST = _case;
}

void CInterfaceManager::Process()
{
	m_pSurface->ComputeSurface(m_TEST);
	m_pScene->PaintGL();
}
