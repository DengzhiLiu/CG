/**
* $Id: Joint.h
*
* Each joint holds information like:
*		name
*		parent's name
*		all its children's name
*		rotation order
*		lenght of joint (this is the offset)
*		channel names (rotateX, rotateY, rotateZ, translateX, ......)
*		frame data for those channels (these are the euler angles)
*		quaternion data for each channel
*		marker data at every frame for original, euler and quaternions
*
* Any time you are required to give the values frameNum to a function in this class
* you will think of 1 as the first frame and you can go to <= total number of frames
*
* Any time you are required to give the values frameNum to a function in this class
* you will think of 1 as the first frame and you can go to <= total number of frames
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/


#ifndef _JOINT_H_
#define _JOINT_H_

#pragma warning(disable: 4786)
#pragma warning(disable: 4788)

#include <map>
#include <vector>
#include <deque>

#include "../Common/vector.h"
#include "../Common/transform.h"
#include "Channel.h"
using namespace std;

// This holds the jointType constant, to use:
enum JOINT_TYPE { NOTYPE=-1, ROOT, JOINT, EFFECTOR};

// This holds the names of a joint's children
typedef std::vector<Joint*> childVector;

// This hold the joints CHANNELS usually 3 sometimes 6, 
// but can be any number
typedef std::vector<Channel *> channelVector;

// This holds a markerPosition for every frame.  
// NOTE: frame 1 index by 0

typedef std::vector<Transform<> > transVector;

class Clip;

class Joint
{
	//friend class Clip;
private:
	int					type;				// Types are ROOT, JOINT, EFFECTOR, and NOTYPE
	int					index;
	bool				used;
	int					channel_num;
	string				name;	
	Vector3      		offset;				// representing the length of the joint
	Vector3				position;

	Joint*				parent;	

	channelVector		channels;		    // STL vector holding the channel names for the joint
	childVector			children;			// STL vector of char* to children names
	
	Transform<>			restpose;			// dressPose
	transVector			reMotionPose;		// relative motionPose
	transVector			abMotionPose;		// absolute motionPose
	
public:	

	Joint();
	~Joint();
	Joint(const Joint& tempJoint);

	// Sets and gets what type of joint the latest addition is.
	void		setType(const char* Type);
	void		setType(int t);
	int			getType();

	void		setUsed(bool flag);
	bool		IsUsed();

	void		setIndex(int Index);
	int         getIndex();

	// Sets and gets what the current joint is named.
	void		setName( char* Name);
	void        setName( std::string Name);
	string		getName();

	// Sets and gets the motion data offset.
	void		setOffset( Vector3 offset);
	Vector3		getOffset();

	void		setPosition(Vector3 pos);
	Vector3		getPosition();
	
	// Sets and gets the joint's only parent.
	void		setParent( Joint* Parent);	
	Joint*		getParent();	

	// Add a channel name to the list and get a channel name from the list
	void		setChannelNumber(int size);
	void		addChannel( Channel* channel);
	Channel*	getChannel(int index);
	int			getChannelNumber();
	int			getCurChannelnumber();

	// Here is how you add a child onto a parents list:
	int			numOfChildren();
	Joint*		getChild(int index);
	void		addChild( Joint* child);		

	// After you compute marker positions for quaternions and eulers add their values
	// into the marker positions
	void		SetRestPose(Transform<> trans) { restpose = trans; }
	Transform<> GetRestPose() { return restpose; }

	void		ClearTransform();
	Transform<> GetReMotionPose(int frame_no);
	void		AddReMotionPose(Transform<> trans) { reMotionPose.push_back(trans); }

	Transform<> GetAbMotionPose(int frame_no);
	void		AddAbMotionPose(Transform<> trans) { abMotionPose.push_back(trans); }

	void		InsertKey(int frame);
};

#endif
