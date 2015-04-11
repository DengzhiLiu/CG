#include "StdAfx.h"
#include "Hide.h"

CHide::CHide(void)
{
}

CHide	g_Hide;

CHide::~CHide(void)
{
}

void	CHide::OpenGL_Render(t3DModel *p3DModel)
{
	// 遍历模型中所有的对象
	for(int i = 0; i < p3DModel->numOfObjects; i++)
	{
		// 获得当前显示的对象
		t3DObject *pObject = &(p3DModel->pObject[i]);
		if (pObject == NULL)	break;

		glColor3ub(255, 255, 255);			
		glBegin(GL_TRIANGLES);					
		// 遍历所有的面
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			// 遍历三角形的所有点
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				// 获得面对每个点的索引
				int index = pObject->pFaces[j].vertIndex[whichVertex];

				// 给出法向量
				glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
				glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
			}
		}

		glEnd();								// 绘制结束
	}
	glFinish();
	::SwapBuffers(wglGetCurrentDC());
}

void	CHide::Draw(int light, int color, int render, t3DModel *p3DModel)
{
	if (p3DModel == NULL)
		return;
	light_type = light;
	color_type = color;

	switch (render)
	{
	case	OPENGL_RENDER:
		OpenGL_Render(p3DModel);	break;
	case	PROGRAM_RENDER:
		{
			CScanZBuffer m_scaner;
			m_scaner.Transform(p3DModel, light_type, color_type);
		}
	default:
		break;
	}
}
