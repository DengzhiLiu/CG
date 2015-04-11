#include "RecCoord.h"

CRecCoord::CRecCoord(void):
		m_iX(0), m_iY(0), m_iZ(0)
{
	m_pCoords = NULL;
}

CRecCoord::CRecCoord(const CRecCoord& coord)
{
	m_iX = coord.m_iX;
	m_iY = coord.m_iY;
	m_iZ = coord.m_iZ;

	int num = m_iX * m_iY * m_iZ * 3;
	m_pCoords = new ElemType[num];
	memmove(m_pCoords, coord.GetCoords(), sizeof(ElemType) * num);

	m_dScale = coord.m_dScale;
	m_Center = coord.m_Center;
	m_MinBound = coord.m_MinBound;
	m_MaxBound = coord.m_MaxBound;
}

CRecCoord::~CRecCoord(void)
{
	if (m_pCoords)
		delete []m_pCoords;
	m_pCoords = NULL;
}

CRecCoord CRecCoord::operator =(const CRecCoord& coord)
{
	m_iX = coord.m_iX;
	m_iY = coord.m_iY;
	m_iZ = coord.m_iZ;

	int num = m_iX * m_iY * m_iZ * 3;
	m_pCoords = new ElemType[num];
	memmove(m_pCoords, coord.GetCoords(), sizeof(ElemType) * num);

	m_dScale = coord.m_dScale;
	m_Center = coord.m_Center;
	m_MinBound = coord.m_MinBound;
	m_MaxBound = coord.m_MaxBound;

	return *this;
}

bool CRecCoord::ReadCoord(const char *_filename)
{
	if (NULL == _filename)
		return false;
	
	FILE *fp = fopen(_filename, "rb");
	//FILE *fp = fopen(_filename, "r");
	if (NULL == fp)
	{
		printf("Can't Open File %s\n", _filename);
		return false;
	}

	// Read Number of points
	fread(&m_iX, sizeof(int), 1, fp);
	fread(&m_iY, sizeof(int), 1, fp);
	fread(&m_iZ, sizeof(int), 1, fp);

	int iGrids = m_iX * m_iY * m_iZ;
	m_pCoords = new ElemType[iGrids * 3];
	fread(m_pCoords, sizeof(ElemType), iGrids * 3, fp);
	fclose(fp);

	ComputeBoundBox();

	//PrintCube();

//#ifdef _DEBUG
//	PrintRecord();
//#endif // _DEBUG

	return true;
}

void CRecCoord::PrintCube()
{
	int i = 0, j = 0, k = 0;

	int index[8];
	// (0, 0, 0)
	index[0] = k * m_iX * m_iY + j * m_iX + i;
	// (1, 0, 0)
	index[1] = k * m_iX * m_iY + j * m_iX + (i + 1);
	// (1, 1, 0)
	index[2] = k * m_iX * m_iY + (j + 1) * m_iX + (i + 1);
	// (0, 1, 0)
	index[3] = k * m_iX * m_iY + (j + 1) * m_iX + i;
	// (0, 0, 1)
	index[4] = (k + 1) * m_iX * m_iY + j * m_iY + i;
	// (1, 0, 1)
	index[5] = (k + 1) * m_iX * m_iY + j * m_iY + (i + 1);
	// (1, 1, 1)
	index[6] = (k + 1) * m_iX * m_iY + (j + 1) * m_iY + (i + 1);
	// (0, 1, 1)
	index[7] = k * m_iX * m_iY + (j + 1) * m_iY + (i + 1);

	printf("Coord %f %f %f\n", m_pCoords[index[0] * 3], m_pCoords[index[0] * 3 + 1], m_pCoords[index[0] * 3 + 2]);
	printf("Coord %f %f %f\n", m_pCoords[index[1] * 3], m_pCoords[index[1] * 3 + 1], m_pCoords[index[1] * 3 + 2]);
	printf("Coord %f %f %f\n", m_pCoords[index[2] * 3], m_pCoords[index[2] * 3 + 1], m_pCoords[index[2] * 3 + 2]);
	printf("Coord %f %f %f\n", m_pCoords[index[3] * 3], m_pCoords[index[3] * 3 + 1], m_pCoords[index[3] * 3 + 2]);
	printf("Coord %f %f %f\n", m_pCoords[index[4] * 3], m_pCoords[index[4] * 3 + 1], m_pCoords[index[4] * 3 + 2]);
	printf("Coord %f %f %f\n", m_pCoords[index[5] * 3], m_pCoords[index[5] * 3 + 1], m_pCoords[index[5] * 3 + 2]);
	printf("Coord %f %f %f\n", m_pCoords[index[6] * 3], m_pCoords[index[6] * 3 + 1], m_pCoords[index[6] * 3 + 2]);
	printf("Coord %f %f %f\n", m_pCoords[index[7] * 3], m_pCoords[index[7] * 3 + 1], m_pCoords[index[7] * 3 + 2]);
	
}

void CRecCoord::ComputeBoundBox()
{
	int iGrids = m_iX * m_iY * m_iZ;
	int start, end = iGrids;
	if (iGrids % 2 == 1)
	{
		m_MinBound[0] = m_MaxBound[0] = m_pCoords[0];
		m_MinBound[1] = m_MaxBound[1] = m_pCoords[1];
		m_MinBound[2] = m_MaxBound[2] = m_pCoords[2];
		start = 1;
	}
	else
	{
		m_MinBound[0] = m_MinBound[1] = m_MinBound[2] = MAXVAL;
		m_MaxBound[0] = m_MaxBound[1] = m_MaxBound[2] = MINVAL;
		start = 0;
	}

	ElemType minval[3], maxval[3];
	for (int i = start; i < end; i += 2)
	{
		if (m_pCoords[i*3] < m_pCoords[(i+1)*3])
		{
			minval[0] = m_pCoords[i*3];
			maxval[0] = m_pCoords[(i+1)*3];
		}
		else
		{
			minval[0] = m_pCoords[(i+1)*3];
			maxval[0] = m_pCoords[i*3];
		}

		if (m_pCoords[i*3+1] < m_pCoords[(i+1)*3+1])
		{
			minval[1] = m_pCoords[i*3+1];
			maxval[1] = m_pCoords[(i+1)*3+1];
		}
		else
		{
			minval[1] = m_pCoords[(i+1)*3+1];
			maxval[1] = m_pCoords[i*3+1];
		}

		if (m_pCoords[i*3+2] < m_pCoords[(i+1)*3+2])
		{
			minval[2] = m_pCoords[i*3+2];
			maxval[2] = m_pCoords[(i+1)*3+2];
		}
		else
		{
			minval[2] = m_pCoords[(i+1)*3+2];
			maxval[2] = m_pCoords[i*3+2];
		}

		m_MinBound[0] = m_MinBound[0] < minval[0] ? m_MinBound[0] : minval[0];
		m_MinBound[1] = m_MinBound[1] < minval[1] ? m_MinBound[1] : minval[1];
		m_MinBound[2] = m_MinBound[2] < minval[2] ? m_MinBound[2] : minval[2];
		m_MaxBound[0] = m_MaxBound[0] > maxval[0] ? m_MaxBound[0] : maxval[0];
		m_MaxBound[1] = m_MaxBound[1] > maxval[1] ? m_MaxBound[1] : maxval[1];
		m_MaxBound[2] = m_MaxBound[2] > maxval[2] ? m_MaxBound[2] : maxval[2];
	}

	m_Center = (m_MinBound + m_MaxBound) / 2;
	m_dScale = 2. / (m_MaxBound - m_MinBound).Length();

	// Normalize the BoundBox
	for (int i = 0; i < iGrids; i++)
	{
		m_pCoords[i*3] = (m_pCoords[i*3] - m_Center[0]) * m_dScale;
		m_pCoords[i*3+1] = (m_pCoords[i*3+1] - m_Center[1]) * m_dScale;
		m_pCoords[i*3+2] = (m_pCoords[i*3+2] - m_Center[2]) * m_dScale * 50;
	}

	m_MinBound = (m_MinBound - m_Center) * m_dScale;
	m_MaxBound = (m_MaxBound - m_Center) * m_dScale;
	m_Center = Vector3(0, 0, 0);
}

// Output the record
void CRecCoord::PrintRecord()
{
	int i, j, k, index;

	freopen("out.txt", "w", stdout);
	printf("%d %d %d\n", m_iX, m_iY, m_iZ);
	printf("Center: %f %f %f\n", m_Center[0], m_Center[1], m_Center[2]);
	printf("MiniBound: %f %f %f\n", m_MinBound[0], m_MinBound[1], m_MinBound[2]);
	printf("MaxBound: %f %f %f\n", m_MaxBound[0], m_MaxBound[1], m_MaxBound[2]);

	// Binary file format Vertex[:, :, 1], Vertex[:, :, 2], ......, Vertex[:, :, 50]
	// for each slice of depth, row first
	for (j = 0; j < m_iY; j++)
		for (i = 0; i < m_iX; i++)
			for (k = 0; k < m_iZ; k++)
			{
				//index = i * m_iY * m_iZ + j * m_iZ + k;
				index = k * m_iX * m_iY + j * m_iX + i;
				printf("%f %f %f\n", m_pCoords[index*3], m_pCoords[index*3+1], m_pCoords[index*3+2]);
			}

}