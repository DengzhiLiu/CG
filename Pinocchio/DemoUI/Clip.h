#include <vector>
#include <map>
#include <string>
using namespace   std;

#include "../Pinocchio/vector.h"
#include "Joint.h"
#include "../Pinocchio/transform.h"

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

typedef std::map<string, Joint *, namesCompare> mappedNames;		// This structure gives us the position of the joint in the joints vector

//Feature vector of a joint <xPos_world,yPos_world,zPos_world,velocity_x,velocity_y,velocity_z>
typedef std::vector<float> featureVector;


class Clip
{
private:
	string			 name;     // usually name is the bvh file name 
	FILE			 *BVHFilePointer ;

	float			interval;				// how long each frame lasts for
	int 			num_frame;			// number of frames in animation

	channelVector   channels;           //all joints channel ,corresponding to the motion data 

	mappedNames		mappedJointNames;		// This is a map of joint names
	mappedNames		mappedEffectorNames;	// This is a map of joint names that are effectors
	JointsVector	joints;					// This is a vector of joints
	vector<vector<Transform<> >> transVector;

   // featureVector   ftrVector;
public:
	Clip();
	~Clip();
	Clip(const Clip& clip);
	
	void		setName(string Name);
	string		getName();

	void	setInterval(float Interval);
	float	getInterval();

	void	setNumFrames(int frame_num);
	int		getNumFrames();

	JointsVector getJoints() { return joints; }
	Joint* getJointByName(string joint_name);
	Joint* getEffectorByName(string Effector_name);

	void  computeTransform(double scale);
	void  computeTransform(Joint* joint, double scale, int frame_no);

	int		readBVH(ifstream &strm);
	void    readBVHjoint();
	//void        computerFeatureVector(int frameNum);

protected:
	

};