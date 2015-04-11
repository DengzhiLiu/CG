#include "StdAfx.h"
#include "OpenGL.h"

float	g_RotateX = 0;
float	g_RotationSpeed = 0.8;

COpenGL::COpenGL(void):
			m_deltaz(0),
			m_object_scale(0)
{
}

// 设置全局变量,绘制函数的句柄
COpenGL	g_OpenGL;

COpenGL::~COpenGL(void)
{
}

BOOL COpenGL::InitializeOpenGL(CDC *pDC)
{
	m_pDC = pDC;
	//首先把DC的象素格式调整为指定的格式，以便后面对DC的使用
	SetupPixelFormat(); 
	//根据DC来创建RC
	m_hRC=wglCreateContext(pDC->GetSafeHdc()); 
	//设置当前的RC，以后的画图操作都画在m_pDC指向的DC上
	wglMakeCurrent(pDC->GetSafeHdc(), m_hRC); 
	//下面可以进行画图操作了
	return TRUE;
}

BOOL COpenGL::SetupPixelFormat()
{
	//初始化象素格式以及选取合适的格式来创建RC
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR), // pfd结构的大小 
		1, // 版本号 
		PFD_DRAW_TO_WINDOW | // 支持在窗口中绘图 
		PFD_SUPPORT_OPENGL | // 支持 OpenGL 
		PFD_DOUBLEBUFFER, // 双缓存模式 
		PFD_TYPE_RGBA, // RGBA 颜色模式 
		24, // 24 位颜色深度 ,color depth
		//::GetDeviceCaps(pDC->GetSafeHdc(), BITSPIXEL),
		0, 0, 0, 0, 0, 0, // 忽略颜色位 
		0, // 没有非透明度缓存 
		0, // 忽略移位位 
		0, // 无累加缓存 
		0, 0, 0, 0, // 忽略累加位 
		32, // 32 位深度缓存 
		0, // 无模板缓存 
		0, // 无辅助缓存 
		PFD_MAIN_PLANE, // 主层 
		0, // 保留 
		0, 0, 0 // 忽略层,可见性和损毁掩模 
	}; 
	int pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if (pixelformat == 0)
	{
		AfxMessageBox("no matched pixelformat!");
		return FALSE;
	}
	//设置指定象素格式
	if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		AfxMessageBox("can't set specified pixelformat!");
		return FALSE;
	}

	return	TRUE;
}

float g_IWLight = 1.0;
float g_IALight = 0.5;
CFVector3 g_LightPos(-5.00, 2.50, 2.500);

BOOL COpenGL::InitGL()
{
	// 纹理等到以后再加
	// change some OpenGL parameter
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
 	glShadeModel(GL_FLAT);

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	//glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	//glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Light values and coordinates
	GLfloat  whiteLight[] = { g_IWLight, g_IWLight, g_IWLight, 1.0f };
	GLfloat  lmodel_ambient[] = { g_IALight, g_IALight, g_IALight, 1.0f };
	GLfloat	 lightPos[] = { g_LightPos.x, g_LightPos.y, g_LightPos.z, 0.0f };
	// Setup and enable light 0
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	//glEnable(GL_COLOR_MATERIAL);
 	static const float Ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
 	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient);
	//throw GetLastError();

	return	TRUE;
}

void COpenGL::SetObjInfo(CFVector3 object_center, float scale)
{
	m_object_center = object_center;
 	m_object_scale = scale;
}

void COpenGL::FocusObject()
{
 	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();
	//	将原点移到模型的中间
 	glTranslatef(-m_object_center.x, -m_object_center.y,
 		-m_object_center.z - m_object_scale * 0.75);
	m_deltaz = -m_object_scale * 0.75;
}

void COpenGL::Set3DProjection(int width, int height)
{
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	// 通知OpenGL用当前设置的背景颜色清除颜色缓存，同时清除深度排序缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// 设置视口变换,默认情况下视口被设置为占据打开窗口的整个像素
	glViewport(0, 0, width, height);
	// 设置透视投影变换
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, width / (double)height, 1e-2, 1e6);
	//gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}

// 设置模型变换
void COpenGL::RotateObject(double x, double y)
{
	double modelview_matrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// 先将模型移至坐标原点，坐标原点是m_object_center
	glTranslated(0.0, 0.0, m_deltaz);
	glRotated(sqrt((double)(x * x + y * y)), 1, 0, 0.0);
	glTranslated(0.0, 0.0, -m_deltaz);
	glMultMatrixd(modelview_matrix);
}

void COpenGL::ZoomObject(double delta)  
{
	if (m_deltaz + delta * m_object_scale > -m_object_scale * 0.5) return;

	double modelview_matrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, delta * m_object_scale);
	m_deltaz += delta * m_object_scale;
	glMultMatrixd(modelview_matrix);
}

void COpenGL::Finish()
{
	wglMakeCurrent(NULL, NULL);
	if (m_hRC != NULL) {
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}

	//删除DC
	if (m_pDC)
		delete m_pDC;
}