#include "Joint.h"



Joint::Joint()
{	
	name   = "";
	parent = NULL;	
	channel_num = 0;

}
Joint::~Joint()
{
	if( parent )
		free( parent );
	
	channels.clear();	
	children.clear();
	posVector.clear();     
	quatVector.clear();	
	transVector.clear();
}
Joint::Joint(const Joint& tempJoint)
{
	type = tempJoint.type;
	name = tempJoint.name;
	parent = tempJoint.parent ;
	channels =tempJoint.channels;
	children = tempJoint.children;
	posVector = tempJoint.posVector;
	quatVector = tempJoint.quatVector;
	transVector = tempJoint.transVector;
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
string Joint::getName()
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
	return this->channels.size();//实际数据个数
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
// 	j->addChild( "some_name" );
// Here is how you get the children:
//	for(int i=0; i< j->numOfChildren(); i++)
//		j->getChild( i );
int	Joint::numOfChildren()
{
	return children.size();
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
void Joint::addPosition(const Vector3 pos)
{
	posVector.push_back(pos);//posVector[0] mesn the joint position at frame 1
}
void Joint::addQuaternion(const Quaternion<> quat)
{
	quatVector.push_back(quat);
}

void Joint::addTransform(const Transform<> trans)
{
	transVector.push_back(trans);
}

// If you want to rescompute quaternion markers call this function to clear the 
// markers and then call add___MarkerPosition again for the quaternions and eulers
void Joint::clearQuaternion()
{
	quatVector.clear();
}
void Joint::clearPosition()
{
	posVector.clear();
}

void Joint::clearTransform()
{
	transVector.clear();
}

// These three functions are used to get marker positions for any joint at a certain
// frame number.
Vector3 Joint::getPosition(int frameNum)
{
	return posVector[frameNum];
}
Quaternion<>	Joint::getQuaternion(int frameNum)
{
	return quatVector[frameNum];
}
Transform<> Joint::getTransform(int framNum)
{
	return transVector[framNum];
}