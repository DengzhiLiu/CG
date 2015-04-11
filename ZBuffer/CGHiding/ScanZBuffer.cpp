#include "StdAfx.h"
#include "ScanZBuffer.h"

inline int round(double x)
{   
	return (x > 0.0) ? (x + 0.5) : (x + 0.5);
}  

CScanZBuffer::CScanZBuffer(void):
			width(800),height(600),
				kd(1.0),ka(0.5)
{
}

CScanZBuffer::~CScanZBuffer(void)
{
	line_bucket.clear();
	edge_list.clear();
	face_coeff.clear();
	projected_vertex.clear();
}

void	CScanZBuffer::Initial(t3DObject object, int light)
{
	int vertex_num;
	int viewport[4];
	double modelview[16], projection[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	// 初始化多边形分类表和投影矩阵
	line_bucket.clear();
	line_bucket.resize(height);
	for (int i = 0; i < height; i++)
		line_bucket[i].clear();
	vertex_num = object.numOfVerts;
	light_vertex.resize(vertex_num);
	projected_vertex.resize(vertex_num);
	for (int i = 0; i < vertex_num; i++)
	{
		gluProject((GLdouble)object.pVerts[i].x, (GLdouble)object.pVerts[i].y, (GLdouble)object.pVerts[i].z,
			modelview, projection, viewport,
			&projected_vertex[i].x,
			&projected_vertex[i].y,
			&projected_vertex[i].z);
	}
	for (int i = 0; i < vertex_num; i++)
	{
		projected_vertex[i].x = round(projected_vertex[i].x);
		projected_vertex[i].y = round(projected_vertex[i].y);
	}
	gluProject((GLdouble)g_LightPos.x, (GLdouble)g_LightPos.y, (GLdouble)g_LightPos.z, modelview, projection, viewport,
				&m_Light_Pos.x, &m_Light_Pos.y,	&m_Light_Pos.z);

	// 求所有顶点的光亮度值
	if (light)			// 点光源光照,Lambert漫反射模型
	{
		CDVector3 direction;
		m_ALight = g_IALight * ka;
		m_WLight = g_IWLight * kd;
		for (int i = 0; i < vertex_num; i++)
		{
			direction = m_Light_Pos - projected_vertex[i];
			direction = direction / sqrt(direction.Norm());
			light_vertex[i] = abs(object.pNormals[i].x * direction.x + object.pNormals[i].y * direction.y + 
								object.pNormals[i].z * direction.z);
			light_vertex[i] = light_vertex[i] * m_WLight + m_ALight;
		}
	}
	else			//	平行光照					
	{
		for (int i = 0; i < vertex_num; i++)
			light_vertex[i] = abs(object.pNormals[i].z) / sqrt(object.pNormals[i].Norm());
		/*	light_vertex[i] = abs((modelview[2] * object.pNormals[i].x + modelview[6] * object.pNormals[i].y + modelview[10] * object.pNormals[i].z) /
			object.pNormals[i].Norm());*/
	}
	
	// 平面的方程系数
	CVector3<double> vVector1, vVector2, vNormal, vPoly[3];
	// 分配需要的存储空间
	face_coeff.resize(object.numOfFaces);

	// 遍历对象的所有面
	for(int i = 0; i < object.numOfFaces; i++)
	{												
		vPoly[0] = projected_vertex[object.pFaces[i].vertIndex[0]];
		vPoly[1] = projected_vertex[object.pFaces[i].vertIndex[1]];
		vPoly[2] = projected_vertex[object.pFaces[i].vertIndex[2]];

		vVector1 = vPoly[0] - vPoly[1];		// 获得多边形的矢量
		vVector2 = vPoly[0] - vPoly[2];		// 获得多边形的第二个矢量
		vNormal  = vVector1 * vVector2;		// 获得两个矢量的叉积
		face_coeff[i].a = vNormal.x;				
		face_coeff[i].b = vNormal.y;
		face_coeff[i].c = vNormal.z;
		face_coeff[i].d = -(vNormal.x*vPoly[0].x + vNormal.y*vPoly[0].y + vNormal.z*vPoly[0].z);
	}
}

//	将面的顶点排序，并按逆时针顺序排列
void	CScanZBuffer::SortFaceVertex(t3DObject *pObject)
{
	for (int i = 0; i < pObject->numOfFaces; i++)
	{
		CDVector3	lowy, midy, highy;
		int lowy_index, midy_index, highy_index;
		highy_index = pObject->pFaces[i].vertIndex[0];
		midy_index = pObject->pFaces[i].vertIndex[1];
		lowy_index = pObject->pFaces[i].vertIndex[2];
		highy = projected_vertex[pObject->pFaces[i].vertIndex[0]];
		midy = projected_vertex[pObject->pFaces[i].vertIndex[1]];
		lowy = projected_vertex[pObject->pFaces[i].vertIndex[2]];
		if (lowy.y > midy.y)
		{
			swap(lowy, midy);
			swap(lowy_index, midy_index);
		}
		if (lowy.y > highy.y)
		{
			swap(lowy, highy);
			swap(lowy_index, highy_index);
		}
		if (midy.y > highy.y)
		{
			swap(midy, highy);
			swap(midy_index, highy_index);
		}
		if (round(highy.y) == round(midy.y))
		{
			if (highy.x > midy.x)
				swap(highy_index, midy_index);
		}
		else
		{
			if (midy.x > lowy.x)
				swap(midy_index, lowy_index);
		}
		pObject->pFaces[i].vertIndex[0] = highy_index;
		pObject->pFaces[i].vertIndex[1] = midy_index;
		pObject->pFaces[i].vertIndex[2] = lowy_index;
	}
}

void	CScanZBuffer::SortFace(t3DObject object)
{
	int max_y;
	// 给每个面的顶点坐标排序
	SortFaceVertex(&object);
	for (int i = 0; i < object.numOfFaces; i++)
	{
		max_y = round(projected_vertex[object.pFaces[i].vertIndex[0]].y);
		// 考虑边界出界的情况
		if (max_y < height && max_y >= 0)	line_bucket[max_y].push_back(i);
	}
}

void	CScanZBuffer::AddEdge(t3DObject object, int y)
{
	for (vector<int>::iterator iter = line_bucket[y].begin(); iter != line_bucket[y].end(); ++iter)
	{
		Edge edge;
		float temp1;
		int	n = *iter;			// n为面的坐标
		CDVector3	vPoly[3];
		
		edge.face_no = n;
		vPoly[0] = projected_vertex[object.pFaces[n].vertIndex[0]];
		vPoly[1] = projected_vertex[object.pFaces[n].vertIndex[1]];
		vPoly[2] = projected_vertex[object.pFaces[n].vertIndex[2]];
		// 最高两顶点连线为水平边时的情况
		if (round(vPoly[0].y) == round(vPoly[1].y))
		{
			edge.xl	= vPoly[0].x;
			edge.xr = vPoly[1].x;
			edge.delta_xl = -(vPoly[0].x - vPoly[2].x) / (vPoly[0].y - vPoly[2].y);
			edge.delta_xr = -(vPoly[1].x - vPoly[2].x) / (vPoly[1].y - vPoly[2].y);
			edge.delta_yl = round(vPoly[0].y) - round(vPoly[2].y) + 1;
			edge.delta_yr = round(vPoly[1].y) - round(vPoly[2].y) + 1;
			edge.Il = light_vertex[object.pFaces[n].vertIndex[0]];
			edge.Ir = light_vertex[object.pFaces[n].vertIndex[1]];
			temp1 = light_vertex[object.pFaces[n].vertIndex[2]];
			edge.delta_Il = (edge.Il - temp1) / edge.delta_yl;
			edge.delta_Ir = (edge.Ir - temp1) / edge.delta_yr;
		}
		else
		{
			edge.xl = edge.xr = vPoly[0].x;
			edge.delta_xl = -(vPoly[0].x - vPoly[1].x) / (vPoly[0].y - vPoly[1].y);
			edge.delta_xr = -(vPoly[0].x - vPoly[2].x) / (vPoly[0].y - vPoly[2].y);
			edge.delta_yl = round(vPoly[0].y) - round(vPoly[1].y) + 1;
			edge.delta_yr = round(vPoly[0].y) - round(vPoly[2].y) + 1;
			edge.Il = edge.Ir = light_vertex[object.pFaces[n].vertIndex[0]];
			edge.delta_Il = (edge.Il - light_vertex[object.pFaces[n].vertIndex[1]]) / edge.delta_yl;
			edge.delta_Ir = (edge.Ir - light_vertex[object.pFaces[n].vertIndex[2]]) / edge.delta_yr;
		}
		edge.zl = -(face_coeff[n].a * edge.xl + face_coeff[n].b * y + face_coeff[n].d) / face_coeff[n].c;
		edge.delta_zx = face_coeff[n].a / face_coeff[n].c;
		edge.delta_zy = face_coeff[n].b / face_coeff[n].c;
		edge_list.push_back(edge);
	}
}

void	CScanZBuffer::Transform(t3DModel *p3DModel, int light_type, int color_type)
{
	float *gcolor;
	arrbyte *rgbcolor;
	int viewport[4];
	CScanZBuffer m_scaner;
	glGetIntegerv(GL_VIEWPORT, viewport);
	width = viewport[2];
	height = viewport[3];
	float *scan_zbuffer = new float[width];
	gcolor = new float[width * height];
	rgbcolor = new arrbyte[width * height];
	for(int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			gcolor[j * width + i] = 1.0;
			rgbcolor[j * width + i][0] = 255;
			rgbcolor[j * width + i][1] = 255;
			rgbcolor[j * width + i][2] = 255;
		}
	
	for (int i = 0; i < p3DModel->numOfObjects; i++)
	{
		t3DObject	object = p3DModel->pObject[i];
		Initial(object, light_type);
		// 将每个面按最大y值进行分桶
		SortFace(object);
		
		// 进行扫描线ZBuffer算法
		for (int y = height-1; y >= 0; y--)
		{
			//vector<int> frame_buffer(width, -1);		// 帧缓冲区
			for (int i = 0; i < width; i++)
				scan_zbuffer[i] = min_z;
			
			if (line_bucket[y].size() > 0)
				AddEdge(object, y);
			if (edge_list.size() == 0)
				continue;
			for (vector< Edge >::iterator iter = edge_list.begin(); iter != edge_list.end();)
			{
				int n = iter->face_no;
				int x0 = max(round(iter->xl), 0);
				int xn = min(round(iter->xr), width-1);
				double z = iter->zl;
				double light  = iter->Il;
				double deltaI = (iter->Ir - iter->Il) / (xn - x0 + 1);
				for (int x = x0; x <= xn; x++)
				{
					if (scan_zbuffer[x] < z)
					{
						scan_zbuffer[x] = z;
						//frame_buffer[x] = n;
						gcolor[y * width + x] = light;
						rgbcolor[y * width + x][0] = (GLubyte)(light * 64);
						rgbcolor[y * width + x][1] = (GLubyte)(light * 128);
						rgbcolor[y * width + x][2] = (GLubyte)(light * 255);
					}
					z -= iter->delta_zx;
					light += deltaI;
				}
				iter->delta_yl--;
				iter->delta_yr--;
				// 面片已扫描完时的情况
				if ((iter->delta_yl == 0) && (iter->delta_yr == 0))
				{
					iter = edge_list.erase(iter);
					if (edge_list.size() == 0)		break;
					else	continue;
				}
				else if (iter->delta_yl == 0)	// 左面边扫描完时的情况
				{
					CDVector3	vPoly[2];
					int	n = iter->face_no;
					vPoly[0] = projected_vertex[object.pFaces[n].vertIndex[1]];
					vPoly[1] = projected_vertex[object.pFaces[n].vertIndex[2]];
					iter->xl = vPoly[0].x;
					iter->delta_xl = -(vPoly[0].x - vPoly[1].x) / (vPoly[0].y - vPoly[1].y);
					iter->delta_yl = y - round(vPoly[1].y);
					iter->xr += iter->delta_xr;
					iter->Il = light_vertex[object.pFaces[n].vertIndex[1]];
					iter->delta_Il = (iter->Il - light_vertex[object.pFaces[n].vertIndex[2]]) / iter->delta_yl;
					iter->Ir += iter->delta_Ir;
					iter->zl -= (iter->delta_zx * iter->delta_xl + iter->delta_zy);
				}
				else if (iter->delta_yr == 0)	// 右面边扫描完时的情况
				{
					CDVector3	vPoly[2];
					int	n = iter->face_no;
					vPoly[0] = projected_vertex[object.pFaces[n].vertIndex[2]];
					vPoly[1] = projected_vertex[object.pFaces[n].vertIndex[1]];
					iter->xr = vPoly[0].x;
					iter->delta_xr = -(vPoly[0].x - vPoly[1].x) / (vPoly[0].y - vPoly[1].y);
					iter->delta_yr = y - round(vPoly[1].y);
					iter->xl += iter->delta_xl;
					iter->Il += iter->delta_Il;
					iter->Ir = light_vertex[object.pFaces[n].vertIndex[2]];
					iter->delta_Ir = (iter->Ir - light_vertex[object.pFaces[n].vertIndex[1]]) / iter->delta_yr;
					iter->zl -= (iter->delta_zx * iter->delta_xl + iter->delta_zy);
				}
				else							// 左面边和右面边都没扫描完时的情况
				{
					iter->xl += iter->delta_xl;
					iter->xr += iter->delta_xr;
					iter->Il += iter->delta_Il;
					iter->Ir += iter->delta_Ir;
					iter->zl -= (iter->delta_zx * iter->delta_xl + iter->delta_zy);
				}
				++iter;
			}
		}

	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(0, 0);
	if (color_type == 0)
		glDrawPixels(width, height, GL_LUMINANCE, GL_FLOAT, gcolor);
	else
		glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, rgbcolor);
	glPopMatrix();
	glFinish();
	delete scan_zbuffer;
	delete gcolor;
	delete rgbcolor;
	::SwapBuffers(wglGetCurrentDC());
}