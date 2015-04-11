#include "StdAfx.h"
#include "OpenGL.h"

float	g_RotateX = 0;
float	g_RotationSpeed = 0.8;

COpenGL::COpenGL(void):
			m_deltaz(0),
			m_object_scale(0)
{
}

// ����ȫ�ֱ���,���ƺ����ľ��
COpenGL	g_OpenGL;

COpenGL::~COpenGL(void)
{
}

BOOL COpenGL::InitializeOpenGL(CDC *pDC)
{
	m_pDC = pDC;
	//���Ȱ�DC�����ظ�ʽ����Ϊָ���ĸ�ʽ���Ա�����DC��ʹ��
	SetupPixelFormat(); 
	//����DC������RC
	m_hRC=wglCreateContext(pDC->GetSafeHdc()); 
	//���õ�ǰ��RC���Ժ�Ļ�ͼ����������m_pDCָ���DC��
	wglMakeCurrent(pDC->GetSafeHdc(), m_hRC); 
	//������Խ��л�ͼ������
	return TRUE;
}

BOOL COpenGL::SetupPixelFormat()
{
	//��ʼ�����ظ�ʽ�Լ�ѡȡ���ʵĸ�ʽ������RC
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR), // pfd�ṹ�Ĵ�С 
		1, // �汾�� 
		PFD_DRAW_TO_WINDOW | // ֧���ڴ����л�ͼ 
		PFD_SUPPORT_OPENGL | // ֧�� OpenGL 
		PFD_DOUBLEBUFFER, // ˫����ģʽ 
		PFD_TYPE_RGBA, // RGBA ��ɫģʽ 
		24, // 24 λ��ɫ��� ,color depth
		//::GetDeviceCaps(pDC->GetSafeHdc(), BITSPIXEL),
		0, 0, 0, 0, 0, 0, // ������ɫλ 
		0, // û�з�͸���Ȼ��� 
		0, // ������λλ 
		0, // ���ۼӻ��� 
		0, 0, 0, 0, // �����ۼ�λ 
		32, // 32 λ��Ȼ��� 
		0, // ��ģ�建�� 
		0, // �޸������� 
		PFD_MAIN_PLANE, // ���� 
		0, // ���� 
		0, 0, 0 // ���Բ�,�ɼ��Ժ������ģ 
	}; 
	int pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if (pixelformat == 0)
	{
		AfxMessageBox("no matched pixelformat!");
		return FALSE;
	}
	//����ָ�����ظ�ʽ
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
	// ����ȵ��Ժ��ټ�
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
	//	��ԭ���Ƶ�ģ�͵��м�
 	glTranslatef(-m_object_center.x, -m_object_center.y,
 		-m_object_center.z - m_object_scale * 0.75);
	m_deltaz = -m_object_scale * 0.75;
}

void COpenGL::Set3DProjection(int width, int height)
{
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	// ֪ͨOpenGL�õ�ǰ���õı�����ɫ�����ɫ���棬ͬʱ���������򻺴�
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// �����ӿڱ任,Ĭ��������ӿڱ�����Ϊռ�ݴ򿪴��ڵ���������
	glViewport(0, 0, width, height);
	// ����͸��ͶӰ�任
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, width / (double)height, 1e-2, 1e6);
	//gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}

// ����ģ�ͱ任
void COpenGL::RotateObject(double x, double y)
{
	double modelview_matrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// �Ƚ�ģ����������ԭ�㣬����ԭ����m_object_center
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

	//ɾ��DC
	if (m_pDC)
		delete m_pDC;
}