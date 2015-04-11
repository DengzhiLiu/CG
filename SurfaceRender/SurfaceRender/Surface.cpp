#include "stdafx.h"
#include "Surface.h"
#include <time.h>

CSurface::CSurface(void)
{
	m_pSurfRender.clear();
}

CSurface::~CSurface(void)
{
	RenderData* pRender;
	size_t	nRender = m_pSurfRender.size();
	for (size_t i = 0; i < nRender; i++)
	{
		pRender = m_pSurfRender[i];
		delete pRender;
		pRender = NULL;
		//deleteVBO(&posVbo);
		//deleteVBO(&normalVbo);
	}
	m_pSurfRender.clear();
}

void CSurface::ImportCoordData(const char *_filename)
{
	m_RecCoord.ReadCoord(_filename);
}

void CSurface::ImportFvalData(const char *_filename)
{
	m_RecFval.ReadFval(_filename);
}

void CSurface::ImportPartData(const char *_filename)
{
	m_RecPart.ReadPartition(_filename);
}

void CSurface::ComputeSurface(int _case)
{
	if (NULL == m_RecCoord.GetCoords())
	{
		AfxMessageBox("Please Import case_coord.dat");
		return;
	}

	m_CubeTool.InitMC(m_RecCoord.GetWidth(), m_RecCoord.GetHeight(), m_RecCoord.GetDepth(), \
		m_RecCoord.GetCoords());

	if (!_case)
	{
		SURFACE* pSurf;
		int nSur = m_RecFval.GetSurfaceNum();

		if (0 == nSur)
		{
			AfxMessageBox("Test 1!\nPlease Import case_fval.dat");
			return;
		}

		for (int i = 0; i < nSur; i++)
		{
			RenderData* pRender = new RenderData();
			pSurf = m_RecFval.GetSurface(i);
			m_CubeTool.ComputeIsosurface(pSurf->pFval, pSurf->isoVal, pRender);
			m_pSurfRender.push_back(pRender);
		}
	}
	else
	{
		int nMinIndex = m_RecPart.GetMinIndex();
		int nMaxIndex = m_RecPart.GetMaxIndex();

		if ((-1 == nMinIndex) && (-1 == nMaxIndex))
		{
			AfxMessageBox("Test 2!\nPlease Import case_partition.dat");
			return;
		}

		for (int i = nMinIndex; i < nMaxIndex; i++ )
		{
			m_RecPart.ConvertISO(i);
			RenderData* pRender = new RenderData();
			m_CubeTool.ComputeIsosurface(m_RecPart.GetValue(), 0.5, pRender);
			m_pSurfRender.push_back(pRender);
		}
	}

}

void CSurface::RenderCoords()
{
	int nx = m_RecCoord.m_iX;
	int ny = m_RecCoord.m_iY;
	int nz = m_RecCoord.m_iZ;
	const ElemType *pCoords = m_RecCoord.GetCoords();

	if (NULL == pCoords)	return;

	glPushMatrix();
	int i, j, k, index;
	glColor3f(1.f, 1.f, 1.f);
	//glPointSize(3.f);
	glBegin(GL_POINTS);
	for (i = 0; i < nx; i++)
		for (j = 0; j < ny; j++)
			for (k = 0; k < nz; k++)
			{
				index = i * ny * nz + j * nz + k;
				glVertex3dv(pCoords + index*3);
			}
	glEnd();

	glPopMatrix();
}

void CSurface::RenderSurfaces()
{
	RenderData* pRender;
	int numSurface = m_RecFval.GetSurfaceNum();

	size_t nRender = m_pSurfRender.size();
	for (size_t i = 0; i < nRender; i++)
	{
		pRender = m_pSurfRender[i];
		if (!pRender->selected)
			continue;

		glBindBuffer(GL_ARRAY_BUFFER, pRender->posVbo);
		glVertexPointer(4, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, pRender->normalVbo);
		glNormalPointer(GL_FLOAT, sizeof(float)*4, 0);
		glEnableClientState(GL_NORMAL_ARRAY);

		//glColor3fv(colorTable[i]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorTable[i]);
		glDrawArrays(GL_TRIANGLES, 0, pRender->totalVerts);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

