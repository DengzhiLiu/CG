#ifndef _BSPTREE_H_
#define _BSPTREE_H_

#pragma once
#include "Group.h"

typedef struct BSPTreeNode 
{
	/* 改进时可以在每个包围盒中设定物体数量的阈值
	改进的时候可以考虑将vector数组结构改为链表结构，vector结构有时可能会不太稳定*/
	int			 subAxis;
	Group*		 objList;
	BSPTreeNode* lchild;
	BSPTreeNode* rchild;
} BSPNode;

class BSPTree
{
public:
	int minnum;			// 叶结点最小面片数
	int	maxdepth;		// 树的深度

public:
	BSPTree(void);
	~BSPTree(void);

	inline bool IsInBox(Point pos, Point Bmin, Point Bmax);
	inline bool IsIntersectBox(Point* pos, Point Bmin, Point Bmax);
	inline bool SolutionExists(Vector e1, Vector e2, Vector d, Vector L, int axis);
	BSPNode* BuildBSPTree(Group* group, int depth, int axis);
	bool Intersect(Ray &ray, BSPNode* root, double tmin, double tmax, bool flag, Color &factor);
};

#endif