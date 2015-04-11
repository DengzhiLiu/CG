

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QSlider;
QT_END_NAMESPACE
class GLWidget;


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

protected:
	//void closeEvent(QCloseEvent *event);

private slots:
	void open();
	void openMeshFile();
	void openWeightFile();
private:
	//bool loadFile(const QString &fileName);
	//QString strippedName(const QString &fullFileName);
	QSlider *createSlider();
	QSlider *createPlay_PauseSlider();

	QString file_name;

	QWidget *centralWidget;

	QMenu *fileMenu;
	QAction *openAction;
	QAction *openMeshFileAction;
	QAction *openWeightFileAction;
	QAction *exitAction;

	GLWidget *glWidget;
	QSlider *xSlider;
	QSlider *ySlider;
	QSlider *zSlider;

	QSlider *Play_Pause;
};

#endif
