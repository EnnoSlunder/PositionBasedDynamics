#include "include/GLWidget.h"


#include "Scene.h"


// STL
#include <iostream>
#include <algorithm>

// QT
#include <QDebug>
#include <QKeyEvent>
#include <QString>
#include <QString>
#include <QCursor>

// Project
#include "utils.h"


//GLWidget::GLWidget()
//{

//}

GLWidget::GLWidget(QWindow *parent) : QOpenGLWidget()
{
    this->setUpdateBehavior(QOpenGLWidget::PartialUpdate);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(loop()));
    m_timer.setInterval(0);
    m_elpasedTimer.start();
    second = 0;
    previous = m_elpasedTimer.elapsed();
    lag = 0.0;
    render = 0.0;

    m_timer.start();
    this->setWindowTitle("QOpenGLWidget");

    m_activeObject = new ActiveObject(this);
}

GLWidget::~GLWidget()
{
    delete m_activeObject;
}

void GLWidget::setScene(Scene *_scene)
{
    m_scene = _scene;
}

Scene *GLWidget::scene() const
{
    return m_scene;
}

void GLWidget::update()
{
    QOpenGLWidget::update();
}

void GLWidget::loop()
{

//    if(m_elpasedTimer.elapsed() > 5000)
//        close();

    QElapsedTimer timer;
    timer.start();
    countFPS();
    processInput();
    inputUpdateTime = timer.elapsed();

    double timeStepSize  = scene()->dynamicsWorld()->getTimeStepSizeMS();
    int simFrameCounter = 0;

    //// WIP play sim in Realtime
    while(lag >= timeStepSize)
    {
        simFrameCounter++;
//        mlog<<"lag:"<<lag;
//        timer.start();
//        scene()->updateSceneObjects();
//        scene()->dynamicsWorld()->update();
//        msDynamics = timer.elapsed();
//        timer.restart();

        lag -= timeStepSize;
    }
//       mlog<<simFrameCounter;
    simPerFrame = simFrameCounter;

//    QThread::msleep(16);

//    //// play every sim frame
    timer.start();
    scene()->updateSceneObjects();
    scene()->dynamicsWorld()->update();
    msDynamics = timer.elapsed();
    timer.restart();


//    mlog<<"physics step2: "<<timer.elapsed();
    if(render < 16)
    {
        render += elapsed;
//        qDebug()<<elapsed;
    }
    else
    {
//        qDebug()<<"update";
        render = 0;

//        timer.start();
        QOpenGLWidget::update();
//        QThread::msleep(10);
//        msRender = timer.elapsed();
//        timer.restart();
//        mlog<<msRender;
    }
}

void GLWidget::countFPS()
{
    current = m_elpasedTimer.elapsed();
    elapsed = current - previous;

    previous = current;

    lag += elapsed;

    if(second < 1000)
    {
        second += elapsed;
        fpsCount += 1;
    }
    else
    {
//        qDebug()<<"calc"<<elapsed;
        fpsRate = fpsCount;
        fpsCount = 0;
        second = 0;
    }
}

void GLWidget::renderText()
{
    QPainter painter(this);
    painter.setPen(QColor(0,0,0));

    float renderFPS = 1000 / msRender;
    QString renderFPStext = QString::number(renderFPS);
    if(isinf(renderFPS)){
        renderFPS = 60;
        renderFPStext =  QString::number(renderFPS) + " +";
    }
     QString fps = " fps: " + renderFPStext;
    float simFPS = int(1000.0 / msDynamics);
    QString simFPStext = QString::number(simFPS);

    if(msDynamics == 0){
        simFPStext =  "-";
    }
    QString a = " sim fps:  " + simFPStext;
    QString b = " sim frame:  " + QString::number(scene()->dynamicsWorld()->frameCount());

    painter.drawText(QRect(5, 5,  200, 50), fps);
    painter.drawText(QRect(5, 19, 200, 50), a);
    painter.drawText(QRect(5, 33, 200, 50), b);
}

ActiveObject *GLWidget::activeObject()
{
    if(m_activeObject)
        return  m_activeObject;

    return nullptr;
}

QPoint GLWidget::localMouseToGLScreenCoord(const QPoint &_localMouse)
{
    //    int pixelOffset = 23;
        int pixelOffset = 0;
        return QPoint(_localMouse.x() *2 + pixelOffset, (this->height()*2) - _localMouse.y() *2 + pixelOffset);
}

QPointF GLWidget::localMouseToGLNDCCoord(const QPointF &_localMouse)
{
    float x = (_localMouse.x() + 0.5) / this->width();
    float y = (_localMouse.y() + 0.5) / this->height();
    float NDCx = 2 * x - 1;
    float NDCy = 1 - 2 * y;
    return QPointF(NDCx,NDCy);
}

QPoint GLWidget::getMouseScreenCoords()
{
    return localMouseToGLScreenCoord(mapFromGlobal(QCursor::pos()));
}

QPointF GLWidget::getMouseNDCCoords()
{
    QPoint pos = QCursor::pos();
    return localMouseToGLNDCCoord(this->mapFromGlobal(pos));

}

Tool GLWidget::tool()
{
    return m_tool;
}

int GLWidget::elapsedTime()
{
    return current;
}

void GLWidget::processInput()
{
    // update() registers keys, add/remove from inputManager containers
    m_tool = MANIPULATOR_TR;
    switch (m_tool) {
        case MANIPULATOR_TR:
            if(scene()->mainpulator)
            {
                scene()->mainpulator->update();
            }
            break;
    }
    // convert Mouse window coordinates to have origin (0,0) at center of image
    QPoint _localMousePos = this->mapFromGlobal(QCursor::pos());
    _localMousePos.setX(_localMousePos.x() - (this->width()/2));
    _localMousePos.setY((_localMousePos.y() - (this->height()/2)) * -1);
    inputManager::update(_localMousePos);

    // handle key press events
    if(inputManager::buttonTriggered(Qt::LeftButton) )
    {
        QPointF toPick = getMouseNDCCoords();
        if(scene()->mainpulator->currentState == NONE)
        {
            auto picked = scene()->pickObject(toPick.x(), toPick.y());
            if(picked)
            {
                picked->update();
                scene()->mainpulator->currentState = TRANSLATE_VIEWPLANE;
//                activeObject()->setState(ActiveObject::SELECTED);
                activeObject()->select();
            }
            else
            {
                activeObject()->deselect();
            }
        }
        m_inputManger.setMouseTriggeredPosition();
        scene()->m_arcCamera.arcBallStart();
//        scene()->mainpulator->setState(TRANSLATE_VIEWPLANE);

//        if notDragging:
        scene()->mainpulator->startDrag();


    }

    if(inputManager::buttonPressed(Qt::LeftButton) && inputManager::keyPressed(Qt::Key_Alt ))
    {
        int _radius = std::min(this->width(), this->height()) / 2;
        scene()->m_arcCamera.orbit(m_inputManger.mousePosition(), m_inputManger.mouseTriggeredPosition());
    }
    if(inputManager::buttonPressed(Qt::LeftButton) && m_tool == MANIPULATOR_TR )
    {
        scene()->mainpulator->drag();
    }
    if(inputManager::buttonReleased(Qt::LeftButton) && m_tool == MANIPULATOR_TR )
    {
        scene()->mainpulator->endDrag();
    }
    if(inputManager::buttonReleased(Qt::LeftButton)  )
    {

    }
    if(inputManager::buttonTriggered(Qt::RightButton) )
    {
        scene()->m_arcCamera.trackStart(m_inputManger.mousePosition());
    }
    if(inputManager::buttonPressed(Qt::RightButton) )
    {
        scene()->m_arcCamera.dolly(m_inputManger.mousePosition());
    }
    if(inputManager::buttonTriggered(Qt::MidButton))
    {
        scene()->m_arcCamera.trackStart(m_inputManger.mousePosition());
    }
    if(inputManager::buttonPressed(Qt::MidButton))
    {
        scene()->m_arcCamera.track(m_inputManger.mousePosition());
    }
    if(inputManager::keyTriggered(Qt::Key_P))
    {
//        mlog<<"activeObject()->currentObject()->isDynamic() "<<activeObject()->currentObject()->isDynamic();
//        if(activeObject()->isActive() && activeObject()->currentObject()->isDynamic())

           activeObject()->processInput(ActiveObject::B_P_PRESSED);
    }
    if(inputManager::keyTriggered(Qt::Key_T))
    {
           activeObject()->processInput(ActiveObject::B_T_PRESSED);
    }
    if(inputManager::keyTriggered(Qt::Key_D))
    {
           activeObject()->processInput(ActiveObject::B_D_PRESSED);
    }



}

void GLWidget::uiTransformChange(const QVector3D _t, const QVector3D _r, const QVector3D _s)
{
    m_activeObject->setTransform(_t, _r, _s);
}

void GLWidget::printVersionInformation()
{
    QString glType;
    QString glVersion;
    QString glProfile;

    // Get Version Information
    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    // Get Profile Information
  #define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
    switch (format().profile())
    {
      CASE(NoProfile);
      CASE(CoreProfile);
      CASE(CompatibilityProfile);
    }
  #undef CASE

    // qPrintable() will print our QString w/o quotes around it.
    qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";

}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
//    qDebug()<<"widget:";
//    printVersionInformation();
        if (event->isAutoRepeat())
        {
          event->ignore();
        }
        else
        {
          inputManager::registerKeyPress(event->key());
        }

        if (event->isAutoRepeat())
        {
    //      qDebug("event isAutoRepeat");
        }
        else
        {
            switch(event->key())
            {
                case Qt::Key_Right:{
                        qDebug()<<"register and call update";
                        scene()->dynamicsWorld()->update();}
                          break;

                case Qt::Key_W:{
                        scene()->m_Lines.clear();}
                        break;

                case Qt::Key_S:{
                        QMatrix4x4 i;
                        Transform test;
                        scene()->mainpulator->setTransform(test);}
                        break;

                case Qt::Key_B:{
                            QElapsedTimer timer;
                            timer.start();
                            scene()->setupScene();
                            mlog<<" setupScene() took: "<<timer.elapsed()<<" ms";
                            }
                        break;

                case Qt::Key_A:{
                            if(m_activeObject->isActive())
                            {
                                mlog<<"activeObject id:"<<m_activeObject->currentObject()->getID();
                            }
                        }
                        break;
                case Qt::Key_C:{
                                scene()->debug(QVector3D(0,0,0));
                        }
                        break;

                case Qt::Key_Space:{
                            if(m_simulating)
                            {
                                scene()->dynamicsWorld()->setSimulate(false);
                                m_simulating = false;
                            }
                            else
                            {
                                scene()->dynamicsWorld()->setSimulate(true);
                                m_simulating = true;
                            }
                        }
                        break;
            }
        }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
      event->ignore();
    }
    else
    {
      inputManager::registerKeyRelease(event->key());
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    inputManager::registerMousePress(event->button());
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    inputManager::registerMouseRelease(event->button());
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    QVector3D camTranslation = scene()->m_arcCamera.translation();
    float t = event->pixelDelta().y();

    QVector3D nextCamTranslation = camTranslation + (QVector3D(0,0,1) * t * 0.05);
    scene()->m_arcCamera.setTranslation(nextCamTranslation);
}

void GLWidget::initializeGL()
{
    qDebug()<<"init w";
    if(scene())
        scene()->initialize();
}

void GLWidget::paintGL()
{
    QElapsedTimer timer;
    timer.start();

    if (scene())
        scene()->paint();
    msRender = timer.elapsed();

    renderText();
}

void GLWidget::resizeGL(int _w, int _h)
{

    qDebug()<<"resize w"<<_w<<_h;
    if (scene())
    {
//        qDebug()<<"resize 2 w"<<_w<<_h;
      scene()->resize(_w, _h);
//      qDebug()<<"resize  3 w"<<_w<<_h;
    }
}
