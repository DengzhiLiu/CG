#include "raytracer.h"

#include <GL/glut.h>
#include <time.h>

/* a global scene pointer, since we need it in the display() routine */
extern Scene *myScene;

/* an array to store the image before sending it to OpenGL */
unsigned char *texData;
GLuint imageBuffer;

float offset = 100;

/* information for creating and displaying a frame counter */
int frameCount, frame, clockPtr;
clock_t startTime, myClock[5];

/* This frame-display routine cycles through each pixel in the image, generates */
/*    a ray, converts the RGB doubleing point color back to unsigned chars　and	*/
/*    then displays the pixels using glDrawPixels().							*/
void display ( void )
{
	char buf[1024];
    Camera* cam = myScene->GetCamera();
    int width = myScene->GetWidth(), height = myScene->GetHeight();

	clock_t start = clock();
	int idx;
    for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			idx = 4 * (j * width + i);
			Ray r( cam->GetEye(), cam->GenerateRay( (double)(i), (double)(height - j - 1) ) );
			Color color = myScene->RayTrace(r, 0);
			texData[idx] = ConvertToUnsigned(color.red());
			texData[idx+1] = ConvertToUnsigned(color.green());
			texData[idx+2] = ConvertToUnsigned(color.blue());
		}
    }

    glRasterPos2i(0, 0);
    glDrawPixels( width, height, GL_RGBA, GL_UNSIGNED_BYTE, texData );
	unsigned int t = clock() - start;
	glColor3f(0, 0, 0);
	glRasterPos2i(3, 10);
	sprintf( buf, "%.2f s", t / 1000.0 );
	int len = (int) strlen(buf);
	for(int i=0; i<len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buf[i]);
    //DisplayTimer();

	glFlush();
    glutSwapBuffers();
}

/* This routine opens a GLUT/GL window, initializes the block of memory we'll use   */
/*    and checks to make sure we have a valid scene before the display loop starts. */
int OpenGLMain(int argc, char** argv)
{
	if (!myScene)
	{
	std::cout << "OpenGLMain() called, but the scene hasn't been initialized!!\n";
	exit(0);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize( myScene->GetWidth(), myScene->GetHeight() );
	glutInitWindowPosition( 100, 100 );
	glutCreateWindow("Ray Tracing Assignment (512×512)");
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keys );
	glutIdleFunc( idle );
	// RGBA四个通道
	texData = (unsigned char*) malloc( myScene->GetWidth() * myScene->GetHeight() * 4 * sizeof( unsigned char ) );
	glutMainLoop();
	return 0;
}




/************************************************************************
** Stuff below this isn't really important to change, unless you want  **
**    to modify the behavior of the OpenGL window                      **
************************************************************************/

void reshape( int w, int h )
{
	//Camera* c = myScene->GetCamera(); 
    if ( w != myScene->GetWidth() || h != myScene->GetHeight() )
            glutReshapeWindow( myScene->GetWidth(), myScene->GetHeight() );
    glViewport( 0, 0, myScene->GetWidth(), myScene->GetHeight() );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	gluOrtho2D( 0, myScene->GetWidth(), 0, myScene->GetHeight() );
	//gluPerspective(c->fovy, c->screenWidth / c->screenHeight, c->near, c->far);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	//gluLookAt(c->eye.X(), c->eye.Y(), c->eye.Z(), c->at.X(), c->at.Y(), c->at.Z(),
	//	c->up.X(), c->up.Y(), c->up.Z());
    glutPostRedisplay();
}


void DisplayTimer( void )
{
    double val;
    char buf[1024];

    if (frame < 5)
    {
            myClock[frame++] = clock();
            val = 0.0;
    }
    else
    {
            clock_t oldClock = myClock[clockPtr];
            myClock[clockPtr] = clock();
            unsigned int diff = myClock[clockPtr] - oldClock;
            clockPtr = (clockPtr+1) % 5;
			// fps=帧数/(每帧绘制时间 t/1000)
            val = (5*CLOCKS_PER_SEC)/((double)diff);
    }

    /* draw 2D text to the screen */
    glColor3f(1,1,1);
    glRasterPos2i(3,10);
    sprintf( buf, "%.2f fps", val );
    int len = (int) strlen(buf);
    for(int i=0; i<len; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buf[i]);
}

void idle( void )
{
    glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)
{
	Camera* c = myScene->GetCamera();
	switch ( key )
	{
	case 'a':
	case 'A':
		c->MoveLeft(offset);
		break;
	case 'd':
	case 'D':
		c->MoveRight(offset);
		break;
	case 'r':
	case 'R':
		c->TurnRight(10);
		break;
	case 'l':
	case 'L':
		c->TurnLeft(10);
		break;
	case 'w':
	case 'W':
		c->MoveUp(offset);
		break;
	case 'x':
	case 'X':
		c->MoveDown(offset);
		break;
	case 'i':
	case 'I':
		c->ZoomIn(offset);
		break;
	case 'o':
	case 'O':
		c->ZoomOut(offset);
		break;
	case 'Q':
	case 'q':
	case 27:
		exit(0);
		break;
	default:
		break;
	}
    glutSwapBuffers();
	glutPostRedisplay();
}

unsigned char ConvertToUnsigned( double val )
{
    return ( val < 0 ? 0 : (unsigned char)(val*255) );
}

