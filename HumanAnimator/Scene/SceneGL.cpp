#include "StdAfx.h"
#include "view.h"
#include "SceneGL.h"
#include "../Common/utildefines.h"
#include "InterfaceManager.h"

#include <math.h>
#include <gl/glew.h>
#include <gl/glu.h>
#include <gl/glut.h>

CSceneGL::CSceneGL(void)
{
	m_frame = 0;
	m_clockPtr = 0;

	m_bDrawMesh = true;
	m_bDrawSkel = false;
	m_bDrawFloor = false;

	m_DC = NULL;
	m_bTexture = false;
	m_SkelSphere = false;
	m_TransMode = Rotation;
	m_ProjMode = OrthoPro;
	m_DrawMode = GLW::DMFill;
	m_ShadeMode = GLW::SMSmooth;
	m_pcam = new View();
	m_Transform = Transform<>();
}

CSceneGL::~CSceneGL(void)
{
	if (m_pcam)
		delete m_pcam;

	wglMakeCurrent(NULL, NULL);
	if (m_hRC != NULL) {
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}
}

BOOL CSceneGL::InitializeDevice(const HDC& dc)
{
	m_DC = dc;
	if ( !SetupPixelFormat() )
		return	FALSE;
	m_hRC = wglCreateContext(m_DC); 
	if (m_hRC ==  NULL)
		return FALSE;

	::wglMakeCurrent(m_DC, m_hRC); 

	return TRUE;
}

BOOL CSceneGL::SetupPixelFormat()
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
		return FALSE;
	}

	if (SetPixelFormat(m_DC, pixelformat, &pfd) == FALSE)
	{
		TRACE("Can't set specified pixelformat!");
		return FALSE;
	}

	return	TRUE;
}

void CSceneGL::InitGL(void)
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glClearColor(1.f, 1.f, 1.f, 0.f);
	//glShadeModel(GL_SMOOTH);		// ??

	GLfloat light0_position[] = {5.0, 10.0, 10.0, 1.0 };
	GLfloat light1_position[] = {-5.0, -10.0, -10.0, 1.0 };
	//GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light0_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light0_specular[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light0_ambient[] = { 0.15, 0.15, 0.15, 1.0 };
	glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, light0_specular );
	glLightfv( GL_LIGHT0, GL_AMBIENT, light0_ambient );

	glLightfv( GL_LIGHT1, GL_POSITION, light1_position );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, light0_diffuse );
	glLightfv( GL_LIGHT1, GL_SPECULAR, light0_specular );
	glLightfv( GL_LIGHT1, GL_AMBIENT, light0_ambient );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable(GL_LIGHT1);
	//glEnable(GL_COLOR_MATERIAL);
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_NORMALIZE );			// normalize the normal in pixel shader
	glDisable( GL_ALPHA_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//////////////////////////////////////////////////////////////////////////
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return	;
}

void CSceneGL::SetCamera(View::ViewMode vm)
{
	m_pcam->SetViewMode(vm);
}

void CSceneGL::SetTransMode(TransMode tm)
{
	m_TransMode = tm;
}

void CSceneGL::SetProjMode(ProjectMode pm)
{
	m_ProjMode = pm;
}

void CSceneGL::SetTexture()
{
	m_bTexture = !m_bTexture;
}

void CSceneGL::SetSkelSphere()
{
	m_SkelSphere = !m_SkelSphere;
	DefMesh* pdefmesh = m_pInterface->getDefmesh();
	if (pdefmesh && pdefmesh->GetClip())
		pdefmesh->BuildSkelList(m_SkelSphere);
}

void CSceneGL::SetDrawMode(GLW::DrawMode dm)
{
	m_DrawMode = dm;
}

void CSceneGL::SetShadeMode(GLW::ShadeMode sm)
{
	m_ShadeMode = sm;
}

void CSceneGL::SetDrawMesh()
{
	m_bDrawMesh = !m_bDrawMesh;
}

void CSceneGL::SetDrawSkel()
{
	m_bDrawSkel = !m_bDrawSkel;
}

void CSceneGL::SetDrawFloor()
{
	m_bDrawFloor = !m_bDrawFloor;
}

void CSceneGL::SetInterface(InterfaceManager *pInteface)
{
	m_pInterface = pInteface;
}

Vector3 CSceneGL::GetTransPoint(Vector3 p)
{
	Vector3 v;
	v[0] = (2. * p[0] - m_pcam->GetWidth()) / (2. * m_pcam->GetWidth());
	v[1] = (2. * p[1] - m_pcam->GetHeight()) / (2. * m_pcam->GetHeight());
	v[2] = sqrt(1- v[0] * v[0] - v[1] * v[1]);

	return v;
}

void CSceneGL::ResetCoord()
{
	m_Transform = Transform<>();
}

void CSceneGL::MouseMoveEvent(Vector3 lastPos, Vector3 curPos)
{
	if (m_TransMode == Translation)
	{
		int view_mode = m_pcam->GetViewMode();
		Vector3 v = (curPos - lastPos) * 0.002;
		switch (view_mode)
		{
		case View::ViewBack:
			v[0] = -v[0];
			break;
		case View::ViewLeft:
			v[2] = v[0];
			v[0] = 0.;
			break;
		case View::ViewRight:
			v[2] = -v[0];
			v[0] = 0.;
			break;
		default:
			break;
		}
		//Transform<> cur = Transform<>(v);
		//m_Transform = m_Transform * cur;
		Vector3 trans = m_Transform.getTrans() + v;
		m_Transform = Transform<>(m_Transform.getRot(), m_Transform.getScale(), trans);
	}
	else if (m_TransMode == Rotation)
	{
		Vector3 p1 = GetTransPoint(lastPos);
		Vector3 p2 = GetTransPoint(curPos);
		Vector3 n = p1 % p2;
		//float angle = acos(p1 * p2);
		float angle = n.length();
		//Quaternion<> rot = Quaternion<>(n, angle);
		//Transform<> cur = Transform<>(rot);
		//m_Transform = m_Transform * cur;
		Quaternion<> rot = m_Transform.getRot() * Quaternion<>(n, angle);
		m_Transform = Transform<>(rot, m_Transform.getScale(), m_Transform.getTrans());
	}
	else
		m_Transform = Transform<>();

	PaintGL();
}

void CSceneGL::MouseWheelEvent(float s)
{
	m_Transform = m_Transform * Transform<>(1-s);
	PaintGL();
}

void CSceneGL::SetView(void)
{
	int width = m_pcam->GetWidth();
	int height = m_pcam->GetHeight();
	double fov = m_pcam->GetFov();
	double znear = m_pcam->GetNear();
	double zfar = m_pcam->GetFar();
	Vector3 eye = m_pcam->GetEye();
	Vector3 at = m_pcam->GetCenter();
	Vector3 up = m_pcam->GetUp();


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (m_ProjMode == PerspecPro)
		gluPerspective(fov, width / (double)height, znear, zfar);
	else {
		if (width > height)
			glOrtho(-width / (double)height, width / (double)height, -1., 1., znear, zfar);
		else
			glOrtho(-1., 1., -height / (double)width, height / (double)width, znear, zfar);
	}

	gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);	// 加了之后模型视图矩阵偏移了(0, 0, -1)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void CSceneGL::DisplayTimer()
{
	float val;
	char buf[100];

	end = clock();
	m_pInterface->deformed = false;
	float diff = (float)(end - startTime);
	
	if (diff < FLT_EPSILON)
		val = CLOCKS_PER_SEC;
	else
		val = CLOCKS_PER_SEC / diff;

	int width = m_pcam->GetWidth();
	int height = m_pcam->GetHeight();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0.0, width, 0.0, height );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

	glColor3f( 1.f, 1.f, 1.f );
	glRasterPos2i( width - 100, height - 50 );
	sprintf( buf, "%.2f fps", val );
	int len = (int) strlen(buf);
	for(int i=0; i<len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buf[i]);

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

//  Draw Background
void CSceneGL::DrawBackground()
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
	glColor4f(0.247f, 0.686f, 1.f, 0.f);
	/*glColor4f(0.f, 0.f, 0.f, 0.f);  */    glVertex2f(-1,1);
	glColor4f(1.f, 1.f, 1.f, 0.f);		glVertex2f(-1,-1);
	glColor4f(0.247f, 0.686f, 1.f, 0.f);
	/*glColor4f(0.f, 0.f, 0.f, 0.f); */     glVertex2f(1,1);
	glColor4f(1.f, 1.f, 1.f, 0.f);		glVertex2f(1,-1);

	glEnd();
	glPopAttrib();	
	glClear(GL_DEPTH_BUFFER_BIT);
}

void CSceneGL::DrawFloor()
{
	double  ox, oz;
	float  size = 1.f;
	int  num_x = 10, num_z = 10;

	glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
	glBegin( GL_QUADS );
	glNormal3f( 0.f, 1.f, 0.f );
	ox = -(num_x * size) / 2.0;
	for ( int x = 0; x < num_x; x++, ox += size )
	{
		oz = -(num_z * size) / 2.0;
		for ( int z = 0; z < num_z; z++, oz += size )
		{
			if ( ((x + z) % 2) == 0 )
				glColor4f( 1.f, 1.f, 1.f, 0.f );
			else
				glColor4f( 0.8f, 0.8f, 0.8f, 0.f );
			glVertex3d( ox, 0.0, oz );
			glVertex3d( ox, 0.0, oz + size );
			glVertex3d( ox + size, 0.0, oz + size );
			glVertex3d( ox  + size, 0.0, oz );
		}
	}
	glEnd();
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}

clock_t startTime;

void CSceneGL::PaintGL()
{
	GLfloat M[16];
	Quaternion<> rot = m_Transform.getRot();
	DefMesh* pdefmesh = m_pInterface->getDefmesh();

	if (!m_pInterface->deformed)
		startTime = clock();
	::wglMakeCurrent(m_DC, m_hRC);
	glViewport(0, 0, m_pcam->GetWidth(), m_pcam->GetHeight());
	DrawBackground();
	SetView();

	//glPushMatrix();
	//rot.getRotationMatrix(M);
	//glMultMatrixf(M);
	//m_SceneObject.DrawCoordinate();
	//glPopMatrix();

	m_Transform.getTransformMatrix(M);
	glMultMatrixf(M);
	
	glTranslatef(0.f, -0.3f, 0.f);

	// floor
	if (m_bDrawFloor)	DrawFloor();

	 //shadow
	if (m_bDrawFloor)	{
		Vector3 lightRay = m_Transform.getRot() * Vector3(5, 10, 10);
		if(lightRay[1] == 0)
			lightRay[1] = 1e-5;
		lightRay = -lightRay / lightRay[1];

		glDisable(GL_LIGHTING);
		glColor3f(0.3f, 0.3f, 0.3f);
		glPushMatrix();
		float matr[16] = {1,0,0,0, (float)lightRay[0],0,(float)lightRay[2],0, 0,0,1,0, 0.,0.01f,0.,1};
		glMultMatrixf(matr);
		glDepthMask(0);

		if (pdefmesh && m_bDrawMesh)
			pdefmesh->RenderMesh(GLW::DMFill, GLW::SMSmooth, 0, 1, m_pInterface->GetRenderContext());
		if (m_pInterface->IsEnabled() && m_bDrawSkel)
			pdefmesh->RenderSkeleton(m_DrawMode);

		glDepthMask(1);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	if (pdefmesh && m_bDrawMesh)
		pdefmesh->RenderMesh(m_DrawMode, m_ShadeMode, m_bTexture, 0, m_pInterface->GetRenderContext());
	if (m_pInterface->IsEnabled() && m_bDrawSkel)
		pdefmesh->RenderSkeleton(m_DrawMode);

	DisplayTimer();
	glFinish();
	SwapBuffers(wglGetCurrentDC());
}