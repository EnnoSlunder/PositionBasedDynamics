
#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

// Qt
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <QOpenGLWidget>
#include "GLWidget.h"

//class AbstractScene : protected QOpenGLFunctions
class AbstractScene : public QOpenGLFunctions
{

public:

//  AbstractScene(QOpenGLWindow *_window = nullptr);

//  AbstractScene(QOpenGLWidget *_widget = 0);

  AbstractScene(GLWidget *_widget = 0);
  virtual ~AbstractScene();
//  QOpenGLWindow* window() const;
  GLWidget* widget() const;
  QOpenGLContext* context();
  const QOpenGLContext* context() const;
  virtual void initialize();
  virtual void paint() = 0;

  void foo();

private:
//  QOpenGLWindow *m_window = nullptr;
  GLWidget *m_widget = nullptr;

};

#endif // ABSTRACTSCENE_H
