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

#include <fstream>
#include <sstream>
#include "attachment.h"
#include "vecutils.h"
#include "lsqSolver.h"

class AttachmentPrivate
{
public:
    AttachmentPrivate() {}
    virtual ~AttachmentPrivate() {}
    virtual Mesh deform(const Mesh &mesh, const vector<Transform<> > &transforms) const = 0;
    virtual Vector<double, -1> getWeights(int i) const = 0;
    virtual AttachmentPrivate *clone() const = 0;
};
    
bool vectorInCone(const Vector3 &v, const vector<Vector3> &ns)
{
    int i;
    Vector3 avg;
    for(i = 0; i < (int)ns.size(); ++i)
        avg += ns[i];
        
    return v.normalize() * avg.normalize() > 0.5;
}

class AttachmentPrivate1 : public AttachmentPrivate {
public:
    AttachmentPrivate1() {}

	// 计算网格中各个顶点运动时的权值
    AttachmentPrivate1(const Mesh &mesh, const Skeleton &skeleton, const vector<Vector3> &match, const VisibilityTester *tester)
    {
        int i, j;
        int nv = mesh.vertices.size();
        //compute edges
        vector<vector<int> > edges(nv);
 
        for(i = 0; i < nv; ++i) {
            int cur, start;
            cur = start = mesh.vertices[i].edge;
			// 遍历一个循环,这个偱环中所包含的点为与点i相关联的所有顶点
            do {
                edges[i].push_back(mesh.edges[cur].vertex);
                cur = mesh.edges[mesh.edges[cur].prev].twin;
            } while(cur != start);
        }
    
        weights.resize(nv);
        int bones = skeleton.fGraph().verts.size() - 1;

		// 每个顶点最多关联bones条边
        for(i = 0; i < nv; ++i) // initialize the weights vectors so they are big enough
            weights[i][bones - 1] = 0.;

        vector<vector<double> > boneDists(nv);
        vector<vector<bool> > boneVis(nv);
        
        for(i = 0; i < nv; ++i) {
            boneDists[i].resize(bones, -1);
            boneVis[i].resize(bones);
            Vector3 cPos = mesh.vertices[i].pos;
                
            vector<Vector3> normals;
			// 计算与顶点i相关联的所有顶点的法向和
            for(j = 0; j < (int)edges[i].size(); ++j) {
				// nj为edge[i].vertexlist中j的下一个顶点
                int nj = (j + 1) % edges[i].size();
                Vector3 v1 = mesh.vertices[edges[i][j]].pos - cPos;
                Vector3 v2 = mesh.vertices[edges[i][nj]].pos - cPos;
                normals.push_back((v1 % v2).normalize());
            }
                    
            double minDist = 1e37;
            for(j = 1; j <= bones; ++j) {
                const Vector3 &v1 = match[j], &v2 = match[skeleton.fPrev()[j]];
				// boneDists存储的是顶点到bone(v1 v2)的距离的平方
                boneDists[i][j - 1] = sqrt(distsqToSeg(cPos, v1, v2));
                minDist = min(boneDists[i][j - 1], minDist);
            }
			// minDist保存着顶点i到所有bones的最小值,下面是处理每个bone与顶点i的关系
            for(j = 1; j <= bones; ++j) {
                //the reason we don't just pick the closest bone is so that if two are
                //equally close, both are factored in.
                if(boneDists[i][j - 1] > minDist * 1.0001)
                    continue;

				// j的值即为到顶点i的距离最短的bones
                const Vector3 &v1 = match[j], &v2 = match[skeleton.fPrev()[j]];
				// 返回的点p是cpos在线段v1-v2上的投影坐标
                Vector3 p = projToSeg(cPos, v1, v2);
				// canSee返回的是p是否比cPos更靠近内部，vectorInCone返回的则是cPos-p向量与
				// 过顶点i所有法线的内积是否大于0.5，即夹角小于acos(0.5)
                boneVis[i][j - 1] = tester->canSee(cPos, p) && vectorInCone(cPos - p, normals);
            }
        }

        //We have -Lw+Hw=HI, same as (H-L)w=HI, with (H-L)=DA (with D=diag(1./area))
        //so w = A^-1 (HI/D)

        vector<vector<pair<int, double> > > A(nv);
        vector<double> D(nv, 0.), H(nv, 0.);
        vector<int> closest(nv, -1);
        for(i = 0; i < nv; ++i) {
            //get areas
            for(j = 0; j < (int)edges[i].size(); ++j) {
                int nj = (j + 1) % edges[i].size();
        
				// edges[i]存储的是从顶点i出发的一个边的偱环
                D[i] += ((mesh.vertices[edges[i][j]].pos - mesh.vertices[i].pos) %
                         (mesh.vertices[edges[i][nj]].pos - mesh.vertices[i].pos)).length();
            }
			// D[i]为其法向和的倒数
            D[i] = 1. / (1e-10 + D[i]);

            //get bones
			// 寻找最近的bones,存储在closet[nv]中
            double minDist = 1e37;
            for(j = 0; j < bones; ++j) {
                if(boneDists[i][j] < minDist) {
                    closest[i] = j;
                    minDist = boneDists[i][j];
                }
            }
			// closest[]存储的是离顶点i最近的bones
            for(j = 0; j < bones; ++j)
                if(boneVis[i][j] && boneDists[i][j] <= minDist * 1.00001)
                    H[i] += 1. / SQR(1e-8 + boneDists[i][closest[i]]);
			// H[i]为boneDist[i][j]平方的倒数和

            //get laplacian
            double sum = 0.;
            for(j = 0; j < (int)edges[i].size(); ++j) {
                int nj = (j + 1) % edges[i].size();
				// pj为以edges[i].size()为循环的j的前一个序列索引
                int pj = (j + edges[i].size() - 1) % edges[i].size();

                Vector3 v1 = mesh.vertices[i].pos - mesh.vertices[edges[i][pj]].pos;
                Vector3 v2 = mesh.vertices[edges[i][j]].pos - mesh.vertices[edges[i][pj]].pos;
                Vector3 v3 = mesh.vertices[i].pos - mesh.vertices[edges[i][nj]].pos;
                Vector3 v4 = mesh.vertices[edges[i][j]].pos - mesh.vertices[edges[i][nj]].pos;
                
				// 内积除以点积的长度是什么意思???
                double cot1 = (v1 * v2) / (1e-6 + (v1 % v2).length());
                double cot2 = (v3 * v4) / (1e-6 + (v3 % v4).length());
                sum += (cot1 + cot2);

                if(edges[i][j] > i) //check for triangular here because sum should be computed regardless
                    continue;
                A[i].push_back(make_pair(edges[i][j], -cot1 - cot2));
            }	// 跳出了所有骨骼的循环

            A[i].push_back(make_pair(i, sum + H[i] / D[i]));

            sort(A[i].begin(), A[i].end());
        }
		// A[i]中存入的值有两种情况，一种是顶点i与bones相关联的权重，另一个是与其相联的顶点间的权重

        nzweights.resize(nv);
        SPDMatrix Am(A);


        LLTMatrix *Ainv = Am.factor();
        if(Ainv == NULL)
            return;

        for(j = 0; j < bones; ++j) {
            vector<double> rhs(nv, 0.);
            for(i = 0; i < nv; ++i) {
                if(boneVis[i][j] && boneDists[i][j] <= boneDists[i][closest[i]] * 1.00001)
                    rhs[i] = H[i] / D[i];
            }

			// 解一个线性方程,解出来的解保存在rhs中
            Ainv->solve(rhs);           
            for(i = 0; i < nv; ++i) {
                if(rhs[i] > 1.)
                    rhs[i] = 1.; //clip just in case
                if(rhs[i] > 1e-8)
                    nzweights[i].push_back(make_pair(j, rhs[i]));
            }
        }

        for(i = 0; i < nv; ++i) {
            double sum = 0.;
            for(j = 0; j < (int)nzweights[i].size(); ++j)
                sum += nzweights[i][j].second;

            for(j = 0; j < (int)nzweights[i].size(); ++j) {
                nzweights[i][j].second /= sum;
                weights[i][nzweights[i][j].first] = nzweights[i][j].second;
            }
        }
		// 将每个bones对每个顶点的权重值进行归一化后存在weights数组中

        delete Ainv;
        return;
    }

    Mesh deform(const Mesh &mesh, const vector<Transform<> > &transforms) const
    {
        Mesh out = mesh;
        int i, nv = mesh.vertices.size();

        if(mesh.vertices.size() != weights.size())
            return out; //error

        for(i = 0; i < nv; ++i) {
            Vector3 newPos;
            int j;
            for(j = 0; j < (int)nzweights[i].size(); ++j) {
				// 将每个顶点的坐标与旋转平移向量相乘，将乘积再乘每个节点相对于关节的权重
				// 就得到了旋转变换之后每个顶点的坐标值
                newPos += ((transforms[nzweights[i][j].first] * out.vertices[i].pos) * nzweights[i][j].second);
            }
            out.vertices[i].pos = newPos;
        }
        
		// 重新计算每个顶点的法线
        out.computeVertexNormals();
    
        return out;
    }

    Vector<double, -1> getWeights(int i) const { return weights[i]; }

    AttachmentPrivate *clone() const
    {
        AttachmentPrivate1 *out = new AttachmentPrivate1();
        *out = *this;
        return out;
    }

private:
    vector<Vector<double, -1> > weights;
    vector<vector<pair<int, double> > > nzweights; //sparse representation
};

Attachment::~Attachment()
{
    if(a)
        delete a;   
}

Attachment::Attachment(const Attachment &att)
{
    a = att.a->clone();
}

Vector<double, -1> Attachment::getWeights(int i) const { return a->getWeights(i); }

Mesh Attachment::deform(const Mesh &mesh, const vector<Transform<> > &transforms) const
{
    return a->deform(mesh, transforms);
}

Attachment::Attachment(const Mesh &mesh, const Skeleton &skeleton, const vector<Vector3> &match, const VisibilityTester *tester)
{
    a = new AttachmentPrivate1(mesh, skeleton, match, tester);
}
