#include "stdafx.h"
#include "GLDraw.h"
#include "DefMesh.h"
#include "TextureArray.h"
#include "../Common/utildefines.h"

#include <vector>
#include <gl/glew.h>
#include <gl/glut.h>
using namespace std;

GLDraw::GLDraw(void)
{
	m_pDefmesh = NULL;
	m_DrawMode = DMFill;   
	m_ShadeMode = SMSmooth;

	m_SkelMode = false;
	m_bTextureOn = false;
}

GLDraw::~GLDraw(void)
{
}

void GLDraw::BuildList()
{
	Material* pmaterial;
	int i, j, k, v, uv;
	Mesh mesh = m_pDefmesh->GetMesh();
	vector<MVert> verts = mesh.getVerts();
	vector<MFace> faces = mesh.getFaces();
	vector<Vector3> coords = mesh.getCoords();
	vector<Group> groups = mesh.getGroups();
	static GLfloat colorm[] = {1.f, .9f, .75f, 1.f};

	static GLfloat no_ambient[] = {1.0, 1.0, 1.0, 1.0};
	static GLfloat no_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	static GLfloat no_specular[] = {1.0, 1.0, 1.0, 1.0};
	static GLfloat no_emission[] = {1.0, 1.0, 1.0, 1.0};
	static GLfloat no_shininess[] = {0.0};

	// construct smooth face display list
	if(glIsList(m_FaceDisplayList))
		glDeleteLists(m_FaceDisplayList,1);
	m_FaceDisplayList = glGenLists(1);

	glNewList (m_FaceDisplayList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(mesh.toAdd[0], mesh.toAdd[1], mesh.toAdd[2]);
	glScalef(mesh.scale, mesh.scale, mesh.scale);
	if (mesh.totgroup)	{							// has mterial file
		for (i = 0; i < mesh.totgroup; i++) {
			pmaterial = mesh.m_pMaterial[groups[i].mtlID];
			if(1 - pmaterial->alpha > FLT_EPSILON) continue;

			if(pmaterial->ptexbuffer!=NULL)	{
				glMaterialfv(GL_FRONT, GL_AMBIENT, no_ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, no_diffuse);	
			}
			else {
				glMaterialfv(GL_FRONT, GL_AMBIENT, pmaterial->ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, pmaterial->diffuse);
			}

			glMaterialfv(GL_FRONT, GL_SHININESS, &pmaterial->shininess);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pmaterial->specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, pmaterial->emission);

			glBindTexture(GL_TEXTURE_2D, pmaterial->texID);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// color * texture

			glBegin(GL_TRIANGLES);
			for (j = groups[i].startf; j <= groups[i].endf; j++) {
				for (k = 0; k < 3; k++)	{
					v = faces[j].v[k];
					uv = faces[j].uv[k];
					if (groups[i].bTexture)		glTexCoord2f(coords[uv][0], coords[uv][1]);
					glNormal3fv(&(verts[v].normal));
					glVertex3fv(&(verts[v].coord));

				}
			}
			glEnd ();
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glDepthMask(GL_FALSE);

		for (i = 0; i < mesh.totgroup; i++) {
			pmaterial = mesh.m_pMaterial[groups[i].mtlID];
			if(1 - pmaterial->alpha > FLT_EPSILON) continue;

			if(pmaterial->ptexbuffer!=NULL)	{
				glMaterialfv(GL_FRONT, GL_AMBIENT, no_ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, no_diffuse);	
			}
			else {
				glMaterialfv(GL_FRONT, GL_AMBIENT, pmaterial->ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, pmaterial->diffuse);
			}

			glMaterialfv(GL_FRONT, GL_SHININESS, &pmaterial->shininess);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pmaterial->specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, pmaterial->emission);

			glBindTexture(GL_TEXTURE_2D, pmaterial->texID);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// color * texture

			glBegin(GL_TRIANGLES);
			for (j = groups[i].startf; j <= groups[i].endf; j++) {
				for (k = 0; k < 3; k++)	{
					v = faces[j].v[k];
					uv = faces[j].uv[k];
					if (groups[i].bTexture)		glTexCoord2f(coords[uv][0], coords[uv][1]);
					glNormal3fv(&(verts[v].normal));
					glVertex3fv(&(verts[v].coord));

				}
			}
			glEnd ();
		}

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorm);
		glBegin(GL_TRIANGLES);
		for (j = 0; j < mesh.totface; j++)
			for (k = 0; k < 3; k++)	{
				glNormal3fv(&(verts[faces[j].v[k]].normal));
				glVertex3fv(&(verts[faces[j].v[k]].coord));
			}
		glEnd ();
	}

	glPopMatrix();
	glEndList();

	// construct flat face display list
	if(glIsList(m_FaceFlatDisplayList))
		glDeleteLists(m_FaceFlatDisplayList,1);
	m_FaceFlatDisplayList = glGenLists(1);

	glNewList(m_FaceFlatDisplayList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(mesh.toAdd[0], mesh.toAdd[1], mesh.toAdd[2]);
	glScalef(mesh.scale, mesh.scale, mesh.scale);
	if (mesh.totgroup)	{							// has mterial file
		for (i = 0; i < mesh.totgroup; i++) {
			pmaterial = mesh.m_pMaterial[groups[i].mtlID];
			if(1 - pmaterial->alpha > FLT_EPSILON) continue;

			if(pmaterial->ptexbuffer!=NULL)	{
				glMaterialfv(GL_FRONT, GL_AMBIENT, no_ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, no_diffuse);	
			}
			else {
				glMaterialfv(GL_FRONT, GL_AMBIENT, pmaterial->ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, pmaterial->diffuse);
			}

			glMaterialfv(GL_FRONT, GL_SHININESS, &pmaterial->shininess);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pmaterial->specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, pmaterial->emission);

			glBindTexture(GL_TEXTURE_2D, pmaterial->texID);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// color * texture

			glBegin(GL_TRIANGLES);
			for (j = groups[i].startf; j <= groups[i].endf; j++) {
				glNormal3fv(&(faces[j].normal));
				for (k = 0; k < 3; k++)	{
					v = faces[j].v[k];
					uv = faces[j].uv[k];
					if (groups[i].bTexture)		glTexCoord2f(coords[uv][0], coords[uv][1]);
					glVertex3fv(&(verts[v].coord));

				}
			}
			glEnd();
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glDepthMask(GL_FALSE);

		for (i = 0; i < mesh.totgroup; i++) {
			pmaterial = mesh.m_pMaterial[groups[i].mtlID];
			if(1 - pmaterial->alpha > FLT_EPSILON) continue;

			if(pmaterial->ptexbuffer!=NULL)	{
				glMaterialfv(GL_FRONT, GL_AMBIENT, no_ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, no_diffuse);	
			}
			else {
				glMaterialfv(GL_FRONT, GL_AMBIENT, pmaterial->ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, pmaterial->diffuse);
			}

			glMaterialfv(GL_FRONT, GL_SHININESS, &pmaterial->shininess);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pmaterial->specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, pmaterial->emission);

			glBindTexture(GL_TEXTURE_2D, pmaterial->texID);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// color * texture

			glBegin(GL_TRIANGLES);
			for (j = groups[i].startf; j <= groups[i].endf; j++) {
				glNormal3fv(&(faces[j].normal));
				for (k = 0; k < 3; k++)	{
					v = faces[j].v[k];
					uv = faces[j].uv[k];
					if (groups[i].bTexture)		glTexCoord2f(coords[uv][0], coords[uv][1]);
					glVertex3fv(&(verts[v].coord));

				}
			}
			glEnd();
		}

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorm);
		glBegin(GL_TRIANGLES);
		for (j = 0; j < mesh.totface; j++) {
			glNormal3fv(&(faces[j].normal));
			for (k = 0; k < 3; k++)
				glVertex3fv(&(verts[faces[j].v[k]].coord));
		}
		glEnd ();
	}

	glPopMatrix();
	glEndList();
}

void GLDraw::BindTexture()
{
	CTextureArray texTool;
	Mesh& mesh = m_pDefmesh->GetOrigMesh();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (size_t i = 0; i < mesh.m_pMaterial.size(); i++) {
		Material* pmtl = mesh.m_pMaterial[i];
		if (pmtl->ptexbuffer) {
			glGenTextures(1, &pmtl->texID);
			//glActiveTexture(GL_TEXTURE0 + pmtl->texID - 1);
			glBindTexture(GL_TEXTURE_2D, pmtl->texID);

			Texturebuffer *tex = pmtl->ptexbuffer;
			if (tex->components == 3)
				texTool.MakeRGBTexture(tex, false);
			else if (tex->components == 4)
				texTool.MakeRGBATexture(tex, false);
			else
				continue;
		}
	}
}

void GLDraw::DrawMesh(void)
{
	if( !m_pDefmesh ) return;
	if (m_bTextureOn)
		glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	switch( m_DrawMode )
	{
	case DMPoints   :   DrawPoints();  break;	
	case DMWire	    :	DrawWire();    break;
	case DMHidden	:	DrawHidden();  break;
	case DMFill	    :	DrawFill();    break;
	case DMFillWire :	DrawFillWire();break;
	default : break;
	}
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void GLDraw::SetSkelMode(bool sphere)
{
	m_SkelMode = sphere;
}

/* Draw Bone by spheres */
void GLDraw::DrawBone(Vector3 head, Vector3 tail)
{
	static GLfloat colors[] = {0.24f, 0.35f, 0.67f, 1.f};
	static GLfloat Ka[] = {0.9f, 0.6f, 0.3f, 1.f};
	static GLfloat Kd[] = {0.8f, 0.7f, 0.5f, 1.f};
	static GLfloat Ks[] = {0.3f, 0.3f, 0.3f, 1.f};

	Vector3 offset = tail - head;
	Vector3 center = (head + tail) / 2;
	if (m_SkelMode)	{
		glPushMatrix();

		Vector3 up, out, right;
		glTranslatef(center[0], center[1], center[2]);
		up[0] = up[1] = up[2] = 1;

		if (offset[2] != 0)
			up[2] = -(offset[0] + offset[1]) / offset[2];
		else if (offset[1] != 0)
			up[1] = -(offset[0] + offset[2]) / offset[1];
		else if (offset[0] != 0)
			up[0] = -(offset[2] + offset[1]) / offset[0];

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Kd);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
		GLUquadricObj *q = gluNewQuadric();
		up = up.normalize();
		out = offset.normalize();
		right = out % up;
		right = right.normalize();

		GLfloat mat[16];
		memset(mat, 0, sizeof(GLfloat[16]));
		mat[0] = right[0];	mat[1] = right[1];	mat[2] = right[2];
		mat[4] = up[0];		mat[5] = up[1];		mat[6] = up[2];
		mat[8] = out[0];	mat[9] = out[1];	mat[10] = out[2];
		mat[15] = 1;
		glMultMatrixf(mat);
		glScalef(4.f, 4.f, offset.length()/2);
		gluSphere(q, 1, 15, 15);

		glPopMatrix();
	}
	else {
		float delta = 0.4;
		Vector3 start, pos[4];
		int maxdir = offset.maxAix();

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colors);
		if (maxdir == 0) {
			if (offset[0] > 0)
				start = Vector3(head[0]-delta, head[1], head[2]);
			else
				start = Vector3(head[0]+delta, head[1], head[2]);
			pos[0] = Vector3(head[0], head[1]-delta, head[2]-delta);
			pos[1] = Vector3(head[0], head[1]-delta, head[2]+delta);
			pos[2] = Vector3(head[0], head[1]+delta, head[2]+delta);
			pos[3] = Vector3(head[0], head[1]+delta, head[2]-delta);
		}
		else if (maxdir == 1) {
			if (offset[1] > 0)
				start = Vector3(head[0], head[1]-delta, head[2]);
			else
				start = Vector3(head[0], head[1]+delta, head[2]);
			pos[0] = Vector3(head[0]-delta, head[1], head[2]-delta);
			pos[1] = Vector3(head[0]+delta, head[1], head[2]-delta);
			pos[2] = Vector3(head[0]+delta, head[1], head[2]+delta);
			pos[3] = Vector3(head[0]-delta, head[1], head[2]+delta);
		}
		else if (maxdir == 2) {
			if (offset[2] > 0)
				start = Vector3(head[0], head[1], head[2]-delta);
			else
				start = Vector3(head[0], head[1], head[2]+delta);
			pos[0] = Vector3(head[0]-delta, head[1]-delta, head[2]);
			pos[1] = Vector3(head[0]+delta, head[1]-delta, head[2]);
			pos[2] = Vector3(head[0]+delta, head[1]+delta, head[2]);
			pos[3] = Vector3(head[0]-delta, head[1]+delta, head[2]);
		}
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(start[0], start[1], start[2]);						// 0
		glVertex3f(pos[0][0],  pos[0][1], pos[0][2]);					// 1
		glVertex3f(pos[1][0],  pos[1][1], pos[1][2]);					// 2
	
		glVertex3f(tail[0],  tail[1] , tail[2]);	// Base
	
		glVertex3f(pos[0][0], pos[0][1], pos[0][2]);					// 1
		glVertex3f(pos[3][0], pos[3][1], pos[3][2]);					// 4
		glVertex3f(start[0], start[1], start[2]);						// 0
		glVertex3f(pos[1][0], pos[1][1], pos[1][2]);					// 2
		glVertex3f(pos[2][0], pos[2][1], pos[2][2]);					// 3
		glVertex3f(start[0], start[1], start[2]);						// 0
		glVertex3f(pos[3][0], pos[3][1], pos[3][2]);					// 4
	
		glVertex3f(tail[0], tail[1] , tail[2]);	// Base
	
		glVertex3f(pos[2][0], pos[2][1], pos[2][2]);					// 3
		glVertex3f(pos[3][0], pos[3][1], pos[3][2]);					// 4
		glEnd();
	}

}

void GLDraw::RenderBone(Joint *root)
{
	Joint* child;
	int	child_num = root->numOfChildren();
	if (child_num == 0)		return;

	for ( int i = 0; i < child_num; i++) {
		child = root->getChild(i);
		DrawBone(root->getPosition(), child->getPosition());
	}

	for (int i = 0; i < child_num; i++)
		RenderBone(root->getChild(i));
}

void GLDraw::BuildSkelList()
{
	// construct bone display list
	Mesh mesh = m_pDefmesh->GetMesh();
	Clip* pclip = m_pDefmesh->GetClip();
	if (pclip == NULL)	return;

	if(glIsList(m_BoneDisplayList))
		glDeleteLists(m_BoneDisplayList, 1);
	m_BoneDisplayList = glGenLists(1);

	glNewList (m_BoneDisplayList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(mesh.toAdd[0], mesh.toAdd[1], mesh.toAdd[2]);
	glScalef(mesh.scale, mesh.scale, mesh.scale);
	RenderBone(pclip->getJointByID(0));
	glPopMatrix();
	glEndList();
}

void GLDraw::DrawSkel(void)
{
	if ( !m_pDefmesh->GetClip() ) return;
	
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
	if (!m_SkelMode)
		glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glPushMatrix();
	switch ( m_DrawMode )
	{
	case DMPoints:
	case DMWire:
	case DMHidden:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glCallList(m_BoneDisplayList);
		break;

	case DMFill:
	case DMFillWire:
		glCallList(m_BoneDisplayList);
		break;
	}
	glPopMatrix();

	glPopAttrib();
}

// render face
void GLDraw::DrawFill(void)
{
	if(m_ShadeMode == SMSmooth)
		glCallList(m_FaceDisplayList);

	if(m_ShadeMode == SMFlat)
		glCallList(m_FaceFlatDisplayList);
}

// render points
void GLDraw::DrawPoints(void)
{  
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	DrawFill();
	glPopAttrib();
}	

// render cull face
void GLDraw::DrawHidden(void)
{	
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1);
	glDisable(GL_LIGHTING);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	DrawFill();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_LIGHTING);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	DrawWire();
	glPopAttrib();
}

// Draw faces with edges
void GLDraw::DrawFillWire(void)
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1);
	DrawFill();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glColor3f(.5f,.5f,.5f);
	DrawWire();
	glPopAttrib();	
}	

// draw lines
void GLDraw::DrawWire(void)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawFill();
	glPopAttrib();
}
