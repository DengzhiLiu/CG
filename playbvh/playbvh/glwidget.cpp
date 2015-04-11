#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
: QGLWidget(parent)
{

	xRot = -45*16;
	yRot = 0;
	zRot = 0;
	xTrans = 0;
	zTrans =0;
	camera_distance = 5.0f;

	on_animation = true;
	animation_time = 0.0f;
	frame_no = 0;
	clip = NULL;
	skin = NULL;

	setFocusPolicy( Qt::ClickFocus);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(advanceBVH()));
	timer->start(33);
}


GLWidget::~GLWidget()
{
	makeCurrent();	
}
void GLWidget::advanceBVH()
{
	if (clip && on_animation)
	{
		frame_no++;
		frame_no = frame_no % clip->getNumFrames();
		updateGL();
	}	
}
QSize GLWidget::minimumSizeHint() const
{
	return QSize(640, 640);
}

QSize GLWidget::sizeHint() const
{
	return QSize(640, 640);
}

void GLWidget::initializeGL()
{
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL );

	float  light0_position[] = { 60.0, 60.0, 60.0, 1.0 };
	float  light0_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
	/*float  light0_specular[] = { 0.2, 0.2, 0.2, 1.0 };*/
	float  light0_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_diffuse );
	/*glLightfv( GL_LIGHT0, GL_SPECULAR, light0_specular );*/
	glLightfv( GL_LIGHT0, GL_AMBIENT, light0_ambient );
	glEnable( GL_LIGHT0 );

	glEnable( GL_LIGHTING );

	glEnable( GL_COLOR_MATERIAL );

	glEnable( GL_DEPTH_TEST );

	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );

	glClearColor( 0.5, 0.5, 0.8, 0.0 );
}


void  GLWidget::drawMessage( int line_no, const char * message )
{
	int   i;
	if ( message == NULL )
		return;

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0.0, win_width, win_height, 0.0 );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

	glColor3f( 1.0, 0.0, 0.0 );
	glRasterPos2i( 8, 24 + 18 * line_no );
	for ( i=0; message[i]!='\0'; i++ )
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, message[i] );


	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

void GLWidget::paintGL()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//////////////////////////////////////////////////////////////////////////
	GLfloat M[16];
	glGetFloatv(GL_MODELVIEW_MATRIX,M);
	//////////////////////////////////////////////////////////////////////////
	glTranslatef( 0.0, 0.0, - camera_distance );
	glTranslatef( xTrans, 0.0,0.0);
	glTranslatef( 0.0, 0.0, zTrans );

	glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

	glTranslatef( 0.0, -0.5, 0.0 );	

	//////////////////////////////////////////////////////////////////////////
	//GLfloat M[16];
	glGetFloatv(GL_MODELVIEW_MATRIX,M);
	//////////////////////////////////////////////////////////////////////////

	float  light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
	glLightfv( GL_LIGHT0, GL_POSITION, light0_position );

	float  size = 1.5f;
	int  num_x = 10, num_z = 10;
	double  ox, oz;
	glBegin( GL_QUADS );
	glNormal3d( 0.0, 1.0, 0.0 );
	ox = -(num_x * size) / 2;
	for ( int x=0; x<num_x; x++, ox+=size )
	{
		oz = -(num_z * size) / 2;
		for ( int z=0; z<num_z; z++, oz+=size )
		{
			if ( ((x + z) % 2) == 0 )
				glColor3f( 1.0, 1.0, 1.0 );
			else
				glColor3f( 0.8, 0.8, 0.8 );
			glVertex3d( ox, 0.0, oz );
			glVertex3d( ox, 0.0, oz+size );
			glVertex3d( ox+size, 0.0, oz+size );
			glVertex3d( ox+size, 0.0, oz );
		}
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0,0,0);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(2.0f,0.0f,0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0,1,0);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,2.0f,0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0,0,1.0);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,2.0f);
	glEnd();

	glColor3f( 1.0f, 0.0f, 0.0f );
	if ( clip )
		clip->RenderFigure( frame_no, 0.2f );

	if (skin&&skin->getEnable())	
		skin->RenderSkin(frame_no, 0.2f);	

	/*char  message[ 64 ];
	if ( bvh )
		sprintf( message, "%.2f (%d)", animation_time, frame_no );
	else
		sprintf( message, "Press 'L' key to Load a BVH file" );
	drawMessage( 0, message );*/

	glFlush();//glutSwapBuffers ();
}

void GLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45, (double)w/h, 1, 500 );
	
	win_width = w;
	win_height = h;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(xRot + 8 * dy);
		setYRotation(yRot + 8 * dx);
	} else if (event->buttons() & Qt::RightButton) {
		setXRotation(xRot + 8 * dy);
		setZRotation(zRot + 8 * dx);
	}
	lastPos = event->pos();
}

void  GLWidget::keyPressEvent (QKeyEvent * event )
{
	int key= event->key();
	if ( key == Qt::Key_S )
		on_animation = !on_animation;

	if ( ( key == Qt::Key_N) && !on_animation )
	{
		animation_time += clip->getInterval();
		frame_no ++;
		frame_no = frame_no % clip->getNumFrames();
	}

	if ( ( key == Qt::Key_P ) && !on_animation && ( frame_no > 0 ) && clip )
	{
		animation_time -= clip->getInterval();
		frame_no --;
		frame_no = frame_no % clip->getNumFrames();
	}

	if ( key == Qt::Key_R )
	{
		animation_time = 0.0f;
		frame_no = 0;
	}
	if (key == Qt::Key_Left)
		xTrans+=0.2;
	if (key ==Qt::Key_Right)
		xTrans-=0.2;	
	if ( key == Qt::Key_Up)
		zTrans+=0.2;
	if( key ==Qt::Key_Down)
		zTrans-=0.2;

//////////////////////////////////////////////////////////////////////////
//	/*if ( key == Qt::Key_L )
//	{
//#ifdef  WIN32
//		const int  file_name_len = 256;
//		char  file_name[ file_name_len ] = "";
//
//		OPENFILENAME	open_file;
//		memset( &open_file, 0, sizeof(OPENFILENAME) );
//		open_file.lStructSize = sizeof(OPENFILENAME);
//		open_file.hwndOwner = NULL;
//		open_file.lpstrFilter = "BVH Motion Data (*.bvh)\0*.bvh\0All (*.*)\0*.*\0";
//		open_file.nFilterIndex = 1;
//		open_file.lpstrFile = file_name;
//		open_file.nMaxFile = file_name_len;
//		open_file.lpstrTitle = "Select a BVH file";
//		open_file.lpstrDefExt = "bvh";
//		open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
//
//
//		BOOL  ret = GetOpenFileName( &open_file );
//
//		if( ret )
//		{
//			if ( bvh )
//				delete  bvh;
//			bvh = new BVH( file_name );
//
//			if ( !bvh->IsLoadSuccess() )
//			{
//				delete  bvh;
//				bvh = NULL;
//			}
//
//			animation_time = 0.0f;
//			frame_no = 0;
//		}
//#endif
//	}*/
	//////////////////////////////////////////////////////////////////////////

	//glutPostRedisplay();
	updateGL();
}

void GLWidget::wheelEvent ( QWheelEvent * event )
{
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	camera_distance += numSteps*0.2;
	updateGL();
}
void GLWidget::setXRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != xRot) {
		xRot = angle;
		emit xRotationChanged(angle);
		updateGL();
	}
}
//! [5]

void GLWidget::setYRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != yRot) {
		yRot = angle;
		emit yRotationChanged(angle);
		updateGL();
	}
}

void GLWidget::setZRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != zRot) {
		zRot = angle;
		emit zRotationChanged(angle);
		updateGL();
	}
}

void GLWidget::normalizeAngle(int *angle)
{
	while (*angle < 0)
		*angle += 360 * 16;
	while (*angle > 360 * 16)
		*angle -= 360 * 16;
}
