/*  This file is part of the Pinocchio automatic rigging library.
    Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "pinocchioApi.h"
#include "debugging.h"
#include <fstream>

ostream *Debugging::outStream = new ofstream();

PinocchioOutput autorig(const Skeleton &given, const Mesh &m)
{
    int i;
    PinocchioOutput out;

	// ��newMesh�ݴ�Mesh m
    Mesh newMesh = prepareMesh(m);

    if(newMesh.vertices.size() == 0)
        return out;

    TreeType *distanceField = constructDistanceField(newMesh);

    //discretization
    vector<Sphere> medialSurface = sampleMedialSurface(distanceField);
	out.mediaSurface = medialSurface;

	// ���������棬��һ�������������������뾶�Ƚ�С�����޳���(ֻ�Ǽ򵥵Ĵ���)
    vector<Sphere> spheres = packSpheres(medialSurface);
	out.spheres = spheres;

    PtGraph graph = connectSamples(distanceField, spheres);
	out.ptGraph = graph;

    //discrete embedding
    vector<vector<int> > possibilities = computePossibilities(graph, spheres, given, out.limbVerts);

    //constraints can be set by respecifying possibilities for skeleton joints:
    //to constrain joint i to sphere j, use: possiblities[i] = vector<int>(1, j);

    vector<int> embeddingIndices = discreteEmbed(graph, spheres, given, possibilities);
	out.embeddingIdx = embeddingIndices; 

    if(embeddingIndices.size() == 0) { //failure
        delete distanceField;
        return out;
    }

	// discreteEmbedding�д洢����skeleton�е�ÿ������ƥ���possibities�еĶ�����PtGraph�е�����
    vector<Vector3> discreteEmbedding = splitPaths(embeddingIndices, graph, given);
	out.discreteEmbedding = discreteEmbedding;

    //continuous refinement
    vector<Vector3> medialCenters(medialSurface.size());
    for(i = 0; i < (int)medialSurface.size(); ++i)
        medialCenters[i] = medialSurface[i].center;

	// ���ʱ��discreteEmbedding�洢����ÿ�����������
    out.embedding = refineEmbedding(distanceField, medialCenters, discreteEmbedding, given);

    //attachment
    VisTester<TreeType> *tester = new VisTester<TreeType>(distanceField);
    out.attachment = new Attachment(newMesh, given, out.embedding, tester);
	//out.attachment.a.AttachmentPrivate1�е�weights������ÿ��bones�Զ����Ȩ��ֵ

    //cleanup
    delete tester;
    delete distanceField;

    return out;
}

