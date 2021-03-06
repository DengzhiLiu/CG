#include <math.h>
#include <fstream>

#include "Clip.h"
#include "Joint.h"
#include "Motion.h"
#include "../Common/utildefines.h"

Clip::Clip()
{
	name   = "";
	interval = 0.0;					// how long each frame lasts for
	num_frame = 0;					// number of frames in animation

	channels.clear();

	mappedJointNames.clear();		// This is a map of joint names
	mappedEffectorNames.clear();	// This is a map of joint names that are effectors
	joints.clear();					// This is a vector of joints
}

Clip::~Clip()
{	
	joints.clear();
	channels.clear();

	mappedJointNames.clear();		// This is a map of joint names
	mappedEffectorNames.clear();	// This is a map of joint names that are effectors
}

Clip::Clip(const Clip& clip)
{
	name=clip.name;
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

float Clip::getInterval()
{
	return interval;
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

bool Clip::readMotionCap(const string filename)
{

#define  BUFFER_LENGTH  1024*4

	MoCap		data;
	MotionType	motion_type;
	std::ifstream  file;
	int		  start_frame;
	char*	  token;
	char	  fileNo[8];
	char      line[ BUFFER_LENGTH ];
	char      separater[] = " :,\t";
	Joint *   par_joint = NULL;
	Joint *   cur_joint = NULL;	
	std::vector< Joint * >   joint_stack;

	int	npos = filename.rfind('.');
	if (npos == -1)		return false;
	string surfix = string(filename, npos+1);
	if (_stricmp(surfix.c_str(), "bvh") == 0)
		motion_type = BVHFILE;
	else if (_stricmp(surfix.c_str(), "skel") == 0)
		motion_type = SKELETONFILE;
	npos = filename.rfind('\\');
	name = string(filename, npos+1);
	string path = string(filename, 0, npos+1);

	file.open( filename.c_str(), ios::in );
	if ( file.is_open() == 0 )  return false; 

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
		if (  ( strcmp( token, "JOINT" ) == 0 )||( strcmp( token, "ROOT" ) == 0 ) )
		{
			// Joint data
			cur_joint = new Joint();			
			cur_joint->setParent( par_joint     )  ;
			cur_joint->setIndex ( joints.size() )  ;
			joints.push_back   ( cur_joint     )  ;

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
			float x, y ,z;	

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

	if (motion_type == BVHFILE) {
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
		file.close();
	}
	else {
		file.getline( line, BUFFER_LENGTH );
		token = strtok( line, separater );
		if ( strcmp( token, "Start" ) != 0 )  goto bvh_error;
		token = strtok( NULL, separater );
		if ( token == NULL )  goto bvh_error;
		start_frame = atoi( token );
		file.close();

		int i, j, k;
		for (j = 0, i = start_frame; j < num_frame; j++, i++) {
			string skelfile = path;
			skelfile.append("ske").append(_itoa(i, fileNo, 10)).append(".txt");
			//string skelfile("ske");
			//skelfile.append(_itoa(i, fileNo, 10)).append(".txt");
			data.readMocapFile(skelfile);
			std::vector<Channel * >::iterator channel_item;
			for ( channel_item = channels.begin(), k = 0; channel_item!=channels.end(); channel_item++, k++ )
			{
				if (MOCAP_BVH[k] < 0)
					(*channel_item)->setData( 0 );
				else
					(*channel_item)->setData( data.mocap_motion[MOCAP_BVH[k]] * BVH_SIGN[k] );
			}
		}
	}

	calJointTransform();
	return true;

bvh_error:
	file.close();	
	return false;
}

void Clip::calJointTransform(void)
{
	for (int i = 0; i < num_frame; i++)
		calJointTransform(joints[0], i);
}

void Clip::calJointTransform(Joint* pjoint, int frame_no)
{
	Vector3 trans;
	Joint* parent = pjoint->getParent();

	if ( pjoint->getParent() == NULL )
		//trans = Vector3(pjoint->getChannel(0)->getData(frame_no) * 0.5, (pjoint->getChannel(1)->getData(frame_no) -pjoint->getChannel(1)->getData(0)) * 0.5, 
		//			pjoint->getChannel(2)->getData(frame_no) * 0.5);
		trans = Vector3(pjoint->getChannel(0)->getData(frame_no) * 0.8, (pjoint->getChannel(1)->getData(frame_no) -pjoint->getChannel(1)->getData(0)), 
			pjoint->getChannel(2)->getData(frame_no) * 0.8);
	else
		trans = pjoint->getOffset();

	//Quaternion 
	Quaternion<> qrot = Quaternion<>(1, Vector3(0, 0, 0));
	for (int i=0; i<pjoint->getChannelNumber(); i++ )
	{
		Channel *  channel = pjoint->getChannel(i);
		if ( channel->getType() == X_ROTATION )
			qrot = qrot * Quaternion<>(Vector3(1, 0, 0), (float)(channel->getData(frame_no)/180.0*PI));
		else if ( channel->getType() == Y_ROTATION )
			qrot = qrot * Quaternion<>(Vector3(0, 1, 0), (float)(channel->getData(frame_no)/180.0*PI));
		else if ( channel->getType() == Z_ROTATION )
			qrot = qrot * Quaternion<>(Vector3(0, 0, 1), (float)(channel->getData(frame_no)/180.0*PI));
	}

	Transform<> transf = Transform<>(qrot, 1, trans);
	//Transform<> transf = Transform<>(qrot, 1, Vector3());
	//transf = transf * Transform<>(Quaternion<>(1, Vector3(0, 0, 0)), 1, trans);
	pjoint->AddReMotionPose(transf);

	if (parent)
		transf = parent->GetAbMotionPose(frame_no) * transf;
	pjoint->AddAbMotionPose(transf);

	for (int i=0; i < pjoint->numOfChildren(); i++)
		calJointTransform( pjoint->getChild(i), frame_no);
}