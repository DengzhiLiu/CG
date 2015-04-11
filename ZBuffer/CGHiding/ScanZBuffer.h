#pragma once

#include "OpenGL.h"
#include "Vector3.h"
#include "ObjInfo.h"
#include <vector>
#include <math.h>
using namespace std;

#define  min_z	-10000

typedef GLubyte arrbyte[3];

typedef struct  Edge_Info
{
	int		face_no;
	float	xl, xr;				// 扫描线与多边形左面边和右面边的交点
	float	delta_xl, delta_xr; // 到下一条扫描线xl和xr的增量
	int		delta_yl, delta_yr; // 穿过左边和右边扫描线条数
	float   Il, Ir;				// 扫描线与多边形左面边和右面边交点的光亮度
	float   delta_Il, delta_Ir; // 左面边与右面边光亮度的增量
	float	zl;					// 左面边交点的像素中心处多边形的深度
	float	delta_zx, delta_zy; // z的增量
}Edge;

class CScanZBuffer
{
//	Attributes
private:
	int	  width, height;				// 视口的宽度和高度
	float kd, ka;						// 定义漫反射率和漫反射系数
	CDVector3 m_Light_Pos;
	float m_WLight, m_ALight;
	vector< Edge >	edge_list;
	vector< float > light_vertex;		  // 存储每个顶点处的光亮度	
	vector< CFVector4 > face_coeff;		  // 存储平面方程系数
	vector< vector<int> > line_bucket;	  // 存储多边形桶	
	vector< CDVector3 > projected_vertex; // 存储投影后的窗口坐标

//	Operations
public:
	CScanZBuffer(void);
	~CScanZBuffer(void);

	void	SortFace(t3DObject object);
	void	SortFaceVertex(t3DObject *pObject);
	void	AddEdge(t3DObject object, int y);				//  加每条扫描边的信息
	void	Initial(t3DObject object, int light);
	void	Transform(t3DModel *p3DModel, int light, int color);
};
