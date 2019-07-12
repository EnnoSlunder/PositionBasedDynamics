#ifndef GLWIDGET_H
#define GLWIDGET_H

// Qt
#include <QOpenGLWindow>
#include <QOpenGLWidget>
#include <QPainter>
#include <QTimer>
#include <QElapsedTimer>
#include <QTime>
#include <QCoreApplication>
#include <QThread>
#include  <QKeyEvent>

// Project
#include "inputManager.h"
#include "dynamics/dynamicsWorld.h"
#include "activeobject.h"


// forward declare class Scene, so Window can be constructed with the Scene null-pointer
//class AbstractScene;
class Scene;

class Scene;

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
//    GLWidget();
    GLWidget(QWindow *parent = nullptr);
    ~GLWidget();
    void setScene(Scene *_scene);

     Scene* scene() const;
     void countFPS();
     void renderText();
     ActiveObject* activeObject();
     QPoint localMouseToGLScreenCoord(const QPoint &_localMouse);
     QPointF localMouseToGLNDCCoord(const QPointF &_localMouse);
     QPoint getMouseScreenCoords();
     QPointF getMouseNDCCoords();
     Tool tool();

     int elapsedTime();

protected slots:
    void update();
    void loop();
    void processInput();
    void uiTransformChange(const QVector3D _t, const QVector3D _r, const QVector3D _s);

    void printVersionInformation();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int _w, int _h);

private:
  bool m_simulating;
  double lag;
  float fpsRate, fpsCount;
  double previous, current, elapsed, second, render;

  double msDynamics, msRender, simPerFrame = 0;
  int inputUpdateTime;

  Tool m_tool;
  ActiveObject *m_activeObject;
  QTimer m_timer;
  QElapsedTimer m_elpasedTimer;
  Scene *m_scene = nullptr;
  inputManager m_inputManger;

};

#endif // GLWIDGET_H
