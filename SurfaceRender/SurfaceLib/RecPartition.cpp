#include "RecPartition.h"

CRecPartition::CRecPartition(void): m_iXYZ(0), m_iMinIndex(-1), 
							m_iMaxIndex(-1), m_piIndex(NULL), m_pValue(NULL)
{
}

CRecPartition::CRecPartition(const CRecPartition &rec)
{
	m_iXYZ = rec.m_iXYZ;
	m_piIndex = new int[m_iXYZ];
	m_pValue = new ElemType[m_iXYZ];
	memmove(m_piIndex, rec.m_piIndex, sizeof(int) * m_iXYZ);
	memmove(m_pValue, rec.m_pValue, sizeof(ElemType) * m_iXYZ);
}

CRecPartition CRecPartition::operator =(const CRecPartition &rec)
{
	m_iXYZ = rec.m_iXYZ;
	m_piIndex = new int[m_iXYZ];
	m_pValue = new ElemType[m_iXYZ];
	memmove(m_piIndex, rec.m_piIndex, sizeof(int) * m_iXYZ);
	memmove(m_pValue, rec.m_pValue, sizeof(ElemType) * m_iXYZ);

	return *this;
}

CRecPartition::~CRecPartition(void)
{
	if (m_piIndex)
		delete []m_piIndex;
	m_piIndex = NULL;
	if (m_pValue)
		delete []m_pValue;
	m_pValue = NULL;
}

bool CRecPartition::ReadPartition(const char *_filename)
{
	if (NULL == _filename)
		return false;

	FILE *fp = fopen(_filename, "rb");
	if (NULL == fp)
	{
		printf("Can't Open File %s\n", _filename);
		return false;
	}

	fread(&m_iXYZ, sizeof(int), 1, fp);

	m_piIndex = new int[m_iXYZ];
	m_pValue = new ElemType[m_iXYZ];
	fread(m_piIndex, sizeof(int), m_iXYZ, fp);
	fclose(fp);

	m_iMinIndex = MAXVAL;
	m_iMaxIndex = MINVAL;
	for (int i = 0; i < m_iXYZ; i++)
	{
		if (0 == m_piIndex[i])
			continue;

		if (m_piIndex[i] < m_iMinIndex)
			m_iMinIndex = m_piIndex[i];
		if (m_piIndex[i] > m_iMaxIndex)
			m_iMaxIndex = m_piIndex[i];
	}

//#ifdef _DEBUG
	//PrintRecord();
//#endif // _DEBUG

	return true;
}

void CRecPartition::ConvertISO(int _index)
{
	memset(m_pValue, 0, sizeof(ElemType) * m_iXYZ);
	for (int i = 0; i < m_iXYZ; i++)
	{
		if (m_piIndex[i] == _index)
			m_pValue[i] = 1;
	}
}

void CRecPartition::PrintRecord()
{
	freopen("out3.txt", "w", stdout);
	printf("%d \n", m_iXYZ);
	for (int i = 0; i < m_iXYZ; i++)
	{
		printf("%d ", m_piIndex[i]);
	}
	printf("\n");
}
