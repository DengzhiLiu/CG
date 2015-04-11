
#ifndef _JOINT_H_
#define _JOINT_H_

#pragma warning(disable: 4786)
#pragma warning(disable: 4788)

/****************************************************************************************
*	joint.h
*
*	Each joint holds information like:
*			name
*			parent's name
*			all its children's name
*			rotation order
*			lenght of joint (this is the offset)
*			channel names (rotateX, rotateY, rotateZ, translateX, ......)
*			frame data for those channels (these are the euler angles)
*			quaternion data for each channel
*			marker data at every frame for original, euler and quaternions
*
*	Any time you are required to give the values frameNum to a function in this class
*   you will think of 1 as the first frame and you can go to <= total number of frames
****************************************************************************************/

#include <map>
#include <vector>
#include <deque>

#include "../Pinocchio/transform.h"
#include "../Pinocchio/vector.h"
#include "Channel.h"
using namespace std;

// This holds the jointType constant, to use:
enum JOINT_TYPE { NOTYPE=-1, ROOT, JOINT, EFFECTOR};

// This holds the names of a joint's children
typedef std::vector<Joint*> childVector;

// This hold the joints CHANNELS usually 3 sometimes 6, 
// but can be any number
typedef std::vector<Channel *> channelVector;

// This holds a marker position (3 float vector)
typedef Vector3 markerPosition;

// This holds a markerPosition for every frame.  
// NOTE: marker at frame 1 = postionVector[0];
typedef std::vector<Vector3> postionVector;

// This holds a quaternion
typedef std::vector<Quaternion<> > quaternionVector;

typedef std::vector<Transform<> > transformVector;

extern class Clip;

class Joint
{
	//friend class Clip;
private:
	int					type;				// Types are ROOT, JOINT, EFFECTOR, and NOTYPE
	int					index;
	int					channel_num;
	string			    name;	
	Vector3      		offset;				// representing the length of the joint

	Joint*				parent;	

	channelVector		channels;		    // STL vector holding the channel names for the joint
	childVector			children;			// STL vector of char* to children names	
	postionVector		posVector;          // Vector pos=posVector[0] means this joint position of frame 1 
	quaternionVector	quatVector;		    // Quaternion quat = quatVector[0] means this joint rotation matrix of frame 1 as to it's parent
	transformVector		transVector;
	
public:	

	Joint();
	~Joint();
	Joint(const Joint& tempJoint);

	// Sets and gets what type of joint the latest addition is.
	void		setType(const char* Type);
	int			getType() ;

	void		setIndex(int Index);
	int         getIndex();

	// Sets and gets what the current joint is named.
	void		setName( char* Name);
	string		getName();

	// Sets and gets the motion data offset.
	void		setOffset( Vector3 offset);
	Vector3		getOffset();	
	
	// Sets and gets the joint's only parent.
	void		setParent( Joint* Parent);	
	Joint*		getParent();	

	// Add a channel name to the list and get a channel name from the list
	// The first name is 0 so if you want all the channels loop 
	// from i=0 to i<numOfChannels()
	void		setChannelNumber(int size);
	void		addChannel( Channel* channel);
	Channel*	getChannel(int index);
	int			getChannelNumber();
	int			getCurChannelnumber();

	// Here is how you add a child onto a parents list:
	// 	j->addChild( "some_name" );
	// Here is how you get the children:
	//	for(int i=0; i< j->numOfChildren(); i++)
	//		j->getChild( i );
	int		numOfChildren();
	Joint*	getChild(int index);
	void	addChild( Joint* child);		

	// After you compute marker positions for quaternions and eulers add their values
	// into the marker positions
	void	addPosition(const Vector3 pos);
	void	addQuaternion(const Quaternion<> quat);
	void	addTransform(const Transform<> trans);

	// These three functions are used to get marker positions for any joint at a certain
	// frame number.
	Vector3		getPosition(int frameNum);	
	Quaternion<>	getQuaternion(int frameNum);
	Transform<>	getTransform(int frameNum);

	// If you want to rescompute quaternion markers call this function to clear the 
	// markers and then call add___MarkerPosition again for the quaternions and eulers
	void	clearQuaternion();
	void	clearPosition();	
	void	clearTransform();
};

#endif
