#include "stdafx.h"
#include "SceneGL.h"
#include "Surface.h"
#include "InterfaceManager.h"
#include <math.h>
#include <string>

using std::string;

CSceneGL::CSceneGL(void)
{
	m_bBlend = false;
	m_bRender = false;
	m_pCamera = new View();
	m_TransMode = Rotation;
	m_DrawMode = DMFill;
}

CSceneGL::~CSceneGL(void)
{
	if (m_pCamera)
		delete m_pCamera;
	m_pCamera = NULL;
}

bool CSceneGL::InitGL()
{
	// use command-line specified CUDA device, otherwise use device with highest Gflops/s
	//if( cutCheckCmdLineFlag(argc, (const char**)argv, "device") )
	//	cutilDeviceInit(argc, argv);
	//else
		cudaSetDevice( cutGetMaxGflopsDeviceId() );

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

	return true;
}

void CSceneGL::SetView()
{
	int width = m_pCamera->GetWidth();
	int height = m_pCamera->GetHeight();
	ElemType fov = m_pCamera->GetFov();
	ElemType znear = m_pCamera->GetNear();
	ElemType zfar = m_pCamera->GetFar();
	Vector3 eye = m_pCamera->GetEye();
	Vector3 at = m_pCamera->GetCenter();
	Vector3 up = m_pCamera->GetUp();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(fov, width / (double)height, znear, zfar);

	if (width > height)
		glOrtho(-width / (double)height, width / (double)height, -1., 1., znear, zfar);
	else
		glOrtho(-1., 1., -height / (double)width, height / (double)width, znear, zfar);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);	// 加了之后模型视图矩阵偏移了(0, 0, -1)	
}

bool CSceneGL::SetupPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR), 
		1, 
		PFD_DRAW_TO_WINDOW |  
		PFD_SUPPORT_OPENGL | 
		PFD_STEREO_DONTCARE|
		PFD_DOUBLEBUFFER, 
		PFD_TYPE_RGBA,
		24, 
		//::GetDeviceCaps(pDC->GetSafeHdc(), BITSPIXEL),
		0, 0, 0, 0, 0, 0, 
		0, 
		0, 
		0, 
		0, 0, 0, 0, 
		32, 
		0, 
		0,
		PFD_MAIN_PLANE, 
		0, 
		0, 0, 0 
	}; 

	int pixelformat = ChoosePixelFormat(m_DC, &pfd);
	if (pixelformat == 0)
	{
		TRACE("No matched pixelformat!");
		return false;
	}

	if (SetPixelFormat(m_DC, pixelformat, &pfd) == false)
	{
		TRACE("Can't set specified pixelformat!");
		return false;
	}

	return	true;
}

bool CSceneGL::InitializeDevice(const HDC& _hdc)
{
	m_DC = _hdc;
	if ( !SetupPixelFormat() )
		return	false;
	m_hRC = wglCreateContext(m_DC); 
	if (m_hRC ==  NULL)
		return false;

	::wglMakeCurrent(m_DC, m_hRC); 

	return true;
}

void CSceneGL::ReSize(int _width, int _height)
{
	m_pCamera->SetView(_width, _height);
}

void CSceneGL::SetCamera(Vector3 _center)
{
	m_pCamera->SetEye(_center + Vector3(0, 0, 2));
	m_pCamera->SetCenter(_center);
}

void CSceneGL::SetTransMode(TransMode _tm)
{
	m_TransMode = _tm;
}

void CSceneGL::SetDrawMode(DrawMode _dm)
{
	m_DrawMode = _dm;
}

void CSceneGL::SetInterface(CInterfaceManager *pInterface)
{
	m_pInterface = pInterface;
}

void CSceneGL::SetBlendState(bool _state)
{
	m_bBlend = _state;
}

Vector3 CSceneGL::GetTransPointClamp(Vector3 p)
{
	Vector3  v;
	ElemType len;

	v[0] = (p[0] - m_pCamera->GetWidth() * 0.5) / (m_pCamera->GetWidth() * 0.5);
	v[1] = (p[1] - m_pCamera->GetHeight() * 0.5) / (m_pCamera->GetHeight() * 0.5);
	len = v[0] * v[0] + v[1] * v[1];
	if( len > 1 ){
		len = sqrt( len );
		v[0] /= len;
		v[1] /= len;
		v[2]  = 0;
	}
	else {
		v[2] = sqrt( 1 - len );
	}

	return v;
}

Vector3 CSceneGL::GetTransPointWrap(Vector3 p)
{
	Vector3  v;
	ElemType len;
	
	v[0] = (p[0] - m_pCamera->GetWidth() * 0.5) / (m_pCamera->GetWidth() * 0.5);
	v[1] = (p[1] - m_pCamera->GetHeight() * 0.5) / (m_pCamera->GetHeight() * 0.5);
	len = v[0] * v[0] + v[1] * v[1];
	if( len > 1 ){
		len = sqrt( len );
		v[0] /= len;
		v[1] /= len;
		len -= 1;
		len -= int(len / 2) * 2;
		v[0] = v[0] * len - v[0];
		v[1] = v[1] * len - v[1];
		len = v[0] * v[0] + v[1] * v[1];
	}

	if (len > 1)
		v[2] = 0;
	else
		v[2] = sqrt(1 - len);

	return v;
}

void CSceneGL::MouseMoveEvent(Vector3 _last, Vector3 _cur)
{
	if (Translation == m_TransMode)
	{
		Vector3 v = (_cur - _last) * 0.002;
		Vector3 trans = m_Transform.GetTrans() + v;
		m_Transform = Transform(m_Transform.GetRot(), m_Transform.GetScale(), trans);
	}
	else if (Rotation == m_TransMode)
	{
		Vector3 p1 = GetTransPointWrap(_last);
		Vector3 p2 = GetTransPointWrap(_cur);
		Vector3 n = p1 % p2;
		float angle = p1 * p2;

		Quaternion qrot = Quaternion(angle, n);
		Quaternion rot = qrot * m_Transform.GetRot();
		m_Transform = Transform(rot, m_Transform.GetScale(), m_Transform.GetTrans());
	}
	else
	{
	}

	PaintGL();
}

void CSceneGL::MouseWheelEvent(ElemType _scale)
{
	m_Transform = m_Transform * Transform(1 - _scale);
	PaintGL();
}

void CSceneGL::ResetModel()
{
	m_Transform = Transform();
	PaintGL();
}

void CSceneGL::DrawBackGround()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	//glColor4f(0.247f, 0.686f, 1.f, 0.f);
	glColor4f(0.f, 0.f, 0.f, 0.f);      glVertex2f(-1,1);
	glColor4f(1.f, 1.f, 1.f, 0.f);		glVertex2f(-1,-1);
	//glColor4f(0.247f, 0.686f, 1.f, 0.f);
	glColor4f(0.f, 0.f, 0.f, 0.f);      glVertex2f(1,1);
	glColor4f(1.f, 1.f, 1.f, 0.f);		glVertex2f(1,-1);
	glEnd();
	glPopAttrib();	
	glClear(GL_DEPTH_BUFFER_BIT);
}

void CSceneGL::PaintGL()
{	
	ElemType TM[16];
	CSurface *pSurface = m_pInterface->GetSurface();

	::wglMakeCurrent(m_DC, m_hRC);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_pCamera->GetWidth(), m_pCamera->GetHeight());
	//DrawBackGround();
	SetView();

	m_Transform.GetTransformMatrix(TM);
	glMultMatrixd(TM);
	if (DMFill == m_DrawMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPushMatrix();
	if (m_bBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glDepthMask(GL_FALSE);
	}

	pSurface->RenderSurfaces();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glPopMatrix();

	if (m_bRender)
	{
		char name[5];
		static int frame = 0;
		_itoa(frame, name, 10);
		string fileName = string(".\\maps\\");
		fileName.append(name);
		fileName.append(".bmp");
		SaveBMP(fileName.c_str());
		frame++;
		m_bRender = !m_bRender;
	}

	glFinish();
	SwapBuffers(wglGetCurrentDC());
}


void CSceneGL::SaveBMP(const char* _fileName)
{
	int width = m_pCamera->GetWidth();
	int height = m_pCamera->GetHeight();

	width -= width % 4;
	GLubyte* pixels = new GLubyte[width * height * 3];
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	FILE *fp;
	fp=fopen(_fileName, "wb");

	BITMAPFILEHEADER bfh; 
	bfh.bfType = 0X4D42;
	bfh.bfSize=54 + (width*24+7)/8*height;
	bfh.bfReserved1=0;
	bfh.bfReserved2=0;
	bfh.bfOffBits=54;
	fwrite(&bfh,14,1,fp); 

	BITMAPINFOHEADER bih;
	bih.biSize=40;
	bih.biWidth=width;
	bih.biHeight=height;
	bih.biPlanes=1;
	bih.biBitCount=24;
	bih.biCompression=BI_RGB;
	bih.biSizeImage=0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	fwrite(&bih,40,1,fp);
	fwrite(pixels,width*height*3,1,fp);

	fclose(fp);
	delete pixels;
}