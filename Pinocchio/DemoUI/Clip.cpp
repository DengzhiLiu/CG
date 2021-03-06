
#include <math.h>
#include <windows.h>
#include <fstream>
#include <stdlib.h>

#include "Clip.h"
#include "Joint.h"


Clip::Clip()
{
	name   = "";
	BVHFilePointer = NULL ;
	interval = 0.0;				// how long each frame lasts for
	num_frame = 0;			// number of frames in animation

	channels.clear();

	mappedJointNames.clear();		// This is a map of joint names
	mappedEffectorNames.clear();	// This is a map of joint names that are effectors
	joints.clear();					// This is a vector of joints
}
Clip::~Clip()
{	
	if (BVHFilePointer)
		free(BVHFilePointer);
		
	joints.clear();
	channels.clear();
	
	mappedJointNames.clear();		// This is a map of joint names
	mappedEffectorNames.clear();	// This is a map of joint names that are effectors
}
Clip::Clip(const Clip& clip)
{
	name=clip.name;	
	BVHFilePointer=clip.BVHFilePointer;
	interval=clip.interval;
	num_frame=clip.num_frame;
	channels=clip.channels;
	mappedJointNames=clip.mappedJointNames;
	mappedEffectorNames=clip.mappedEffectorNames;
	joints=clip.joints;
}
void Clip::setName(string Name)
{
	name=Name;
}
string  Clip::getName()
{
	return name;
}
void Clip::setInterval(float Interval)
{
	this->interval=Interval;
}
float Clip::getInterval()
{
	return interval;
}

void Clip::setNumFrames(int frame_num)
{
	this->num_frame= frame_num;
}
int Clip::getNumFrames()
{
	return num_frame;
}

Joint* Clip::getJointByName(string joint_name)
{
	return mappedJointNames[joint_name];
}
Joint* Clip::getEffectorByName(string Effector_name)
{
	return mappedEffectorNames[Effector_name];

}

int Clip::readBVH(ifstream &file)
{
	
#define  BUFFER_LENGTH  1024*4
	
	//std::ifstream  file;
	char      line[ BUFFER_LENGTH ];
	char *    token;
	char      separater[] = " :,\t";
	std::vector< Joint * >   joint_stack;
	Joint *   par_joint = NULL;
	Joint *   cur_joint = NULL;		

	//const char *  mn_first = bvh_file_name;
	//const char *  mn_last = bvh_file_name + strlen( bvh_file_name );
	//if ( strrchr( bvh_file_name, '\\' ) != NULL )
	//	mn_first = strrchr( bvh_file_name, '\\' ) + 1;
	//else if ( strrchr( bvh_file_name, '/' ) != NULL )
	//	mn_first = strrchr( bvh_file_name, '/' ) + 1;
	//if ( strrchr( bvh_file_name, '.' ) != NULL )
	//	mn_last = strrchr( bvh_file_name, '.' );
	//if ( mn_last < mn_first )
	//	mn_last = bvh_file_name + strlen( bvh_file_name );
	//string motion_name;
	//name.assign( mn_first, mn_last );	
	//
	//file.open( bvh_file_name, ios::in );
	//if ( file.is_open() == 0 )  return 0; 

	while ( ! file.eof() )
	{
		// Terminates abnormally if it comes to the end of the file
		if ( file.eof() )  goto bvh_error;

		// Read a line, get the first word
		file.getline( line, BUFFER_LENGTH );
		token = strtok( line, separater );

		// If the blank line next to line
		if ( token == NULL )  continue;

		// Start of block joints
		if ( strcmp( token, "{" ) == 0 )	
			continue;
	
		// End of the block joint
		if ( strcmp( token, "}" ) == 0 )
		{			
			par_joint = cur_joint->getParent();
			cur_joint = par_joint;
		
			continue;
		}

		//Start of the joint information
		if (  ( strcmp( token, "JOINT" ) == 0 )||( strcmp( token, "ROOT" ) == 0 )
			 )
		{
			// Joint data
			cur_joint = new Joint();			
			cur_joint->setParent( par_joint     )  ;
			cur_joint->setIndex ( joints.size() )  ;
			joints.push_back   ( cur_joint     )  ;
			// 在这里是不是要清空curJoint的信息
			
			if (( strcmp( token, "JOINT" ) == 0 ))			
				cur_joint->setType("JOINT");			
			else
				cur_joint->setType("ROOT");			
			
			if ( par_joint )
				par_joint->addChild(cur_joint);

			// Loading of articular
			token = strtok( NULL, "" );
			while ( *token == ' ' )  token ++;
			cur_joint->setName( token );

			// Added to the index
			mappedJointNames[ cur_joint->getName() ] = cur_joint;
			continue;
		}

		// Start the terminal information
		if ( ( strcmp( token, "End" ) == 0 ) )
		{
			cur_joint = new Joint();			
			cur_joint->setParent(par_joint)  ;	
			cur_joint->setIndex ( joints.size() )  ;
			joints.push_back( cur_joint );
			cur_joint->setType("EFFECTOR");	

			if ( par_joint )
				par_joint->addChild( cur_joint );		

			// Create my name with the word Effector before the name of my parents name
			char* newBuffer = new char[ 9 + (strlen(par_joint->getName().c_str()))];
			strcpy(newBuffer, "Effector");
			strcat(newBuffer,par_joint->getName().c_str() );
			cur_joint->setName(newBuffer);			
			delete[] newBuffer ;

			mappedJointNames[ cur_joint->getName() ] = cur_joint;
		}

		// Joint position or offset information end
		if ( strcmp( token, "OFFSET" ) == 0 )
		{
			double    x, y ,z;	

			//Coordinates reads	
			token = strtok( NULL, separater );
			x = token ? atof( token ) : 0.0;
			token = strtok( NULL, separater );
			y = token ? atof( token ) : 0.0;
			token = strtok( NULL, separater );
			z = token ? atof( token ) : 0.0;
			
			cur_joint->setOffset(Vector3(x,y,z));			
	
			continue;
		}

		// Channel information in a joint
		if ( strcmp( token, "CHANNELS" ) == 0 )
		{
			//Reads the number of channels
			token = strtok( NULL, separater );
			cur_joint->setChannelNumber( token ? atoi( token ) : 0 );

			// Loading Channel Info
			for (int i=0; i<cur_joint->getChannelNumber(); i++ )
			{
				// Create Channel
				Channel *  channel = new Channel();
				channel->setJointPointer(cur_joint);				
				channel->setIndex(channels.size());		
				// 在识别channel的过程中用一个channels存储了所有的channel以方便
				// 后面对捕获运动数据的存取
				channels.push_back( channel );
				cur_joint->addChannel( channel );

				// Determine the type of channel
				token = strtok( NULL, separater );
				if ( strcmp( token, "Xrotation" ) == 0 )
					channel->setType(X_ROTATION);
				else if ( strcmp( token, "Yrotation" ) == 0 )
					channel->setType(Y_ROTATION);
				else if ( strcmp( token, "Zrotation" ) == 0 )
					channel->setType(Z_ROTATION);
				else if ( strcmp( token, "Xposition" ) == 0 )
					channel->setType(X_POSITION);
				else if ( strcmp( token, "Yposition" ) == 0 )
					channel->setType(Y_POSITION);
				else if ( strcmp( token, "Zposition" ) == 0 )
					channel->setType(Z_POSITION);
			}
			par_joint = cur_joint;
			continue;
		}
	
		if ( strcmp( token, "MOTION" ) == 0 )
			break;
	}

	file.getline( line, BUFFER_LENGTH );
	token = strtok( line, separater );
	if ( strcmp( token, "Frames" ) != 0 )  goto bvh_error;
	token = strtok( NULL, separater );
	if ( token == NULL )  goto bvh_error;
	num_frame = atoi( token );

	file.getline( line, BUFFER_LENGTH );
	token = strtok( line, ":" );
	if ( strcmp( token, "Frame Time" ) != 0 )  goto bvh_error;
	token = strtok( NULL, separater );
	if ( token == NULL )  goto bvh_error;
	interval = atof( token );

	//read motion data 
	
	for (int i=0; i<num_frame; i++ )
	{
		file.getline( line, BUFFER_LENGTH );
		token = strtok( line, separater );

		std::vector<Channel * >::iterator channel_item;
		for ( channel_item = channels.begin();channel_item!=channels.end();channel_item++ )
		{
			if ( token == NULL )
				goto bvh_error;
			(*channel_item)->setData( atof( token )) ;
			token = strtok( NULL, separater );
		}
	}
	computeTransform(1.);
	file.close();

	return 1;

bvh_error:
	file.close();	
	return 0;
}

void Clip::computeTransform(double scale)
{
	int frame_num = getNumFrames();
	for (int frame_no = 0; frame_no < frame_num; ++frame_no)
		computeTransform(joints[0], scale, frame_no);

}

void Clip::computeTransform(Joint* joint, double scale, int frame_no)
{
	Vector3 trans;
	if ( joint->getParent() == NULL )
		trans = Vector3(joint->getChannel(0)->getData(frame_no), joint->getChannel(1)->getData(frame_no), joint->getChannel(2)->getData(frame_no));
	else
		trans = joint->getOffset();

	//trans = trans*scale;

	//Quaternion 
	Quaternion<> qrot = Quaternion<>(1, Vector3(0, 0, 0));
	for (int i=0; i<joint->getChannelNumber(); i++ )
	{
		Channel *  channel = joint->getChannel(i);
		if ( channel->getType() == X_ROTATION )
			qrot = qrot * Quaternion<>(Vector3(1, 0, 0), channel->getData(frame_no)/180.0*M_PI);
		else if ( channel->getType() == Y_ROTATION )
			qrot = qrot * Quaternion<>(Vector3(0, 1, 0), channel->getData(frame_no)/180.0*M_PI);
		else if ( channel->getType() == Z_ROTATION )
			qrot = qrot * Quaternion<>(Vector3(0, 0, 1), channel->getData(frame_no)/180.0*M_PI);
	}
	//qrot.normalize();

	Transform<> cur = Transform<>(qrot, 1., trans);
	joint->addTransform(cur);

	for (int i=0; i < joint->numOfChildren(); i++)
		computeTransform( joint->getChild(i), scale, frame_no);

}

// End of BVH.cpp
