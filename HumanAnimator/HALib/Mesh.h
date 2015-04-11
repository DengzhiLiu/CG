/**
* $Id: Mesh.h
*
*	Vertex Info
*		
*	Face Info
*
*	Vertex Group
*	
*	Mesh Info
*		Mesh Name
*		Vertex Vector
*		Face Vector
*		TextureCoord Vector
*		Material Vector
*		Group Vector
*		Bound Box
*		others
*	
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef MESH_H
#define MESH_H

#pragma once
#include "../Common/vector.h"
#include "Clip.h"
#include "Material.h"

#include <vector>
#include <map>
using namespace std;

typedef struct MVert
{
	Vector3 coord;			// coordinate 
	Vector3 normal;			// normal vector
	float	bone[4];		// weight table
	float	weight[4];
	//map<int, float> weights;
} MVert;

typedef struct MFace {
	int v[3];				// vertex index
	int uv[3];				// coordinate index
	Vector3 normal;			// face normal
} MFace;

typedef struct Group {					// devided by material
	//string		GName;
	int			mtlID;
	int			startf, endf;
	bool		bTexture;
	Group() {mtlID = -1; startf = endf = 0; bTexture = false;}
} Group;

class Mesh
{
public:
	Mesh(void);
	Mesh(const Mesh &mesh);
	~Mesh(void);

	Mesh& operator = (const Mesh mesh)
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

		return *this;
	}

public:
	vector<MVert>	getVerts() { return m_Vertices; }
	vector<MFace>	getFaces() { return m_Faces; }
	vector<Vector3> getCoords() { return m_Coords; }
	vector<Group>	getGroups() { return m_Groups; }

	void	normalizeBoundBox();
	void	computeVertexNormals();

	Vector3 getMinBound() { return minbound; }
	Vector3 getMaxBound() { return maxbound; }

	void	setVerts(vector<MVert > verts) { m_Vertices = verts; }

public:
	string		ModelFileName;
	string		MtlFileName;

	int			totvert;
	int			totcoord;
	int			totface;
	int			totgroup;
	float		scale;
	Vector3		minbound, maxbound;
	float		toScale;
	Vector3		toAdd;

	// Vertices, Faces, UVCoord, Materials
	vector<MVert >		m_Vertices;
	vector<MFace >		m_Faces;
	vector<Vector3>		m_Coords;
	vector<Group>		m_Groups;
	vector<Material *>	m_pMaterial;
};

#endif
