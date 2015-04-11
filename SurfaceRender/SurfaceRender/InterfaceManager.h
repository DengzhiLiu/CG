#ifndef _INTERFACE_MANAGER_H
#define _INTERFACE_MANAGER_H

#pragma once
#include "SceneGL.h"
#include "Surface.h"

class CInterfaceManager
{
public:
	enum	TEST{ TEST1 = 0, TEST2};

	CInterfaceManager(void);
	~CInterfaceManager(void);

	void	SetTest(TEST _case);

	void	SetCurScene(CSceneGL *pScene);

	CSceneGL* GetScene() const { return m_pScene; }

	void	Process();

	CSurface*	GetSurface() { return m_pSurface; }

public:
	CSceneGL*	m_pScene;
	CSurface*	m_pSurface;
	TEST		m_TEST;
};

#endif
