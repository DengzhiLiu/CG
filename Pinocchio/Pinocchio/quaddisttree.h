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

#ifndef QUADDISTTREE_H
#define QUADDISTTREE_H

#include "hashutils.h"
#include "dtree.h"
#include "multilinear.h"
#include "intersector.h"
#include "pointprojector.h"
#include <numeric>
#include <map>

template<int Dim>
class DistFunction : public Multilinear<double, Dim>
{
    typedef Multilinear<double, Dim> super;
    typedef Rect<double, Dim> MyRect;
public:
    template<class Eval> void initFunc(const Eval &eval, const MyRect &rect)
    {
        for(int i = 0; i < (1 << Dim); ++i) {
            setValue(i, eval(rect.getCorner(i)));
        }
        return;
    }
    
private:
};

template<int Dim>
class DistData : public DistFunction<Dim>
{
public:
    typedef DistFunction<Dim> super;
    typedef DNode<DistData<Dim>, Dim> NodeType;

    DistData(NodeType *inNode) : node(inNode) {}

    void init() { }

    template<class Eval, template<typename Node, int IDim> class Indexer>
	// fullSplit(eval, tol, out, 0, true)
    void fullSplit(const Eval &eval, double tol, DRootNode<DistData<Dim>, Dim, Indexer> *rootNode, int level = 0, bool cropOutside = false)
    {
        int i;
        const Rect<double, Dim> &rect = node->getRect();
        node->initFunc(eval, rect);
        
        bool nextCropOutside = cropOutside;
        if(cropOutside && level > 0) {
			// 这里的center取的好像有问题，是不是取的一个随机数
            double center = eval(rect.getCenter());
            double len = rect.getSize().length() * 0.5;
            if(center > len)
                return;
            if(center < -len)
                nextCropOutside = false;
        }
        
        if(level == (32 / Dim))
            return;
        bool doSplit = false;
		// 如果没有父结点的话就分割此结点,如果有父结点了的话就不分割了吗？
        if(node->getParent() == NULL)
            doSplit = true;
		// node->getParent() != NULL时的情况
        if(!doSplit) {
            int idx[Dim + 1];
            for(i = 0; i < Dim + 1; ++i)
                idx[i] = 0;
            Vector<double, Dim> center = rect.getCenter();
            while(idx[Dim] == 0) {
                Vector<double, Dim> cur;
                bool anyMid = false;
                for(i = 0; i < Dim; ++i) {
                    switch(idx[i]) {
                        case 0: cur[i] = rect.getLo()[i]; break;
                        case 1: cur[i] = rect.getHi()[i]; break;
                        case 2: cur[i] = center[i]; anyMid = true; break;
                    }
                }
                if(anyMid && fabs(evaluate(cur) - eval(cur)) > tol) {
                    doSplit = true;
                    break;
                }
                for(i = 0; i < Dim + 1; ++i) {
                    if(idx[i] != 2) {
                        idx[i] += 1;
                        for(--i; i >= 0; --i)
                            idx[i] = 0;
                        break;
                    }
                }
            }
        }
        if(!doSplit)
            return;
		// split将node分成八个结点，每个结点的值都与node的值相同, children的类型为DNode
        rootNode->split(node);
		// 好像每一个cell的子结点numChildren都不同
        for(i = 0; i < NodeType::numChildren; ++i) {
            eval.setRect(Rect<double, Dim>(rect.getCorner(i)) | Rect<double, Dim>(rect.getCenter()));
			// 层层递归，开始进行下一次的分割
            node->getChild(i)->fullSplit(eval, tol, rootNode, level + 1, nextCropOutside);
        }
    }

    template<class Real> Real evaluate(const Vector<Real, Dim> &v)
    {
        if(node->getChild(0) == NULL) {
			// 可能是利用线性插值计算出的梯度值
            return super::evaluate((v - node->getRect().getLo()).apply(divides<Real>(),
                                                                       node->getRect().getSize()));
        }
        Vector<Real, Dim> center = node->getRect().getCenter();
        int idx = 0;
        for(int i = 0; i < Dim; ++i)
            if(v[i] > center[i])
                idx += (1 << i);
        return node->getChild(idx)->evaluate(v);
    }

    template<class Real> Real integrate(Rect<Real, Dim> r)
    {
        r &= Rect<Real, Dim>(node->getRect());
        if(r.isEmpty())
            return Real();
        if(node->getChild(0) == NULL) {
            Vector<Real, Dim> corner = node->getRect().getLo(), size = node->getRect().getSize();
            Rect<Real, Dim> adjRect((r.getLo() - corner).apply(divides<Real>(), size),
                                    (r.getHi() - corner).apply(divides<Real>(), size));
            return Real(node->getRect().getContent()) * super::integrate(adjRect);
        }
        Real out = Real();
        for(int i = 0; i < NodeType::numChildren; ++i)
            out += node->getChild(i)->integrate(r);
        return out;
    }

private:
    NodeType *node;
};

typedef DistData<3>::NodeType OctTreeNode;
typedef DRootNode<DistData<3>, 3> OctTreeRoot;

template<class RootNode = OctTreeRoot> class OctTreeMaker 
{
public:
	// proj中保存的是三角面片和二叉分好之后的三角面片索引
    static RootNode *make(const ObjectProjector<3, Tri3Object> &proj, const Mesh &m, double tol)
    {
        DistObjEval eval(proj, m);
		// eval中的proj的rnodes中存储的是分好的二叉树，objs是三角面片, mint中triangles存储的是
		// 在v1, v2方向上的bounds包围盒中，与每个cell(200*200)相交的直线(直线为三角形顶点在v1, v2方向上的投影长度)
		// mint中sNormals存储的是每个三角面片的法向在dir方向上的投影
        RootNode *out = new RootNode();

		// 分别对out中ArrayIndexer的root和table进行操作
        out->fullSplit(eval, tol, out, 0, true);
        out->preprocessIndex();

        return out;
    }

    static RootNode *make(const ObjectProjector<3, Vec3Object> &proj, double tol, const RootNode *dTree = NULL)
    {
        PointObjDistEval eval(proj, dTree);
        RootNode *out = new RootNode();

        out->fullSplit(eval, tol, out);
        out->preprocessIndex();

        return out;
    }

private:
    class DistObjEval
    {
    public:
        DistObjEval(const ObjectProjector<3, Tri3Object> &inProj, const Mesh &m) : proj(inProj), mint(m, Vector3(1, 0, 0))
        {
            level = 0;
            rects[0] = Rect3(Vector3(), Vector3(1.));
            inside[0] = 0;
        }

		// eval(rect.getCenter() (lo+hi)/2)
        double operator()(const Vector3 &vec) const
        {
            unsigned int cur = ROUND(vec[0] * 1023.) + 1024 * (ROUND(vec[1] * 1023.) + 1024 * ROUND(vec[2] * 1023.));
            unsigned int sz = cache.size();
            double &d = cache[cur];
            if(sz == cache.size())
                return d;
            return d = compute(vec);
        }

        void setRect(const Rect3 &r) const
        {
            while(!(rects[level].contains(r.getCenter()))) --level;

            inside[level + 1] = inside[level];
            ++level;
            if(!inside[level]) {
                double d = (*this)(r.getCenter());
                double diag2 = r.getSize().length() * 0.5;
            
                if(d >= diag2)
                    inside[level] = 1;
                else if(d <= -diag2)
                    inside[level] = -1;
            }
                       
            rects[level] = r;
        }

    private:
        double compute(const Vector3 &vec) const
        {
            int i, ins = inside[level];
            if(!ins) {
                ins = 1;
                vector<Vector3> isecs = mint.intersect(vec);
                for(i = 0; i < (int)isecs.size(); ++i) {
                    if(isecs[i][0] > vec[0])
                        ins = -ins;
                }
            }
            
            return (vec - proj.project(vec)).length() * ins;
        }
        
        mutable map<unsigned int, double> cache;
        const ObjectProjector<3, Tri3Object> &proj;
        Intersector mint;
        mutable Rect3 rects[11];
        mutable int inside[11];
        mutable int level; //essentially index of last rect
    };
    
    class PointObjDistEval
    {
    public:
        PointObjDistEval(const ObjectProjector<3, Vec3Object> &inProj, const RootNode *inDTree) : proj(inProj), dTree(inDTree) {}

        double operator()(const Vector3 &vec) const
        {
            unsigned int cur = ROUND(vec[0] * 1023.) + 1024 * (ROUND(vec[1] * 1023.) + 1024 * ROUND(vec[2] * 1023.));
            unsigned int sz = cache.size();
            double &d = cache[cur];
            if(sz == cache.size())
                return d;
            return d = (vec - proj.project(vec)).length();
        }

        void setRect(const Rect3 &r) const { }

    private:
        mutable hash_map<unsigned int, double> cache;
        const ObjectProjector<3, Vec3Object> &proj;
        const RootNode *dTree;
    };
};
#endif
