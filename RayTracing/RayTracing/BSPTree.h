#ifndef _BSPTREE_H_
#define _BSPTREE_H_

#pragma once
#include "Group.h"

typedef struct BSPTreeNode 
{
	/* �Ľ�ʱ������ÿ����Χ�����趨������������ֵ
	�Ľ���ʱ����Կ��ǽ�vector����ṹ��Ϊ����ṹ��vector�ṹ��ʱ���ܻ᲻̫�ȶ�*/
	int			 subAxis;
	Group*		 objList;
	BSPTreeNode* lchild;
	BSPTreeNode* rchild;
} BSPNode;

class BSPTree
{
public:
	int minnum;			// Ҷ�����С��Ƭ��
	int	maxdepth;		// �������

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