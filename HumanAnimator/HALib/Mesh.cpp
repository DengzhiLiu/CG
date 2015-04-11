#include "Mesh.h"
#include "../Common/utils.h"
#include "../Common/utildefines.h"

Mesh::Mesh()
{
	scale = 1.;
	totvert = 0;
	totcoord = 0;
	totface = 0;
	totgroup = 0;

	m_Vertices.clear();
	m_Faces.clear();
	m_Coords.clear();
	m_Groups.clear();
	m_pMaterial.clear();
}

Mesh::Mesh(const Mesh &mesh)
{
	ModelFileName = mesh.ModelFileName;
	MtlFileName = mesh.MtlFileName;

	scale = mesh.scale;
	totvert = mesh.totvert;
	totcoord = mesh.totcoord;
	totface = mesh.totface;
	totgroup = mesh.totgroup;

	toScale = mesh.toScale;
	toAdd = mesh.toAdd;

	minbound = mesh.minbound;
	maxbound = mesh.maxbound;

	m_Vertices = mesh.m_Vertices;
	m_Faces = mesh.m_Faces;
	m_Coords = mesh.m_Coords;
	m_Groups = mesh.m_Groups;
	for (size_t i = 0; i < mesh.m_pMaterial.size(); i++) {
		Material* pmtl = mesh.m_pMaterial[i]->clone();
		m_pMaterial.push_back(pmtl);
	}
}

void Mesh::normalizeBoundBox()
{
	int v;
	float minpos[3], maxpos[3];
	INIT_MINMAX(minpos, maxpos);

	for (int i = 0; i < totface; i++)
		for (int j = 0 ; j < 3; j++) {
			v = m_Faces[i].v[j];
			DO_MINMAX(&(m_Vertices[v].coord), minpos, maxpos);
		}

	minbound = Vector3(minpos[0], minpos[1], minpos[2]);
	maxbound = Vector3(maxpos[0], maxpos[1], maxpos[2]);
	scale = 2. / (maxbound - minbound).length();

	Vector3 center = (minbound + maxbound) / 2;
	Vector3 foot = Vector3(center[0], minpos[1], center[2]);
	toAdd = -foot * scale;
}

void Mesh::computeVertexNormals()
{
	int i;
	for(i = 0; i < totvert; ++i)
		m_Vertices[i].normal = Vector3();
	for(i = 0; i < totface; i += 1) {
		int i1 = m_Faces[i].v[0];
		int i2 = m_Faces[i].v[1];
		int i3 = m_Faces[i].v[2];
		Vector3 normal = ((m_Vertices[i2].coord - m_Vertices[i1].coord) % (m_Vertices[i3].coord - m_Vertices[i1].coord)).normalize();
		m_Faces[i].normal = normal;
		m_Vertices[i1].normal += normal;
		m_Vertices[i2].normal += normal;
		m_Vertices[i3].normal += normal;
	}
	for(i = 0; i < totvert; ++i)
		m_Vertices[i].normal = m_Vertices[i].normal.normalize();
}

Mesh::~Mesh(void)
{
	m_Vertices.clear();
	m_Faces.clear();
	m_Coords.clear();
	m_Groups.clear();
	for (size_t i = 0; i < m_pMaterial.size(); i++) {
		if (m_pMaterial[i])
			delete m_pMaterial[i];
	}
	m_pMaterial.clear();
}
