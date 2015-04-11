#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#ifdef  WIN32
#include <windows.h>
#endif

#include <GL/glut.h>

#include "Clip.h"
#include "skinning.h"

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent = 0);
	~GLWidget();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

public slots:
		void setXRotation(int angle);
		void setYRotation(int angle);
		void setZRotation(int angle);

private slots:
		void advanceBVH();
	
signals:
		void xRotationChanged(int angle);
		void yRotationChanged(int angle);
		void zRotationChanged(int angle);

protected:
	void initializeGL();
	void drawMessage( int line_no, const char * message );
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent (QKeyEvent * event );
	void wheelEvent ( QWheelEvent * event );

	void normalizeAngle(int *angle);
	
private:
public:
	int xRot;
	int yRot;
	int zRot;
	float xTrans;
	float zTrans;
	float   camera_distance; 
	int     win_width, win_height;
	QPoint lastPos;	

	bool   on_animation;
	float  animation_time ;
	int    frame_no;
	Clip *   clip ;
	Skin* skin;
};

#endif
