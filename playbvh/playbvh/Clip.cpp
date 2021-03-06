
//
//#include <mbstring.h>
//#include <string.h>
//#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <fstream>

#include "Clip.h"
#include "Joint.h"

#include <gl/glut.h>
//#include <QDebug>


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
std::string  Clip::getName()
{
	return name;
}
void Clip::setInterval(float Interval)
{
	interval=Interval;
}
float Clip::getInterval()
{
	return interval;
}

void Clip::setNumFrames(int frame_num)
{
	num_frame= frame_num;
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

int Clip::readBVH(const char* bvh_file_name)
{

#define  BUFFER_LENGTH  1024*4

	std::ifstream  file;
	char      line[ BUFFER_LENGTH ];
	char *    token;
	char      separater[] = " :,\t";
	std::vector< Joint * >   joint_stack;
	Joint *   par_joint = NULL;
	Joint *   cur_joint = NULL;		

	const char *  mn_first = bvh_file_name;
	const char *  mn_last = bvh_file_name + strlen( bvh_file_name );
	if ( strrchr( bvh_file_name, '\\' ) != NULL )
		mn_first = strrchr( bvh_file_name, '\\' ) + 1;
	else if ( strrchr( bvh_file_name, '/' ) != NULL )
		mn_first = strrchr( bvh_file_name, '/' ) + 1;
	if ( strrchr( bvh_file_name, '.' ) != NULL )
		mn_last = strrchr( bvh_file_name, '.' );
	if ( mn_last < mn_first )
		mn_last = bvh_file_name + strlen( bvh_file_name );
	std::string motion_name;
	name.assign( mn_first, mn_last );	

	file.open( bvh_file_name, ios::in );
	if ( file.is_open() == 0 )  return 0; 

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

	file.close();

	glPushMatrix();
	glLoadIdentity();
	calJointsPositionAtFrame(joints[0],0);
	glPopMatrix();

	return 1;

bvh_error:
	file.close();	
	return 0;
}

void Clip::calJointsPositionAtFrame(Joint* joint,int frame_no)
{
	GLfloat M[16];
	glPushMatrix();

	Vector3 offset;
	if ( joint->getParent() == NULL )	
	{
		offset.x = joint->getChannel(0)->getData(frame_no);
		offset.y = joint->getChannel(1)->getData(frame_no);
		offset.z = joint->getChannel(2)->getData(frame_no);
		glTranslatef( offset.x, offset.y, offset.z );
	}
		
	else
	{
		offset=joint->getOffset();
		glTranslatef( offset.x, offset.y,offset.z  );
	}

	for (int i=0; i<joint->getChannelNumber(); i++ )
	{
		Channel *  channel = joint->getChannel(i);
		if ( channel->getType() == X_ROTATION )		
			glRotatef( channel->getData(frame_no), 1.0f, 0.0f, 0.0f );
		else if ( channel->getType() == Y_ROTATION )
			glRotatef( channel->getData(frame_no), 0.0f, 1.0f, 0.0f );
		else if ( channel->getType() == Z_ROTATION )
			glRotatef( channel->getData(frame_no), 0.0f, 0.0f, 1.0f );		
	}

	glGetFloatv(GL_MODELVIEW_MATRIX,M);
	//Vector3 pos=Vector3(M[12],M[13],M[14]);
	//vector3_vector->push_back(pos);	
	/*qDebug("%s\t%f\t%f\t%f\t%f\t%f\t%f ",joint->getName().c_str(),M[12], -M[14] ,M[13] , offset.x, -offset.z,offset.y );*/

	for (int i=0; i<joint->numOfChildren(); i++ )	
		calJointsPositionAtFrame( joint->getChild(i), frame_no );

	glPopMatrix();
}

void Clip::RenderFigure( int frame_no, float scale )
{
	RenderFigure( joints[0], frame_no, scale );
}

void  Clip::RenderFigure( Joint * joint, int frame_no, float scale )
{
	glPushMatrix();

	if ( joint->getParent() == NULL )
	{
		Vector3 offset;
		offset.x = joint->getChannel(0)->getData(frame_no)*scale;
		offset.y = joint->getChannel(1)->getData(frame_no)*scale;
		offset.z = joint->getChannel(2)->getData(frame_no)*scale;
		glTranslatef( offset.x, offset.y, offset.z );
	}

	else
	{
		Vector3 offset =joint->getOffset();
		glTranslatef( offset.x*scale, offset.y*scale,offset.z*scale  );
	}

	for (int i=0; i<joint->getChannelNumber(); i++ )
	{
		Channel *  channel = joint->getChannel(i);
		if ( channel->getType() == X_ROTATION )		
			glRotatef( channel->getData(frame_no), 1.0f, 0.0f, 0.0f );
		else if ( channel->getType() == Y_ROTATION )
			glRotatef( channel->getData(frame_no), 0.0f, 1.0f, 0.0f );
		else if ( channel->getType() == Z_ROTATION )
			glRotatef( channel->getData(frame_no), 0.0f, 0.0f, 1.0f );		
	}

	for (int i=0; i<joint->numOfChildren(); i++ )
	{
		Joint *  child = joint->getChild(i);
		Vector3 offset = child->getOffset();

		//绘制当前关节，采用当前关节的局部坐标系		
		RenderJoint();

		//在当前关节和其子关节之间绘制一条线段，采用当前关节的局部坐标系
		RenderBone( offset.x * scale, offset.y * scale, offset.z * scale );
	}

	if (joint->numOfChildren() == 0)
	{
		//绘制当前关节，采用当前关节的局部坐标系		
		RenderJoint();	
	}

	for (int i=0; i<joint->numOfChildren(); i++ )	
		RenderFigure( joint->getChild(i), frame_no, scale );	

	glPopMatrix();
}
//绘制当前关节，采用当前关节的局部坐标系		
void Clip::RenderJoint()
{
	glPushMatrix();

	glColor3f(1,1,0);
	glutSolidSphere(0.05,16,16);

	glPopMatrix();
}

//在当前关节和其子关节之间绘制一条线段，采用当前关节的局部坐标系
void  Clip::RenderBone( float x1, float y1, float z1 )
{
	glPushMatrix();

	glColor3f(1,0,1);
	glBegin(GL_LINES);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(x1,y1,z1);
	glEnd();

	glPopMatrix();
}