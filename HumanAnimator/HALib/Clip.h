/**
* $Id: vector.h
*
* Each Bone holds information like:
*	bindrot		used when initial bone's orientation doesn't suit for bind model
*	bindposeinv transform coordinate of mesh vertex into local coordinate of bone
*	motion		bone's transform in each frame
*
* Clip is a chain of bones, Joint can be indexed by ID or Name
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef _CLIP_H_
#define _CLIP_H_

#pragma warning(disable: 4244)
#pragma warning(disable: 4267)

#include "../Common/vector.h"
#include "Joint.h"

#include <map>
#include <vector>
#include <string>
using namespace   std;

// This is the function used for comparing names in the stl map
struct namesCompare
{
	bool operator()(const string s1, const string s2) const
	{
		return (strcmp(s1.c_str(),s2.c_str()) < 0);
	}
};
typedef std::vector<Joint*> JointsVector;						// This structure will hold our motionData

typedef std::map<std::string, Joint *, namesCompare> mappedNames;		// This structure gives us the position of the joint in the joints vector

//////////////////////////////////////////////////////////////////////////

class Bone
{
public:
	Bone(string n, int id) { name = n; ID = id;}
	~Bone() { motion.clear(); }
	Bone(const Bone& bone) {
		ID = bone.ID;
		name = bone.name;
		bindRot = bone.bindRot;
		bindposeinv = bone.bindposeinv;
		motion = bone.motion;
	}

	int		getID()			{ return ID; }
	string	getName()		{ return name; }
	
	void			setHead(Joint* pjoint) { head = pjoint; }
	Joint*			getHead() { return head; }

	void			setBindRot(Quaternion<> qrot) { bindRot = qrot; }
	Quaternion<>	getBindRot() { return bindRot; }

	void			setBindTrans(Transform<> trans) { bindposeinv = trans; }
	Transform<>		getBindTrans() { return bindposeinv; }

	void			addMotion(Transform<> m) { motion.push_back(m); }
	Transform<>		getMotion(int frame_no) { return motion[frame_no]; }

	void			insertMotion(int frame, Transform<> trans) { motion.insert(motion.begin() + frame,  trans); }

private:
	int					 ID;
	string				 name;
	Joint*				 head;
	Quaternion<>		 bindRot;
	Transform<>			 bindposeinv;
	vector<Transform<> > motion;
};

typedef vector<Bone *> BoneVector;

//////////////////////////////////////////////////////////////////////////
class Clip
{
public:
	Clip();
	~Clip();
	Clip(const Clip& clip);

	enum MotionType { BVHFILE, SKELETONFILE };
	channelVector   channels;				//all joints channel ,corresponding to the motion data 

	mappedNames		mappedJointNames;		// This is a map of joint names
	mappedNames		mappedEffectorNames;	// This is a map of joint names that are effectors
	JointsVector	joints;					// This is a vector of joints

	void	setName(string Name);
	string	getName();

	float	getInterval();
	int		getNumFrames();
	void	setFrameNum(int num) { num_frame = num; }
	int		getJointNum() { return (int)joints.size(); }

	Joint*  getJointByID(int id) { return joints[id]; }
	Joint*	getJointByName(string joint_name);
	Joint*	getEffectorByName(string Effector_name);

	bool	readMotionCap(const string filename);
	void	calJointTransform();
	void    calJointTransform(Joint* pjoint, int frameno);

private:
	string			name;				// usually name is the bvh file name
	float			interval;			// how long each frame lasts for
	int 			num_frame;			// number of frames in animation
};

#endif //_CLIP_H_