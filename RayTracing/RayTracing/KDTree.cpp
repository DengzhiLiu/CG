#include <algorithm>
#include <assert.h>
#include "Material.h"
#include "KDTree.h"

#define MAX_TODO 64

// KdTreeAccel Method Definitions
KdTreeAccel::KdTreeAccel(Group* group, vector<Sphere *> sphere)
{
	isectCost = 80 ;				// cost for intersect
	traversalCost = 1;			// cost for traversal
	emptyBonus = 0.5;
	maxPrims = 1;				// max prims in the leaf
	u_int nPrim = group->GetSize();
	spherelist = sphere;

	// Initialize mailboxes for _KdTreeAccel_
	//curMailboxId = 0;
	nMailboxes = nPrim;
	mailboxPrims = (MailboxPrim *)malloc(nMailboxes * sizeof(MailboxPrim));
	for (u_int i = 0; i < nMailboxes; ++i)
		new (&mailboxPrims[i]) MailboxPrim(group->Get(i));
		//mailboxPrims[i] = MailboxPrim(group->Get(i));

	// Build kd-tree for accelerator
	int maxDepth = int( 8 + 1.3 * Log2Int(float(nPrim)) );
	//int maxDepth = 1;
	nextFreeNode = nAllocedNodes = 0;

	// Compute bounds for kd-tree construction
	vector<BBox> primBounds;
	primBounds.reserve(nPrim);
	for (vector<Triangle *>::const_iterator iter = group->objs.begin(); iter != group->objs.end(); ++iter) {
		BBox b = BBox((*iter)->CBPmin, (*iter)->CBPmax);
		bounds = bounds.Union(b);
		primBounds.push_back(b);
	}

	// Allocate working memory for kd-tree construction
	BoundEdge *edges[3];
	for (int i = 0; i < 3; ++i)
		edges[i] = new BoundEdge[2*nPrim];
	int *prims0 = new int[nPrim];
	int *prims1 = new int[(maxDepth+1) * nPrim];
	// Initialize _primNums_ for kd-tree construction
	int *primNums = new int[nPrim];
	for (u_int i = 0; i < nPrim; ++i)
		primNums[i] = i;

	// Start recursive construction of kd-tree
	buildTree(0, bounds, primBounds, primNums, nPrim, maxDepth, edges, prims0, prims1);
	
	// Free working memory for kd-tree construction
	delete[] primNums;
	for (int i = 0; i < 3; ++i)
		delete[] edges[i];
	delete[] prims0;
	delete[] prims1;
}

KdTreeAccel::~KdTreeAccel() {
	for (u_int i = 0; i < nMailboxes; ++i)
		mailboxPrims[i].~MailboxPrim();
	if (mailboxPrims)
		free(mailboxPrims);
	if (nodes)
		free(nodes);
}

void KdTreeAccel::buildTree(int nodeNum, BBox &nodeBounds, const vector<BBox> &allPrimBounds, int *primNums,
							int nPrims, int depth, BoundEdge *edges[3], int *prims0, int *prims1, int badRefines)
{
	if (nPrims == 0)	return;
	
	// Get next free node from _nodes_ array
	if (nextFreeNode == nAllocedNodes) {
		int nAlloc = max(2 * nAllocedNodes, 512);
		KdAccelNode *n = (KdAccelNode *)malloc(nAlloc *	sizeof(KdAccelNode));
		if (nAllocedNodes > 0) {
			memcpy(n, nodes, nAllocedNodes * sizeof(KdAccelNode));
			free(nodes);
		}
		nodes = n;
		nAllocedNodes = nAlloc;
	}
	++nextFreeNode;

	// Initialize leaf node if termination criteria met
	if ((nPrims <= maxPrims) || depth == 0) {
		nodes[nodeNum].initLeaf(primNums, nPrims, mailboxPrims);
		return;
	}

	// Initialize interior node and continue recursion
	// Choose split axis position for interior node
	int bestAxis = -1, bestOffset = -1;
	double bestCost = INFINITY;
	double oldCost = isectCost * double(nPrims);
	Vector d = nodeBounds.pMax - nodeBounds.pMin;
	double totalSA = 2.0 * (d.X()*d.Y() + d.X()*d.Z() + d.Y()*d.Z());
	double invTotalSA = 1.0 / totalSA;
	// Choose which axis to split along
	int axis = d.MainAxis();
	u_int retries = 0;

retrySplit:
	// Initialize edges for _axis_
	for (int i = 0; i < nPrims; ++i) {
		int pn = primNums[i];
		const BBox &bbox = allPrimBounds[pn];
		edges[axis][2*i] = BoundEdge(bbox.pMin.d[axis], pn, true);
		edges[axis][2*i+1] = BoundEdge(bbox.pMax.d[axis], pn, false);
	}
	sort(&edges[axis][0], &edges[axis][2*nPrims]);

	// Compute cost of all splits for _axis_ to find best
	int nBelow = 0, nAbove = nPrims;
	int otherAxis[3][2] = { {1,2}, {0,2}, {0,1} };
	//nodeBounds.Expand(EPSILON);					// 划分轴一定不会太接近包围盒的端点
	for (int i = 0; i < 2*nPrims; ++i) {
		if (edges[axis][i].type == BoundEdge::END) --nAbove;
		double edget = edges[axis][i].t;
		if (edget - nodeBounds.pMin.d[axis] > EPSILON && nodeBounds.pMax.d[axis] - edget > EPSILON) {
				// Compute cost for split at _i_th edge
				int otherAxis0 = otherAxis[axis][0];
				int otherAxis1 = otherAxis[axis][1];
				double belowSA = 2 * (d.d[otherAxis0] * d.d[otherAxis1] +
					(edget - nodeBounds.pMin.d[axis]) *	(d.d[otherAxis0] + d.d[otherAxis1]));
				double aboveSA = 2 * (d.d[otherAxis0] * d.d[otherAxis1] +
					(nodeBounds.pMax.d[axis] - edget) * (d.d[otherAxis0] + d.d[otherAxis1]));
				double pBelow = belowSA * invTotalSA;
				double pAbove = aboveSA * invTotalSA;
				double eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0.0;
				double cost = traversalCost + isectCost * (1.0 - eb) * (pBelow * nBelow + pAbove * nAbove);
				// Update best split if this is lowest cost so far
				if (bestCost - cost > EPSILON)  {
					bestCost = cost;
					bestAxis = axis;
					bestOffset = i;
				}
		}
		if (edges[axis][i].type == BoundEdge::START) ++nBelow;
	}
	assert(nBelow == nPrims && nAbove == 0); // NOBOOK
	// Create leaf if no good splits were found
	if (bestAxis == -1 && retries < 2) {
		++retries;
		axis = (axis+1) % 3;
		goto retrySplit;
	}
	if (bestCost - oldCost > EPSILON) ++badRefines;
	if ((bestCost > 4.0 * oldCost && nPrims < 16) || bestAxis == -1 || badRefines == 3) {
			nodes[nodeNum].initLeaf(primNums, nPrims, mailboxPrims);
			return;
	}
	// Classify primitives with respect to split
	int n0 = 0, n1 = 0;
	for (int i = 0; i < bestOffset; ++i)
		if (edges[bestAxis][i].type == BoundEdge::START)
			prims0[n0++] = edges[bestAxis][i].primNum;
	for (int i = bestOffset; i < 2*nPrims; ++i)
		if (edges[bestAxis][i].type == BoundEdge::END)
			prims1[n1++] = edges[bestAxis][i].primNum;
	// Recursively initialize children nodes
	double tsplit = edges[bestAxis][bestOffset].t;
	nodes[nodeNum].initInterior(bestAxis, tsplit);
	BBox bounds0 = nodeBounds, bounds1 = nodeBounds;
	bounds0.pMax.d[bestAxis] = bounds1.pMin.d[bestAxis] = tsplit;
	buildTree(nodeNum+1, bounds0, allPrimBounds, prims0, n0, depth-1, edges,
		prims0, prims1 + nPrims, badRefines);
	nodes[nodeNum].aboveChild = nextFreeNode;
	buildTree(nodes[nodeNum].aboveChild, bounds1, allPrimBounds, prims1, n1, depth-1, edges,
		prims0, prims1 + nPrims, badRefines);
}

bool KdTreeAccel::Intersect(Ray &ray, bool flag, Color &factor) const 
{
	bool hit = false;
	// Compute initial parametric range of ray inside kd-tree extent
	double tmin = 0, tmax = INFINITY;
	// intersect with the sphere first
	for (vector<Sphere *>::const_iterator iter = spherelist.begin(); iter != spherelist.end(); ++iter) {
		if ( (*iter)->Intersect(ray, tmin, tmax) ) {
			if (!flag)	{
				factor = factor * (*iter)->GetMaterial()->kt;
				if (factor.r < EPSILON || factor.g < EPSILON || factor.b < EPSILON)	return true;
			}
			else hit = true;
		}
	}

	if (flag) {
		if (!bounds.Intersect(ray, tmin, tmax))
			return false;
	}
	else {
		tmin = 0.0;
		tmax = 1.0;
	}

	// Prepare to traverse kd-tree for ray
	//int rayId = curMailboxId++;
	Point orig = ray.GetOrigin();
	Vector dir = ray.GetDirection();
	Vector invDir(1.0/dir.X(), 1.0/dir.Y(), 1.0/dir.Z());
	KdToDo todo[MAX_TODO];
	int todoPos = 0;
	// Traverse kd-tree nodes in order for ray
	const KdAccelNode *node = &nodes[0];
	while (node != NULL) {
		// Bail out if we found a hit closer than the current node
		//if (ray.hitDist < tmin) break;
		if (!node->IsLeaf()) {
			// Process kd-tree interior node
			// Compute parametric distance along ray to split plane
			int axis = node->SplitAxis();
			double tplane = (node->SplitPos() - orig.d[axis]) * invDir.d[axis];
			// Get node children pointers for ray
			const KdAccelNode *firstChild, *secondChild;
			int belowFirst = orig.d[axis] <= node->SplitPos();
			if (belowFirst) {
				firstChild = node + 1;
				secondChild = &nodes[node->aboveChild];
			}
			else {
				firstChild = &nodes[node->aboveChild];
				secondChild = node + 1;
			}
			// Advance to next child node, possibly enqueue other child
			if ((tplane - tmax > EPSILON) || tplane < EPSILON)
				node = firstChild;
			else if (tmin - tplane > EPSILON)
				node = secondChild;
			else {
				// Enqueue _secondChild_ in todo list
				todo[todoPos].node = secondChild;
				todo[todoPos].tmin = tplane;
				todo[todoPos].tmax = tmax;
				++todoPos;
				node = firstChild;
				tmax = tplane;
			}
		}
		else {
			// Check for intersections inside leaf node
			u_int nPrimitives = node->nPrimitives();
			if (nPrimitives == 1) {
				MailboxPrim *mp = node->onePrimitive;
				// Check one primitive inside leaf node
				//if (mp->lastMailboxId != rayId) {
					//mp->lastMailboxId = rayId;
				if (mp->primitive->Intersect(ray, tmin, tmax)) {
					if (!flag)	{
						factor = factor * mp->primitive->GetMaterial()->kt;
						if (factor.r < EPSILON || factor.g < EPSILON || factor.b < EPSILON)	return true;
					}
					else hit = true;
				//}
				}
			}
			else {
				MailboxPrim **prims = node->primitives;
				for (u_int i = 0; i < nPrimitives; ++i) {
					MailboxPrim *mp = prims[i];
					// Check one primitive inside leaf node
					//if (mp->lastMailboxId != rayId) {
						//mp->lastMailboxId = rayId;
					if (mp->primitive->Intersect(ray, tmin, tmax)) {
						if (!flag)	{
							factor = factor * mp->primitive->GetMaterial()->kt;
							if (factor.r < EPSILON || factor.g < EPSILON || factor.b < EPSILON)	return true;
						}
						else hit = true;
					}
					//}
				}
			}
			// Grab next node to process from todo list
			if (todoPos > 0) {
				--todoPos;
				node = todo[todoPos].node;
				tmin = todo[todoPos].tmin;
				tmax = todo[todoPos].tmax;
			}
			else
				break;
		}
	}
	return hit;
}