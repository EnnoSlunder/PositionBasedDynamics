#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QHBoxLayout>
#include <QWidget>
#include <QSplitterHandle>

#include "GLWidget.h"
#include "Scene.h"
#include "ControlWidget.h"
#include "dynamics/dynamicsWorldController.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow();
    MainWindow(GLWidget *_glw);
    ~MainWindow();

    void setGLController(GLWidget *_glw);
    void setDynamicsController(DynamicsWorldController *_dwc);

//protected:
    void setupUi(GLWidget *_glw);
    void setupUi();

protected slots:
    void foo(double ){ mlog<<"FUCK this ";}

private:
    QWidget *mainWidget;
    GLWidget *glw =  nullptr;
    DynamicsWorldController *dwc = nullptr;
    ControlWidget *controlWidget;
    QHBoxLayout layout;

};

#endif // MAINWINDOW_H
