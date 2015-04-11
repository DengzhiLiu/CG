#ifndef _QUADTREE_H
#define _QUADTREE_H

#pragma once
#include "Common.h"

typedef struct QuadTreeNode 
{
	struct QuadTreeNode *left, *right;
	struct QuadTreeNode *top, *bottom;

	QuadTreeNode(): left(NULL), right(NULL), top(NULL), bottom(NULL)
	{}
} QuadNode;


class QuadTree
{
public:
	QuadTree();
	~QuadTree();

	QuadNode* BuildQuadTree();

public:
	//QuadNode *root;
};

#endif