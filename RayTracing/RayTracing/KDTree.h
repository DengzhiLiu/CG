#ifndef _KDTREE_H_
#define _KDTREE_H_

#pragma once
#include "Ray.h"
#include "BBox.h"
#include "Sphere.h"
#include "Group.h"
#include "Triangle.h"

// KdAccelNode Declarations
struct MailboxPrim {
	MailboxPrim(Triangle* p) {
		primitive = p;
		lastMailboxId = -1;
	}
	Triangle* primitive;
	int lastMailboxId;
};

struct KdAccelNode {
	// KdAccelNode Methods
	void initLeaf(int *primNums, int np, MailboxPrim *mailboxPrims) {
			//maxDepth.Max(depth);
			nPrims = np << 2;
			flags |= 3;
			// Store _MailboxPrim *_s for leaf node
			if (np == 0)
				onePrimitive = NULL;
			else if (np == 1)
				onePrimitive = &mailboxPrims[primNums[0]];
			else {
				primitives = (MailboxPrim **)malloc(np * sizeof(MailboxPrim *));
				for (int i = 0; i < np; ++i)
					primitives[i] = &mailboxPrims[primNums[i]];
			}
	}
	void initInterior(int axis, double s) {
		split = s;
		flags &= ~3;
		flags |= axis;
	}
	double SplitPos() const { return split; }
	int nPrimitives() const { return nPrims >> 2; }
	int SplitAxis() const { return flags & 3; }
	bool IsLeaf() const { return (flags & 3) == 3; }
	union {
		u_int flags;   // Both
		double split;   // Interior
		u_int nPrims;  // Leaf
	};
	union {
		u_int aboveChild;           // Interior
		MailboxPrim *onePrimitive;  // Leaf
		MailboxPrim **primitives;   // Leaf
	};
};

struct KdToDo {
	const KdAccelNode *node;
	double tmin, tmax;
};

struct BoundEdge {
	// BoundEdge Public Methods
	BoundEdge() { }
	BoundEdge(double tt, int pn, bool starting) {
		t = tt;
		primNum = pn;
		type = starting ? START : END;
	}
	bool operator<(const BoundEdge &e) const {
		if (t == e.t)
			return (int)type < (int)e.type;
		else return t < e.t;
	}
	double t;
	int primNum;
	enum { START, END } type;
};

// KdTreeAccel Declarations
struct KdAccelNode;
class  KdTreeAccel {
public:
	// KdTreeAccel Private Data
	int isectCost, traversalCost, maxPrims;
	float emptyBonus;
	u_int nMailboxes;
	MailboxPrim *mailboxPrims;
	//mutable int curMailboxId;
	KdAccelNode *nodes;
	vector<Sphere *> spherelist;
	int nAllocedNodes, nextFreeNode;
	BBox bounds;

public:
	// KdTreeAccel Public Methods
	KdTreeAccel(Group* group, vector<Sphere *> sphere);
	BBox WorldBound() const { return bounds; }
	bool CanIntersect() const { return true; }
	~KdTreeAccel();
	void buildTree(int nodeNum, BBox &bounds,	const vector<BBox> &primBounds,
		int *primNums, int nprims, int depth, BoundEdge *edges[3], int *prims0, int *prims1, int badRefines = 0);
	bool Intersect(Ray &ray, bool flag, Color &factor ) const;
};

#endif