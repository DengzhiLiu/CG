/**
* $Id: vector.h
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
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#pragma warning(disable: 4786)
#pragma warning(disable: 4788)

#include <map>
#include <vector>

enum  ChannelEnum
{		
	X_ROTATION=0, Y_ROTATION, Z_ROTATION,
	X_POSITION, Y_POSITION, Z_POSITION
};

typedef std::vector<float> motionData;

class Joint;

class Channel
{

private:
	Joint *      joint; // pointer of the joint which is ths channel belongs to
	int	         type;	// Types are enum  ChannelEnum-X_ROTATION, Y_ROTATION, Z_ROTATION,X_POSITION, Y_POSITION, Z_POSITION
	int          index;
	motionData   data;	//data[i] means this channel motion data in frame i	

public:
	Channel();
	~Channel();
	Channel(const Channel& tempChannel);

	//Set and get the joint pointer which is the channel belongs to
	void		setJointPointer(Joint * );
	Joint*		getJointPointer();

	// Sets and gets what type of channel 
	void		setType(int );
	int			getType();

	// Sets and gets motion data in frame i
	void		setData(float dat);
	float		getData(int FrameIndex);

	void		setIndex(int Index);
	int			getIndex();

};

#endif
