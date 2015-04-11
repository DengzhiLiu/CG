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

	// 用newMesh暂存Mesh m
    Mesh newMesh = prepareMesh(m);

    if(newMesh.vertices.size() == 0)
        return out;

    TreeType *distanceField = constructDistanceField(newMesh);

    //discretization
    vector<Sphere> medialSurface = sampleMedialSurface(distanceField);
	out.mediaSurface = medialSurface;

	// 构造球曲面，将一个球中所包含的其它半径比较小的球剔除掉(只是简单的处理)
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

	// discreteEmbedding中存储的是skeleton中的每个顶点匹配的possibities中的顶点在PtGraph中的索引
    vector<Vector3> discreteEmbedding = splitPaths(embeddingIndices, graph, given);
	out.discreteEmbedding = discreteEmbedding;

    //continuous refinement
    vector<Vector3> medialCenters(medialSurface.size());
    for(i = 0; i < (int)medialSurface.size(); ++i)
        medialCenters[i] = medialSurface[i].center;

	// 这个时候discreteEmbedding存储的是每个顶点的坐标
    out.embedding = refineEmbedding(distanceField, medialCenters, discreteEmbedding, given);

    //attachment
    VisTester<TreeType> *tester = new VisTester<TreeType>(distanceField);
    out.attachment = new Attachment(newMesh, given, out.embedding, tester);
	//out.attachment.a.AttachmentPrivate1中的weights保存着每个bones对顶点的权重值

    //cleanup
    delete tester;
    delete distanceField;

    return out;
}

