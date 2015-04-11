#include "Render.h"
#include "Surface.h"
#include "InterfaceManager.h"
#include <cutil_inline.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#pragma comment(lib, "glew32")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

int width = 800, height = 600;
Vector3 lastPos = Vector3(-1000, -1000, -1000);

CSceneGL Scene;
CInterfaceManager InterManager;

bool InitGL(void) 
{
	// initialize necessary OpenGL extensions
	glewInit();
	if (! glewIsSupported("GL_VERSION_2_0 "))
	{
			fprintf(stderr, "ERROR: Support for necessary OpenGL extensions missing.");
			fflush(stderr);
			return false;
	}

	// default initialization
	glClearColor(0.1, 0.2, 0.3, 1.0);
	glEnable(GL_DEPTH_TEST);

	// good old-fashioned fixed function lighting
	float black[] = { 0.0, 0.0, 0.0, 1.0 };
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	float ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	float diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
	float lightPos[] = { 0.0, 0.0, 1.0, 0.0 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);

	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	//glEnable( GL_DEPTH_TEST );
	//glDisable( GL_ALPHA_TEST );
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glutReportErrors();

	InterManager.SetCurScene(&Scene);
	Scene.SetInterface(&InterManager);

	return true;
}

void display(void)
{
	// needed clear depth buffer bit, depth buffer bit is enabled
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Scene.PaintGL();
	glFlush ();
	glutSwapBuffers();
}

void reshape (int w, int h)
{
	width = w;
	height = h;
	Scene.ReSize(w, h);
}

void Idle(void)
{

}

void keyboard(unsigned char key, int x, int y)
{
	static bool drawFill = true;
	switch (key) {
		case 'a':
		case 'A':
			Scene.SetTransMode(CSceneGL::Rotation);
			break;
		case 't':
		case 'T':
			Scene.SetTransMode(CSceneGL::Translation);
			break;
		case 's':
		case 'S':
			Scene.SetTransMode(CSceneGL::Scale);
			break;
		case 'r':
		case 'R':
			Scene.ResetModel();
			break;
		case 'b':
		case 'B':
			Scene.SetBlendColor();
			break;
		case 'w':
		case 'W':
			drawFill = !drawFill;
			if (drawFill)
				Scene.SetDrawMode(CSceneGL::DMFill);
			else
				Scene.SetDrawMode(CSceneGL::DMWire);
			break;
		case 'm':
		case 'M':
			Scene.SetRenderState();
			break;
	  case 27:
		  exit(0);
		  break;
	}

	glutPostRedisplay();
}

void mainMenu(int i)
{
	keyboard((unsigned char) i, 0, 0);
}

void initMenus()
{
	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Toggle animation [ ]", ' ');
	glutAddMenuEntry("Reset [r]", 'r');
	glutAddMenuEntry("Rotation [a]", 'a');
	glutAddMenuEntry("Translation [t]", 't');
	glutAddMenuEntry("Enable Blend [b]", 'b');
	glutAddMenuEntry("Toggle wireframe [w]", 'w');
	glutAddMenuEntry("Quit (esc)", '\033');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void mouse_button(int button, int state, int x, int y)
{
	Vector3 pos = Vector3(x, height - y, 0);
	lastPos = pos;

	if (GLUT_DOWN == state)
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			if (CSceneGL::Translation != Scene.GetTransMode())
				Scene.SetTransMode(CSceneGL::Rotation);
			break;
		case GLUT_MIDDLE_BUTTON:
			Scene.SetTransMode(CSceneGL::Scale);
			break;
		//case GLUT_RIGHT_BUTTON:
		//	Scene.SetTransMode(CSceneGL::Translation);
		//	break;
		default:
			break;
		}
	}

	glutPostRedisplay();
}

void mouse_motion(int x, int y)
{
	Vector3 pos = Vector3(x, height - y, 0);

	if (lastPos[2] == -1000)
	{
		lastPos = pos;
		return;
	}

	Scene.MouseMoveEvent(lastPos, pos);
	lastPos = pos;
	glutPostRedisplay();
}

bool ReadRecord()
{
	if (false == InterManager.LoadData())
	{
		printf("Load Data Failure!\n");
		return false;
	}

	return true;
}

void Process()
{
	InterManager.Process();
}

int main(int argc, char** argv)
{
	// use command-line specified CUDA device, otherwise use device with highest Gflops/s
	if( cutCheckCmdLineFlag(argc, (const char**)argv, "device") )
		cutilDeviceInit(argc, argv);
	else
		cudaSetDevice( cutGetMaxGflopsDeviceId() );

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//glutInitDisplayMode (GL_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Surface Render");
	Idle();
	if (false == InitGL())
		return -1;

	initMenus();
	ReadRecord();
	Process();

	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_motion);
	glutMainLoop();
	return 0;
}