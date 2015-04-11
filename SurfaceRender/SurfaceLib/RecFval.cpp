#include "RecFval.h"

CRecFval::CRecFval(void):
	m_iSurfaces(0), m_iXYZ(0)
{
	m_pSurfaces.clear();
}

CRecFval::CRecFval(const CRecFval &fval)
{
	m_iSurfaces = fval.m_iSurfaces;
	m_iXYZ = fval.m_iXYZ;
	
	for (int i = 0; i < m_iSurfaces; i++)
	{
		SURFACE *pSur = (fval.m_pSurfaces[i])->clone();
		m_pSurfaces.push_back(pSur);
	}
}

CRecFval CRecFval::operator =(const CRecFval &fval)
{
	m_iSurfaces = fval.m_iSurfaces;
	m_iXYZ = fval.m_iXYZ;

	for (int i = 0; i < m_iSurfaces; i++)
	{
		SURFACE *pSur = (fval.m_pSurfaces[i])->clone();
		m_pSurfaces.push_back(pSur);
	}

	return *this;
}

CRecFval::~CRecFval(void)
{
	size_t nSur = m_pSurfaces.size();
	if (nSur)
	{
		for (size_t i = 0; i < nSur; i++)
		{
			delete m_pSurfaces[i];
			m_pSurfaces[i] = NULL;
		}
	}
	m_pSurfaces.clear();
}

bool CRecFval::ReadFval(const char *_filename)
{
	if (NULL == _filename)
		return false;

	int i;
	FILE *fp = fopen(_filename, "rb");
	if (NULL == fp)
	{
		printf("Can't Open File %s\n", _filename);
		return false;
	}

	fread(&m_iSurfaces, sizeof(int), 1, fp);
	fread(&m_iXYZ, sizeof(int), 1, fp);

	for (i = 0; i < m_iSurfaces; i++)
	{
		SURFACE *pSur = new SURFACE(m_iXYZ);

		fread(&pSur->order, sizeof(int), 1, fp);
		fread(&pSur->type, sizeof(int), 1, fp);
		fread(&pSur->isoVal, sizeof(ElemType), 1, fp);
		fread(pSur->pFval, sizeof(ElemType), m_iXYZ, fp);
		m_pSurfaces.push_back(pSur);
	}
	fclose(fp);

//#ifdef _DEBUG
//	PrintRecord();
//#endif // _DEBUG

	return true;
}

void CRecFval::PrintRecord()
{
	freopen("out2.txt", "w", stdout);
	printf("%d %d\n", m_iSurfaces, m_iXYZ);
	int i, j;
	for (i = 0; i < m_iSurfaces; i++)
	{
		printf("%d %d %f\n", m_pSurfaces[i]->order, m_pSurfaces[i]->type, m_pSurfaces[i]->isoVal);
		ElemType *pVal = m_pSurfaces[i]->pFval;
		for (j = 0; j < m_iXYZ; j++)
			printf("%f ", pVal[j]);
		printf("\n");
	}
}