#include "Joint.h"

Joint::Joint()
{	
	name   = "";
	used = false;
	parent = NULL;	
	channel_num = 0;

}
Joint::~Joint()
{
	if( parent )
		free( parent );
	
	channels.clear();	
	children.clear();
	reMotionPose.clear();
	abMotionPose.clear();
}
Joint::Joint(const Joint& tempJoint)
{
	type = tempJoint.type;
	used = tempJoint.used;
	name = tempJoint.name;
	parent = tempJoint.parent ;
	channels =tempJoint.channels;
	children = tempJoint.children;
	position = tempJoint.position;
	restpose = tempJoint.restpose;
	reMotionPose = tempJoint.reMotionPose;
	abMotionPose = tempJoint.abMotionPose;
}

// Sets and gets what type of joint the latest addition is.
void Joint::setType(const char* Type)
{
	if(strcmp(Type, "JOINT") == 0)
		type = JOINT;
	else if(strcmp(Type, "EFFECTOR") == 0)
		type = EFFECTOR;
	else if(strcmp(Type, "ROOT") == 0)
		type = ROOT;
	else
		type = NOTYPE;
}
void Joint::setUsed(bool flag)
{
	used = flag;
}
bool Joint::IsUsed()
{
	return used;
}
void Joint::setType(int t)
{
	type = t;
}
int	Joint::getType()
{
	return type;
}
void Joint::setIndex(int Index)
{
	this->index=Index;

}
int Joint::getIndex()
{
	return index;
}
// Sets and gets what the current joint is named.
void Joint::setName( char* Name)
{
	name.assign(Name);	
}

void Joint::setName( std::string Name)
{
	name = Name;
}
std::string Joint::getName()
{
	return name;
}

// Sets and gets the motion data offset.
void Joint::setOffset( Vector3 Offset)
{
	this->offset=Offset;
}
Vector3 Joint::getOffset(void)
{
	return offset;
}

void Joint::setPosition(Vector3 pos)
{
	position = pos;
}
Vector3 Joint::getPosition()
{
	return	position;
}
void Joint::setChannelNumber(int size)
{	
	channel_num=size;
}
int Joint::getChannelNumber()
{
	return channel_num;
}
int	Joint::getCurChannelnumber()
{
	return (int)this->channels.size();
}
// Add a channel name to the list and get a channel name from the list
// The first name is 0 so if you want all the channels loop 
// from i=0 to i<numOfChannels()
void Joint::addChannel( Channel* channnel)
{
	channels.push_back( channnel );
}
Channel* Joint::getChannel(int Index)
{
	return channels[Index];// if this joint is not Hip,then channel[0]=ZRotation,channel[1]=XRotation,channel[2]=YRotation,
}


// Sets and gets the joint's only parent.
void Joint::setParent( Joint* Parent)
{
	this->parent=Parent;
}
Joint*	Joint::getParent()
{
	return this->parent;
}

// Here is how you add a child onto a parents list:
int	Joint::numOfChildren()
{
	return (int)children.size();
}
Joint* Joint::getChild(int index)
{
	return children[index];
}
void Joint::addChild( Joint* child)
{
	children.push_back(child);
}

// After you compute marker positions for quaternions and eulers add their values
// into the marker positions
void Joint::ClearTransform()
{
	reMotionPose.clear();
	abMotionPose.clear();
}

Transform<> Joint::GetReMotionPose(int frame_no)
{
	return reMotionPose[frame_no];
}

Transform<> Joint::GetAbMotionPose(int frame_no)
{
	return	abMotionPose[frame_no];
}

void Joint::InsertKey(int frame)
{
	Transform<> trans1 = GetAbMotionPose(frame);
	Transform<> trans2 = GetAbMotionPose(frame+1);
	Transform<> abmotion = trans1.slerp(trans2, 0.5);

	Transform<> remotion;
	if (parent == NULL)
		remotion = abmotion;
	else
		remotion = parent->GetAbMotionPose(frame + 1).inverse() * abmotion;

	reMotionPose.insert(reMotionPose.begin() + frame + 1, remotion);
	abMotionPose.insert(abMotionPose.begin() + frame + 1, abmotion);
}