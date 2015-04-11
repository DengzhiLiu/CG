#include <QtGui>
#include <QtOpenGL>

#include "glwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow()
{	

	centralWidget = new QWidget;
	setCentralWidget(centralWidget);

	glWidget = new GLWidget;

	openAction = new QAction(tr("&Open..."), this);	
	openAction->setShortcut(QKeySequence::Open);	
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

	openMeshFileAction = new QAction(tr("&Open MeshFile"), this);	
	openMeshFileAction->setShortcut(QKeySequence::Open);	
	connect(openMeshFileAction, SIGNAL(triggered()), this, SLOT(openMeshFile()));

	openWeightFileAction = new QAction(tr("&Open WeightFile"), this);	
	openWeightFileAction->setShortcut(QKeySequence::Open);	
	connect(openWeightFileAction, SIGNAL(triggered()), this, SLOT(openWeightFile()));

	exitAction = new QAction(tr("&Exit"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));	
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	fileMenu = menuBar()->addMenu(tr("&File"));	
	fileMenu->addAction(openAction);
	fileMenu->addAction(openMeshFileAction);	
	fileMenu->addAction(openWeightFileAction);	
	fileMenu->addAction(exitAction);

	xSlider = createSlider();
	ySlider = createSlider();
	zSlider = createSlider();
	//Play_Pause = createPlay_PauseSlider();

	connect(xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXRotation(int)));
	connect(glWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
	connect(ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYRotation(int)));
	connect(glWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
	connect(zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZRotation(int)));
	connect(glWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));
	
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(glWidget);
	mainLayout->addWidget(xSlider);
	mainLayout->addWidget(ySlider);
	mainLayout->addWidget(zSlider);
	/*setLayout(mainLayout);*/
	centralWidget->setLayout(mainLayout);

	xSlider->setValue(15 * 16);
	ySlider->setValue(345 * 16);
	zSlider->setValue(0 * 16);

	setWindowTitle(tr("BVH Player"));
	 resize(500, 500);
}

QSlider *MainWindow::createSlider()
{
	QSlider *slider = new QSlider(Qt::Vertical);
	slider->setRange(0, 360 * 16);
	slider->setSingleStep(16);
	slider->setPageStep(15 * 16);
	slider->setTickInterval(15 * 16);
	slider->setTickPosition(QSlider::TicksRight);
	return slider;
}

QSlider *MainWindow::createPlay_PauseSlider()
{
	QSlider *slider = new QSlider(Qt::Horizontal);
	slider->setRange(0, 100);
	slider->setSingleStep(1);
	slider->setPageStep(10);
	slider->setTickInterval(15 * 16);
	slider->setTickPosition(QSlider::TicksRight);
	
	return slider;
}
void MainWindow::open()
{	
	file_name = QFileDialog::getOpenFileName(this,tr("Open BVH File"), ".",tr("BVH file (*.bvh)"));

	if ( glWidget->clip )
		delete  glWidget->clip;
	glWidget->clip = new Clip();
	bool LoadSuccess =glWidget->clip->readBVH(file_name.toStdString().c_str());	

	if (!LoadSuccess)
	{
		delete glWidget->clip;
		glWidget->clip = NULL;
	}

	glWidget->animation_time = 0.0f;
	glWidget->frame_no = 0;	
}

void MainWindow::openMeshFile()
{
	file_name = QFileDialog::getOpenFileName(this,tr("Open Mesh File"), ".",tr("Obj File (*.obj)"));

	glWidget->skin = new Skin(glWidget->clip,Gray ,false);//Wire

	glWidget->skin->readObjFile(file_name.toStdString());
}

void MainWindow::openWeightFile()
{
	file_name = QFileDialog::getOpenFileName(this,tr("Open Weight File"), ".",tr("Weight File (*.WEIGHT)"));
	glWidget->skin->readWeightInfo(file_name.toStdString());
}