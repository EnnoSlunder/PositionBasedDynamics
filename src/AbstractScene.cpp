
// Project
#include "AbstractScene.h"
#include <QDebug>

#include <iostream>
#include <memory>


void AbstractScene::foo()
{
    qDebug("foo foo ");
}


//AbstractScene::AbstractScene(QOpenGLWindow *_window)
//{
//    m_window = _window;
//}

AbstractScene::AbstractScene(GLWidget *_widget)
{
    m_widget = _widget;
}

AbstractScene::~AbstractScene()
{

}

GLWidget *AbstractScene::widget() const
{
    return m_widget;
}

QOpenGLContext *AbstractScene::context()
{
  return widget() ? widget()->context() : nullptr;
}

const QOpenGLContext *AbstractScene::context() const
{
  return widget() ? widget()->context() : nullptr;
}

void AbstractScene::initialize()
{
  initializeOpenGLFunctions();
}


