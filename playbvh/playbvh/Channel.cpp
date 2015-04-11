

#include "Channel.h"

Channel::Channel()
{
	joint = NULL;
	type  = 0;
	data.clear();
}
Channel::~Channel()
{
	if( joint)
		free(joint);
	data.clear();

}
// Copy constructor does everything necessary.
Channel::Channel(const Channel& tempChannel)
{
	joint = tempChannel.joint;
	type  = tempChannel.type;
	data  = tempChannel.data;
}

//Set and get the joint pointer which is the channel belongs to
void Channel::setJointPointer(Joint * tempJoint)
{
	joint = tempJoint;
}
Joint*	Channel::getJointPointer()
{
	return joint;
}

// Sets and gets what type of channel 
void Channel::setType(int type)
{
	this->type=type;
}
int	Channel::getType()
{
	return type;
}

// Sets and gets motion data in frame i
void Channel::setData(float dat)
{
	data.push_back(dat);
}
float Channel::getData(int frameIndex)
{
	return data.at(frameIndex);
}
void Channel::setIndex(int Index)
{
	this->index= Index;
}
int	Channel::getIndex()
{
	return index;
}