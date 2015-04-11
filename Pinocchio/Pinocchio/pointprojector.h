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

#ifndef POINTPROJECTOR_H
#define POINTPROJECTOR_H

#include <set>

#include "vector.h"
#include "rect.h"
#include "vecutils.h"
#include "debugging.h"

struct Vec3Object
{
    Vec3Object(const Vector3 &inV) : v(inV) {}
    
    Rect3 boundingRect() const { return Rect3(v); }
    double operator[](int i) const { return v[i]; }
    Vector3 project(const Vector3 &) const { return v; }
    
    Vector3 v;
};

struct Tri3Object
{
    Tri3Object(const Vector3 &inV1, const Vector3 &inV2, const Vector3 &inV3) : v1(inV1), v2(inV2), v3(inV3) {}
    
    Rect3 boundingRect() const { return Rect3(v1) | Rect3(v2) | Rect3(v3); }
    double operator[](int i) const { return v1[i] + v2[i] + v3[i]; } //for comparison only, no need to divide by 3
    Vector3 project(const Vector3 &v) const { return projToTri(v, v1, v2, v3); }
    
    Vector3 v1, v2, v3;
};

template<int Dim, class Obj>
class ObjectProjector
{
public:
    typedef Vector<double, Dim> Vec;
    typedef Rect<double, Dim> Rec;

    ObjectProjector() {}
    ObjectProjector(const vector<Obj> &inObjs) : objs(inObjs)
    {
        int i, d;
        vector<int> orders[Dim];
    
		// objs contain Vector3 v1, v2, v3
		// 分别对x, y, z的坐标值进行排序
		// 为什么不是对所有的顶点坐标进行排序，而只是对三角形(一个顶点)进行排序
        for(d = 0; d < Dim; ++d) {
            orders[d].reserve(objs.size());
            for(i = 0; i < (int)objs.size(); ++i)
                orders[d].push_back(i);
			// 按照每个triobj在每个坐标系下三个坐标值的和分别进行排序，排序结果的索引保存在
			// orders[i](i=0,1,2)中
            sort(orders[d].begin(), orders[d].end(), DLess(d, objs));
        }

        rnodes.reserve((int)objs.size() * 2 - 1);
		// 对三角面片triobj进行排序，生成的二叉树保存在rnodes中，rnodes中间层保存的是结点按中
		// 序的深度优先访问的序列号,结点child1值为-1时保存的则为其结点编号，一般只保存一个结点
        initHelper(orders);
    }

    Vec project(const Vec &from) const
    {
        double minDistSq = 1e37;
        Vec closestSoFar;

        int sz = 1;
        static pair<double, int> todo[10000];
        todo[0] = make_pair(rnodes[0].rect.distSqTo(from), 0);

        while(sz > 0) {
            if(todo[--sz].first > minDistSq) {
                continue;
            }
            int cur = todo[sz].second; //the top element that was just popped
        
            int c1 = rnodes[cur].child1;
            int c2 = rnodes[cur].child2;
        
            if(c1 >= 0) { //not a leaf
                double l1 = rnodes[c1].rect.distSqTo(from);
                if(l1 < minDistSq)
                    todo[sz++] = make_pair(l1, c1);
            
                double l2 = rnodes[c2].rect.distSqTo(from);
                if(l2 < minDistSq)
                    todo[sz++] = make_pair(l2, c2);
            
                if(sz >= 2 && todo[sz - 1].first > todo[sz - 2].first) {
                    swap(todo[sz - 1], todo[sz - 2]);
                }
                if(sz > 9995) {//getting close to our array limit
                    Debugging::out() << "Large todo list, likely to fail" << endl;
                }
                continue;
            }

            //leaf -- consider the object
            Vec curPt = objs[c2].project(from);
            double distSq = (from - curPt).lengthsq();
            if(distSq <= minDistSq) {
                minDistSq = distSq;
                closestSoFar = curPt;
            }
        }

        return closestSoFar;
    };

    struct RNode
    {
        Rec rect;
        int child1, child2; //if child1 is -1, child2 is the object index
    };

    const vector<RNode> &getRNodes() const { return rnodes; }

private:

    struct DL { bool operator()(const pair<double, int> &p1,
                                const pair<double, int> &p2) const { return p1.first > p2.first; } };
        
	// 传入参数orders是根据每个triobj在每个坐标系下坐标值的和进行排序而得到的索引
    int initHelper(vector<int> orders[Dim], int curDim = 0)
    {
        int out = rnodes.size();
        rnodes.resize(out + 1);
    
		// orders的第一维是维度(0~3),第二维是三角面片的个数ox003416
		// roders中保存的是面片的顶点索引
		// roders中二叉树的剖分方法是按照中值进行剖分，将比中值小的存在其左子树child1中，
		// 比中值大的存在其右子树child2中
		// 在建成的二叉树中，中间层存储的是其序列号(深度优先访问的数组,中序访问)，子结点中只存储了
		// 一个元素，当其child1的值为-1时，child2的值就是排序之后的triobj的索引
        int num = orders[0].size();
        if(num == 1) {
            rnodes[out].rect = objs[orders[0][0]].boundingRect();
            rnodes[out].child1 = -1;
            rnodes[out].child2 = orders[0][0];
        }
        else {
            int i, d;
            vector<int> orders1[Dim], orders2[Dim];
            set<int> left;
            for(i = 0; i < num / 2; ++i)
                left.insert(orders[curDim][i]);
        
            for(d = 0; d < Dim; ++d) {
                orders1[d].reserve((num + 1) / 2);
                orders2[d].reserve((num + 1) / 2);
                for(i = 0; i < num; ++i) {
                    if(left.count(orders[d][i]))
                        orders1[d].push_back(orders[d][i]);
                    else
                        orders2[d].push_back(orders[d][i]);
                }
            }
        
			// orders1中为curDim中，oder中前一半的元素，order2为后一半的元素
            rnodes[out].child1 = initHelper(orders1, (curDim + 1) % Dim);
            rnodes[out].child2 = initHelper(orders2, (curDim + 1) % Dim);
			// 或操作就是将两个矩形合并
            rnodes[out].rect = rnodes[rnodes[out].child1].rect | rnodes[rnodes[out].child2].rect;
        }
        return out;
    }

    class DLess
    {
    public:
        DLess(int inDim, const vector<Obj> &inObjs) : dim(inDim), objs(inObjs) {}
        bool operator()(int i1, int i2) { return objs[i1][dim] < objs[i2][dim]; }
    private:
        int dim;
        const vector<Obj> &objs;
    };

    vector<RNode> rnodes;
    vector<Obj> objs;
};
#endif
