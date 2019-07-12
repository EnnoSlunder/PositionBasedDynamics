#include <QtDebug>

#include "MainWindow.h"


MainWindow::MainWindow()
{
    controlWidget = new ControlWidget();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setGLController(GLWidget *_glw)
{
    glw = _glw;

    connect(glw->activeObject(),
            SIGNAL(transformChanged(const QMatrix4x4, const QVector3D, const QVector3D, const QVector3D, int)),
            controlWidget->transformWidget,
            SLOT(setTransform(const QMatrix4x4, const QVector3D, const QVector3D, const QVector3D, int)));

    connect(controlWidget->transformWidget,
            SIGNAL(matrixChanged(const QVector3D, const QVector3D, const QVector3D)),
            glw,
            SLOT(uiTransformChange(const QVector3D, const QVector3D, const QVector3D)));
}

void MainWindow::setDynamicsController(DynamicsWorldController *_dwc)
{
    dwc = _dwc;

    connect(controlWidget->dynamicsWidget->startSim,
            SIGNAL(clicked()),
            dwc,
            SLOT(startStopSim()));
    connect(controlWidget->dynamicsWidget->stepSim,
            SIGNAL(clicked()),
            dwc,
            SLOT(stepSim()));

      connect(controlWidget->dynamicsWidget->gravityLabelEditY, SIGNAL(valueChanged(float)), dwc, SLOT(setGravityY(float)));

      connect(controlWidget->dynamicsWidget->stepSizeEdit, SIGNAL(valueChanged(float)), dwc, SLOT(setTimeStepSize(float)));

      connect(controlWidget->dynamicsWidget->preConditionsIterEdit, SIGNAL(valueChanged(int)), dwc, SLOT(setPreConditionIteration(int)));

      connect(controlWidget->dynamicsWidget->constraintIterEdit, SIGNAL(valueChanged(int)), dwc, SLOT(setConstraintIteration(int)));

      connect(controlWidget->dynamicsWidget->particleMassEdit, SIGNAL(valueChanged(float)), dwc, SLOT(setParticleMass(float)));

      connect(controlWidget->dynamicsWidget->pbdDampingEdit, SIGNAL(valueChanged(float)), dwc, SLOT(setPBDDamping(float)));

      connect(controlWidget->dynamicsWidget->distanceConstraintStretchEdit, SIGNAL(valueChanged(float)), dwc, SLOT(setDistanceConstraintStretch(float)));

      connect(controlWidget->dynamicsWidget->distanceConstraintCompressEdit, SIGNAL(valueChanged(float)), dwc, SLOT(setDistanceConstraintCompress(float)));

      connect(controlWidget->dynamicsWidget->shapeMatchingConstraintAttractEdit, SIGNAL(valueChanged(float)), dwc, SLOT(setShapeMatchingConstraintAttract(float)));
}

MainWindow::MainWindow(GLWidget *_glw)
{
    controlWidget = new ControlWidget();
}

void MainWindow::setupUi(GLWidget *_glw)
{
    // create Widget
    mainWidget =  new QWidget;
    controlWidget = new ControlWidget();

    glw = _glw;

    connect(glw->activeObject(),
            SIGNAL(transformChanged(const QMatrix4x4, const QVector3D, const QVector3D, const QVector3D)),
            controlWidget->transformWidget,
            SLOT(setTransform(const QMatrix4x4, const QVector3D, const QVector3D, const QVector3D)));

    connect(controlWidget->transformWidget,
            SIGNAL(matrixChanged(const QVector3D, const QVector3D, const QVector3D)),
            glw,
            SLOT(uiTransformChange(const QVector3D, const QVector3D, const QVector3D)));

    // layout
    layout.addWidget(glw);
    layout.addWidget(controlWidget);

    mainWidget->setLayout(&layout);

    setCentralWidget(glw);
//    setCentralWidget(mainWidget);
    glw->setFocus(Qt::ActiveWindowFocusReason);

//    glw->setParent(mainWidget);
//    mainWidget->resize(750,1000);
//    glw->resize(900,720);
//    this->resize(1000,720);5

//        mainWidget->resize(2000,2000);
//        glw->resize(900,720);
//        this->resize(2000,2000);
}

void MainWindow::setupUi()
{
    mainWidget =  new QWidget;

    if(glw)
    {
        layout.addWidget(glw);
//        glw->resize(900,720);
        glw->setFocus(Qt::ActiveWindowFocusReason);
    }
    layout.addWidget(controlWidget);

    mainWidget->setLayout(&layout);
    setCentralWidget(mainWidget);
    mainWidget->resize(750,1000);
    this->resize(1000,750);

    // who is focusing away :( ?
    if(glw)
        glw->setFocus(Qt::ActiveWindowFocusReason);

}





















