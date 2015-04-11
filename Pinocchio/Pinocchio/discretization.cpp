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
#include "deriv.h"
#include "debugging.h"

//fits mesh inside unit cube, makes sure there's exactly one connected component
Mesh  prepareMesh(const Mesh &m)
{
    Mesh out = m;

    if(!m.isConnected()) {
        Debugging::out() << "Bad mesh: should be a single connected component" << endl;
        return Mesh();
    }

    out.computeVertexNormals();
    out.normalizeBoundingBox();

    return out;
}


//constructs a distance field on an octree--user responsible for deleting output
TreeType *constructDistanceField(const Mesh &m, double tol)
{
    vector<Tri3Object> triobjvec;
    for(int i = 0; i < (int)m.edges.size(); i += 3) {
        Vector3 v1 = m.vertices[m.edges[i].vertex].pos;
        Vector3 v2 = m.vertices[m.edges[i + 1].vertex].pos;
        Vector3 v3 = m.vertices[m.edges[i + 2].vertex].pos;
        
        triobjvec.push_back(Tri3Object(v1, v2, v3));
    }
    
	// 将空间面片投影到二叉树结构体中,二叉树中按照x,y,z的坐标值顺序进行均匀剖分
	// proj实际上保存的是一棵均匀二叉树，rnodes保存的是二叉树各个顶点的索引
	// objs保存的是其三角面信息
    ObjectProjector<3, Tri3Object> proj(triobjvec);

    TreeType *out = OctTreeMaker<TreeType>().make(proj, m, tol);

    Debugging::out() << "Done fullSplit " << out->countNodes() << " " << out->maxLevel() << endl;

    return out;
}

// 计算八个方向的梯度向量
double getMinDot(TreeType *distanceField, const Vector3 &c, double step)
{
    typedef Deriv<double, 3> D;
    typedef Vector<D, 3> VD;
    
    int i, j;
    vector<Vector3> vecs;
    vecs.push_back(Vector3(step, step, step));
    vecs.push_back(Vector3(step, step, -step));
    vecs.push_back(Vector3(step, -step, step));
    vecs.push_back(Vector3(step, -step, -step));
    vecs.push_back(Vector3(-step, step, step));
    vecs.push_back(Vector3(-step, step, -step));
    vecs.push_back(Vector3(-step, -step, step));
    vecs.push_back(Vector3(-step, -step, -step));
    
    for(i = 0; i < (int)vecs.size(); ++i) {
        vecs[i] += c;
		// Deriv(const Real &inX, int varNum) : x(inX) { d[varNum] = Real(1.); }
        VD vd = VD(D(vecs[i][0], 0), D(vecs[i][1], 1), D(vecs[i][2], 2));
        
		// 求出vecs向量在网格的一个cell上的梯度值result
        D result = distanceField->locate(vecs[i])->evaluate(vd);
        vecs[i] = Vector3(result.getDeriv(0), result.getDeriv(1), result.getDeriv(2)).normalize();
    }
    
    double minDot = 1.;
    
	// 求出梯度中两两的最小内积
    for(i = 1; i < (int)vecs.size(); ++i) for(j = 0; j < i; ++j) {
		// 计算其内积
        minDot = min(minDot, vecs[i] * vecs[j]);
    }
    
    return minDot;
}

bool sphereComp(const Sphere &s1, const Sphere &s2) { return s1.radius > s2.radius; }

//samples the distance field to find spheres on the medial surface
//output is sorted by radius in decreasing order
vector<Sphere> sampleMedialSurface(TreeType *distanceField, double tol)
{
    int i;
    vector<Sphere> out;

    vector<OctTreeNode *> todo;
    todo.push_back(distanceField);
    int inTodo = 0;
    while(inTodo < (int)todo.size()) {
        OctTreeNode *cur = todo[inTodo];
		// inTodo记录的是层数
        ++inTodo;
		// 采用的是深度优先的遍历算法，从第1个子结点遍历到第8个子结点
        if(cur->getChild(0)) {
            for(i = 0; i < 8; ++i) {
                todo.push_back(cur->getChild(i));
            }
            continue;
        }
        
        //we are at octree leaf
        Rect3 r = cur->getRect();
        double rad = r.getSize().length() / 2.;
        Vector3 c = r.getCenter();
		// 获得此网格八个方向最小的梯度向量(两两梯度的内积中的最小的值)
        double dot = getMinDot(distanceField, c, rad);
        if(dot > 0.)
            continue;
    
        //we are likely near medial surface
        double step = tol;
        double x, y;
        vector<Vector3> pts;
        double sz = r.getSize()[0];
		// sz中对角线向量中x方向的长度
        for(x = 0; x <= sz; x += step) for(y = 0; y <= sz; y += step) {
            pts.push_back(r.getLo() + Vector3(x, y, 0));
			// 保证不重复加x轴
            if(y != 0.)
                pts.push_back(r.getLo() + Vector3(x, 0, y));
            if(x != 0. && y != 0.)
                pts.push_back(r.getLo() + Vector3(0, x, y));
        }
        
        //pts now contains a grid on 3 of the octree cell faces (that's enough)
        for(i = 0; i < (int)pts.size(); ++i) {
            Vector3 &p = pts[i];
            double dist = -distanceField->locate(p)->evaluate(p);
            if(dist <= 2. * step)
                continue; //we want to be well inside
            double dot = getMinDot(distanceField, p, step * 0.001);
            if(dot > 0.0)
			//if(dot > -0.4991)
                continue;
			// 圆球中只压入半径比较大的球心
             out.push_back(Sphere(p, dist));
        }
    }
    
    Debugging::out() << "Medial axis points = " << out.size() << endl;
    
    sort(out.begin(), out.end(), sphereComp);

    return out;
}

//takes sorted medial surface samples and sparsifies the vector
vector<Sphere> packSpheres(const vector<Sphere> &samples, int maxSpheres)
{
    int i, j;
    vector<Sphere> out;

    for(i = 0; i < (int)samples.size(); ++i) {
        for(j = 0; j < (int)out.size(); ++j) {
			// sq与SQR均为长度的平方
            if((out[j].center - samples[i].center).lengthsq() < SQR(out[j].radius))
                    break;
        }
        if(j < (int)out.size())
            continue;

		// 在已经加入的圆中没有找到包含这个圆心的圆
        out.push_back(samples[i]);
        if((int)out.size() > maxSpheres)
            break;
    }

    return out;
}

double getMaxDist(TreeType *distanceField, const Vector3 &v1, const Vector3 &v2, double maxAllowed)
{
    double maxDist = -1e37;
    Vector3 diff = (v2 - v1) / 100.;
    for(int k = 0; k < 101; ++k) {
        Vector3 pt = v1 + diff * double(k);
        maxDist = max(maxDist, distanceField->locate(pt)->evaluate(pt));
        if(maxDist > maxAllowed)
            break;
    }
    return maxDist;
}

//constructs graph on packed sphere centers
PtGraph connectSamples(TreeType *distanceField, const vector<Sphere> &spheres)
{
    int i, j;
    PtGraph out;

    for(i = 0; i < (int)spheres.size(); ++i)
        out.verts.push_back(spheres[i].center);
    out.edges.resize(spheres.size());
        
    for(i = 1; i < (int)spheres.size(); ++i) for(j = 0; j < i; ++j) {
        int k;
        Vector3 ctr = (spheres[i].center + spheres[j].center) * 0.5;
        double radsq = (spheres[i].center - spheres[j].center).lengthsq() * 0.25;
        if(radsq < SQR(spheres[i].radius + spheres[j].radius) * 0.25) {
			//if spheres intersect, there should be an edge
			// 两个球相交,则在两个球心之间加入一条边
            out.edges[i].push_back(j);
            out.edges[j].push_back(i);
            continue;
        }
        for(k = 0; k < (int)spheres.size(); ++k) {
            if(k == i || k == j)
                continue;
			// radsq为两个球心间距离一半的平方,这里好像有问题
            if((spheres[k].center - ctr).lengthsq() < radsq)
                break; //gabriel graph condition violation
        }
        if(k < (int)spheres.size())
            continue;
		// 没有找到一个球与这两个球的球心连线的中点的距离小于两球之间距离的情况
        //every point on edge should be at least this far in:
        double maxAllowed = -.5 * min(spheres[i].radius, spheres[j].radius);
        if(getMaxDist(distanceField, spheres[i].center, spheres[j].center, maxAllowed) < maxAllowed) {
			// 程序中这种情况是没有遇到的
            out.edges[i].push_back(j);
            out.edges[j].push_back(i);
        }
    }

    return out;
}
