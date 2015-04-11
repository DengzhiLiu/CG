#include "stdafx.h"
#include "HWDraw.h"
#include "DefMesh.h"

#include <io.h>
#include <string>
#include <fcntl.h>
#include <assert.h>
#include <direct.h>

using namespace std;

HWDraw::HWDraw()
{
	m_pDefmesh = NULL;
	m_DrawMode = DMFill;   
	m_ShadeMode = SMSmooth;

	m_bPlay = false;
	m_bShadow = false;
	m_bTextureOn = false;
	
	ProgramObject = 0;
	vertexshaderObj = -1;
	fragShaderObj = -1;

	normalAttrib = -1;
	positionAttrib = -1;
	indexAttrib = -1;
	weightAttrib = -1;
	texcoordAttrib = -1;

	texSampler = -1;
	playParam = -1;
	addParam = -1;
	mtlParam = -1;
	shadowParam = -1;
	rootParam = -1;
	scaleParam = -1;
	refMatriceParam = -1;
	motionMatriceParam = -1;
}

HWDraw::~HWDraw()
{
	glDeleteShader(vertexshaderObj);
	glDeleteShader(fragShaderObj);
	glDeleteProgram(ProgramObject);
}

int HWDraw::CountSize(const char* fileName)
{
	int count = -1;
	// Open the file, seek to the end to find its length
	int fd = _open(fileName, _O_RDONLY);
	if (fd != -1) {
		count = _lseek(fd, 0, SEEK_END) + 1;
		_close(fd);
	}

	return count;
}

char* HWDraw::ReadShaderFile(const char* filename, int size)
{
	FILE* fp = fopen(filename, "r");
	if (!fp)	return NULL;

	char* buffer = new char[size];
	int bytes = fread(buffer, 1, size, fp);
	buffer[bytes] = '\0';
	fclose(fp);
	return buffer;
}

GLuint HWDraw::AddShader(GLuint programObject, const GLchar *shaderSource, GLenum shaderType)
{
	assert(programObject != 0);
	assert(shaderSource != 0);
	assert(shaderType != 0);

	GLuint object = glCreateShader(shaderType);
	assert(object != 0);

	glShaderSource(object, 1, &shaderSource, NULL);

	// compile vertex shader object
	glCompileShader(object);

	// check if shader compiled
	GLint compiled = 0;
	glGetShaderiv(object, GL_COMPILE_STATUS, &compiled);
	if (!compiled)	return -1;

	// attach vertex shader to program object
	glAttachShader(programObject, object);

	// delete vertex object, no longer needed
	//glDeleteShader(object);
	return object;
}

void HWDraw::InitShader()
{
	//if (!GLEW_ARB_vertex_shader || !GL_ARB_fragment_shader)
	//	return;
	ProgramObject = glCreateProgram();

	int		nPos;
	CString sPath;   
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	sPath.ReleaseBuffer(); 
	nPos = sPath.ReverseFind('\\');   
	sPath = sPath.Left(nPos);
	nPos = sPath.ReverseFind('\\');   
	sPath = sPath.Left(nPos);

	string path = sPath.GetBuffer();
	string vertexfile = path + string("\\GLSL\\skinning_vertex.glsl");
	string fragfile = path + string("\\GLSL\\skinning_fragment.glsl");
	sPath.ReleaseBuffer();

	int count = CountSize(vertexfile.c_str());
	if (count == -1)	return;

	GLchar* ShadeData = ReadShaderFile(vertexfile.c_str(), count);
	if (!ShadeData)	return;

	vertexshaderObj = AddShader(ProgramObject, ShadeData, GL_VERTEX_SHADER);
	delete[] ShadeData;

	count = CountSize(fragfile.c_str());
	if (count == -1)	return;

	ShadeData = ReadShaderFile(fragfile.c_str(), count);
	if (!ShadeData)	return;

	fragShaderObj = AddShader(ProgramObject, ShadeData, GL_FRAGMENT_SHADER);
	delete[] ShadeData;

	// bind position attribute to location 0
	glBindAttribLocation(ProgramObject, 0, "position");

	glLinkProgram(ProgramObject);
	GLint linked = false;
	glGetProgramiv(ProgramObject, GL_LINK_STATUS, &linked);
	if (!linked)	return;

	glValidateProgram(ProgramObject);
	GLint validated = false;
	glGetProgramiv(ProgramObject, GL_VALIDATE_STATUS, &validated);
	if (!validated)	return;

	positionAttrib = glGetAttribLocation(ProgramObject, "position");
	assert(positionAttrib >= 0);

	normalAttrib = glGetAttribLocation(ProgramObject, "normal");
	assert(normalAttrib >= 0);

	indexAttrib = glGetAttribLocation(ProgramObject, "index");
	assert(indexAttrib >= 0);

	weightAttrib = glGetAttribLocation(ProgramObject, "weight");
	assert(weightAttrib >= 0);

	texcoordAttrib = glGetAttribLocation(ProgramObject, "texcoord");
	assert(texcoordAttrib >= 0);

	playParam = glGetUniformLocation(ProgramObject, "play");
	assert(playParam >= 0);

	scaleParam = glGetUniformLocation(ProgramObject, "mscale");
	assert(scaleParam >= 0);

	addParam = glGetUniformLocation(ProgramObject, "toAdd");
	assert(addParam >= 0);

	rootParam = glGetUniformLocation(ProgramObject, "rootoffset");
	assert(rootParam >= 0);

	refMatriceParam = glGetUniformLocation(ProgramObject, "refMatrice[0]");
	assert(refMatriceParam >= 0);

	motionMatriceParam = glGetUniformLocation(ProgramObject, "motionMatrice[0]");
	assert(motionMatriceParam >= 0);

	mtlParam = glGetUniformLocation(ProgramObject, "usemtl");
	assert(mtlParam >= 0);

	shadowParam = glGetUniformLocation(ProgramObject, "shadow");
	assert(shadowParam >= 0);

	texSampler = glGetUniformLocation(ProgramObject, "tex");
	assert(texSampler >= 0);

	//glDetachShader(ProgramObject, fragShaderObj);
	//glLinkProgram(ProgramObject);
	glUseProgram(ProgramObject);

	glUniform1i(playParam, 0);
	glUniform1i(mtlParam, 0);
	glUniform1i(shadowParam, 0);
	glUniform1i(texSampler, 0);
	glUniform1f(scaleParam, m_pDefmesh->GetMesh().scale);
	glUniform3fv(addParam, 1, &(m_pDefmesh->GetMesh().toAdd));

	glUseProgram(0);
}

void HWDraw::SetUniParam(const GLchar* UnifromName, int value)
{
	glUseProgram(ProgramObject);

	GLint location = glGetUniformLocation(ProgramObject, UnifromName);
	glUniform1i(location, value);

	glUseProgram(0);
}

void HWDraw::SetVSBindMat()
{
	Bone* bone;
	float tm[16];
	GLint currentMat;
	Transform<> bindinvtrans;
	int bone_num = m_pDefmesh->getBoneNum();

	glUseProgram(ProgramObject);

	glUniform3fv(rootParam, 1, &(m_pDefmesh->toAdd));
	for (int i = 0; i < bone_num; i++)	{
		bone = m_pDefmesh->getBoneByID(i);
		currentMat = refMatriceParam + i;
		bindinvtrans = bone->getBindTrans();
		bindinvtrans.getTransformMatrix(tm);

		glUniformMatrix4fv(currentMat, 1, GL_FALSE, tm);		// GL_TRUE row first£¬GL_FALSE column first
	}

	glUseProgram(0);
}

void HWDraw::SetVSMotionMat(int frame)
{
	Bone* bone;
	float tm[16];
	GLint currentMat;
	Transform<> motiontrans;
	int bone_num = m_pDefmesh->getBoneNum();

	m_bPlay = true;
	glUseProgram(ProgramObject);

	for (int i = 0; i < bone_num; i++)	{
		bone = m_pDefmesh->getBoneByID(i);
		currentMat = motionMatriceParam + i;
		motiontrans = bone->getMotion(frame);
		motiontrans.getTransformMatrix(tm);

		glUniformMatrix4fv(currentMat, 1, GL_FALSE, tm);
	}

	glUseProgram(0);
}

void HWDraw::DrawMesh()
{
	if( !m_pDefmesh ) return;

	glUseProgram(ProgramObject);

	if (m_bTextureOn)
		glUniform1i(mtlParam, 1);
	else
		glUniform1i(mtlParam, 0);

	if (m_bPlay)
		glUniform1i(playParam, 1);
	else
		glUniform1i(playParam, 0);

	if (m_bShadow)
		glUniform1i(shadowParam, 1);
	else
		glUniform1i(shadowParam, 0);

	glPushMatrix();
	switch( m_DrawMode )
	{
	case DMPoints   :   DrawPoints();  break;	
	case DMWire	    :	DrawWire();    break;
	case DMHidden	:	DrawHidden();  break;
	case DMFillWire :	//DrawFillWire();break;
	case DMFill	    :	DrawFill();    break;
	
	default : break;
	}
	glPopMatrix();

	glUseProgram(0);
}

void HWDraw::DrawSkel()
{

}

void HWDraw::DrawFill()
{
	int i, j, k, v, uv;
	Mesh mesh = m_pDefmesh->GetOrigMesh();
	vector<MVert> verts = mesh.getVerts();
	vector<MFace> faces = mesh.getFaces();
	vector<Vector3> coords = mesh.getCoords();
	vector<Group> groups = mesh.getGroups();

	glPushMatrix();

	if (mesh.totgroup)	{
		for (i = 0; i < mesh.totgroup; i++) {
			if (groups[i].bTexture)	{
				//glUniform1i(texSampler, mesh.m_pMaterial[groups[i].mtlID]->texID - 1);
				glBindTexture(GL_TEXTURE_2D, mesh.m_pMaterial[groups[i].mtlID]->texID);
			}
			glBegin(GL_TRIANGLES);
			for (j = groups[i].startf; j <= groups[i].endf; j++) {
				for (k = 0; k < 3; k++)	{
					v = faces[j].v[k];
					uv = faces[j].uv[k];
					if (groups[i].bTexture)		glVertexAttrib2f(texcoordAttrib, coords[uv][0], coords[uv][1]);
					glVertexAttrib3fv(normalAttrib, &(verts[v].normal));			// normal
					if (m_bPlay) {
						glVertexAttrib4fv(weightAttrib, verts[v].weight);			// weights
						glVertexAttrib4fv(indexAttrib, verts[v].bone);				// indices
					}
					glVertexAttrib3fv(positionAttrib, &(verts[v].coord));
				}
			}
			glEnd ();
		}
	}
	else {
		glBegin(GL_TRIANGLES);
		for (j = 0; j < mesh.totface; j++)	{
			for (k = 0; k < 3; k++)	{
				v = faces[j].v[k];
				glVertexAttrib3fv(normalAttrib, &(verts[v].normal));			// normal
				if (m_bPlay) {
					glVertexAttrib4fv(weightAttrib, verts[v].weight);			// weights
					glVertexAttrib4fv(indexAttrib, verts[v].bone);				// indices
				}
				glVertexAttrib3fv(positionAttrib, &(verts[v].coord));
			}
		}
		glEnd();
	}
	
	glPopMatrix();
}

void HWDraw::DrawWire()
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawFill();
	glPopAttrib();
}

void HWDraw::DrawPoints()
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	DrawFill();
	glPopAttrib();
}

void HWDraw::DrawHidden()
{
	glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	DrawFill();
	glPopAttrib();
}

void HWDraw::DrawFillWire()
{
}