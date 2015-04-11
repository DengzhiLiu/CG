#ifndef _CLIP_H_
#define  _CLIP_H_

#include <vector>
#include <map>
#include <string>
#include "Vector3.h"
using namespace   std;

#include "Joint.h"

// This is the function used for comparing names in the stl map
struct namesCompare
{
	bool operator()(const string s1, const string s2) const
	{
		return (strcmp(s1.c_str(),s2.c_str()) < 0);
	}

	/*bool operator()(const char* s1, const char* s2) const
	{
	return (strcmp(s1, s2) < 0);
	}*/
};
typedef std::vector<Joint*> JointsVector;						// This structure will hold our motionData

typedef std::map<std::string, Joint *, namesCompare> mappedNames;		// This structure gives us the position of the joint in the joints vector

class Clip
{
	friend class Clip2;
	friend class Clip_List;
	friend class Syn_Clip;
private:
	string			 name;     // usually name is the bvh file name 
	FILE			 *BVHFilePointer ;

	float			interval;				// how long each frame lasts for
	int 			num_frame;			// number of frames in animation



public:
	Clip();
	~Clip();
	Clip(const Clip& clip);

	channelVector   channels;           //all joints channel ,corresponding to the motion data 

	mappedNames		mappedJointNames;		// This is a map of joint names
	mappedNames		mappedEffectorNames;	// This is a map of joint names that are effectors
	JointsVector	joints;					// This is a vector of joints

	void		setName(string Name);
	string		getName();

	void	setInterval(float Interval);
	float	getInterval();

	void	setNumFrames(int frame_num);
	int		getNumFrames();

	Joint* getJointByName(string joint_name);
	Joint* getEffectorByName(string Effector_name);
	void calJointsPositionAtFrame(Joint* joint,int frame_no);

	int		readBVH(const char* filename);
	void    readBVHjoint();

	void  RenderFigure( int frame_no, float scale );
	void  RenderFigure( Joint * joint, int frame_no, float scale );
	void  RenderBone( float x1, float y1, float z1 );	
	void  RenderJoint();

protected:


};

#endif //_CLIP_H_