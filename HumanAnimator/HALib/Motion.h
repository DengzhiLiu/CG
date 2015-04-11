/**
* $Id: Motion.h
*
* Process the motion capture data
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef MOTION_H
#define MOTION_H

#define MOCAP_JOINT_NUM 16
#define MOCAP_DOF_NUM 26

#include <string>
using namespace std;

extern int MOCAP_BVH[42];
extern int BVH_SIGN[42];

class MoCap 
{
public:
	float mocap_motion[MOCAP_DOF_NUM];
	float mocap_length[MOCAP_JOINT_NUM];

public:
	MoCap();
	~MoCap();

	bool readMocapFile(string filename);
};

#endif