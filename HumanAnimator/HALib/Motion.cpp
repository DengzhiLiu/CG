#include "Motion.h"

#include <fstream>
using namespace std;

int MOCAP_BVH[42] = { 0, 2, 1,			// root position
					  5, 4, 3,			// root rotation
					  8, 7, 6,			// left hip
					  -1, -1, 9,		// left knee
					  12, 11, 10,		// right hip
					  -1, -1, 13,		// right knee 
					  14, -1, -1,		// chest
					  25, 24, 23,		// neck
					  14, -1, -1,		// left collar
					  17, 16, 15,		// left shoulder
					  18, -1, -1,		// left elbow
					  14, -1, -1,		// right collar
					  21, 20, 19,		// right shoulder
					  22, -1, -1		// right elbow
					};

// sign for each joint
int BVH_SIGN[42] = { 1, 1, -1,  1, -1, 1,  1, -1, 1,  1, -1, 1,  1, -1, 1,  1, -1, 1,  1, -1, 1,  1, -1, 1, \
					1, -1, 1,  1, -1, 1,  1, -1, 1,  1, -1, 1,  1, -1, 1,  1, -1, 1 };

MoCap::MoCap()
{

}

MoCap::~MoCap()
{

};

bool MoCap::readMocapFile(string filename)
{
	ifstream fin;
	fin.open(filename.c_str());
	if ( fin.is_open() == 0 )  
		return false;

	//	read motion information
	for(int i=0; i < MOCAP_DOF_NUM; i++)
	{
		fin >> mocap_motion[i];
	}

	//	read bone informantion
	for(int i = 0; i < MOCAP_JOINT_NUM; i++)
	{
		fin >> mocap_length[i];
	}

	fin.close();
	return true;
}