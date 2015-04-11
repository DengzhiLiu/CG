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
	float	xl, xr;				// ɨ��������������ߺ�����ߵĽ���
	float	delta_xl, delta_xr; // ����һ��ɨ����xl��xr������
	int		delta_yl, delta_yr; // ������ߺ��ұ�ɨ��������
	float   Il, Ir;				// ɨ��������������ߺ�����߽���Ĺ�����
	float   delta_Il, delta_Ir; // �����������߹����ȵ�����
	float	zl;					// ����߽�����������Ĵ�����ε����
	float	delta_zx, delta_zy; // z������
}Edge;

class CScanZBuffer
{
//	Attributes
private:
	int	  width, height;				// �ӿڵĿ�Ⱥ͸߶�
	float kd, ka;						// �����������ʺ�������ϵ��
	CDVector3 m_Light_Pos;
	float m_WLight, m_ALight;
	vector< Edge >	edge_list;
	vector< float > light_vertex;		  // �洢ÿ�����㴦�Ĺ�����	
	vector< CFVector4 > face_coeff;		  // �洢ƽ�淽��ϵ��
	vector< vector<int> > line_bucket;	  // �洢�����Ͱ	
	vector< CDVector3 > projected_vertex; // �洢ͶӰ��Ĵ�������

//	Operations
public:
	CScanZBuffer(void);
	~CScanZBuffer(void);

	void	SortFace(t3DObject object);
	void	SortFaceVertex(t3DObject *pObject);
	void	AddEdge(t3DObject object, int y);				//  ��ÿ��ɨ��ߵ���Ϣ
	void	Initial(t3DObject object, int light);
	void	Transform(t3DModel *p3DModel, int light, int color);
};
