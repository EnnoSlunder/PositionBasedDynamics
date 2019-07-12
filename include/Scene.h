#ifndef SCENE_H
#define SCENE_H

// STL
#include <vector>
#include <memory>
#include <unordered_map>

//Qt
#include <QMatrix4x4>
#include <QVector3D>
#include <QTransform>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLShader>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLTexture>
#include <QDir>
#include <QFile>

// Project
#include "AbstractScene.h"
#include "GLWidget.h"
#include "transform.h"
#include "camera.h"
#include "sceneobject.h"
#include "shape.h"
#include "model.h"
#include "utils.h"
#include "manipulator.h"
#include "dynamics/dynamicsWorld.h"
#include "dynamics/collisiondetection.h"
#include "Framebuffer.h"

class Scene : public AbstractScene
{
  static int numCreation;
public:
  Scene(GLWidget *_widget);
  ~Scene();
  void initialize();
  void resize(int width, int height);
  void paint();
  void updateSceneObjects();
  void drawScreenQuad();

  void setDynamicsWorld(DynamicsWorld *_world);
  void QtOpenGLinitialize();
  void DynamicsInitialize();
  void setupScene();
  void updateLinesVBO();
  void updatePointsVBO();

  void initFramebuffer();

  void debug(const QVector3D &_pos);

  // creation
  void addShape(Scene *_scene, std::string _name, const QVector3D* _data, int _size);
  ModelPtr addModel(Scene *_scene, std::string _name, std::string _path);

  pSceneOb addSceneObjectFromModel(std::string _name, uint _materialID, const QVector3D &_pos, const QQuaternion &_rot);
  pSceneOb addSceneObjectFromParticle(const DynamicObjectPtr _particle, ParticlePtr _p, int matID = 0);

  LightPtr addPointLight();
  LightPtr addPointLight(const QVector3D &_pos, const QVector3D &_color);
  MaterialPtr addMaterial(const QVector3D &_ambient, const QVector3D &_diffuse, const QVector3D &_specular, float _shininess);
  void addLine(const QVector3D &_start, const QVector3D &_end);

  void drawLines();
  void drawLinesAsPolygons();
  void drawPoints();
  void drawGeometryShader();

  // accessor
  uint width();
  uint height();
  ShapePtr getShapeFromPool(std::string _key);
  ModelPtr getModelFromPool(std::string _key);
  MaterialPtr getMaterial(int _index);
  pSceneOb getSceneObjectFromIndex(int _index);
  pSceneOb getPointerFromSceneObject(const SceneObject *_sceneObject);

  DynamicsWorld* dynamicsWorld();

  Ray castRayFromCamera(float ndcX, float ndcY, float depthZ  = -1.0);
  pSceneOb pickObject(float ndcX, float ndcY);
  int3 readPixel(uint _x, uint _y);

  CollisionDetection m_CollisionDetect;
  QVector3D currentRayStart = QVector3D(0,0,0);
  QVector3D currentRayEnd = QVector3D(0,0,0);

private:
  int mtest = 0;
    int widthT, heightT;

  friend class GLWidget;
  friend class Manipulator;

  DynamicsWorld *m_DynamicsWorld;

  QOpenGLShaderProgram* m_activeProgram;
  QOpenGLShaderProgram* m_screen_program;
  QOpenGLShaderProgram* m_lighting_program;
  QOpenGLShaderProgram* m_flat_program;
  QOpenGLShaderProgram* m_manipulator_program;
  QOpenGLShaderProgram* m_picking_program;
  QOpenGLShaderProgram* m_skybox_program;
  QOpenGLShaderProgram* m_wireframe_program;
  QOpenGLShaderProgram* m_wireframe_lines_program;

  std::vector<QVector3D> somePoints;
  QOpenGLVertexArrayObject* pointsVAO;
  QOpenGLBuffer pointsVBO;

  int SCR_HEIGHT, SCR_WIDTH;
  unsigned int fbo, texture, rbo, intermediateFBO, screenTexture, tmpFbo, tmpText;

  QOpenGLTexture* m_view_position_texture;
  QOpenGLVertexArrayObject* m_quad_vao;
  QOpenGLBuffer m_quad_vbo;

  QOpenGLVertexArrayObject* m_lines_vao;
  QOpenGLBuffer m_lines_vbo;

  QMatrix4x4 m_projection_matrix;
  Camera3D m_arcCamera;

  ShapeMap m_ShapePool;
  ModelMap m_ModelPool;

  Manipulator* mainpulator;
  Framebuffer* framebuffer;

  std::vector <LightPtr> m_Pointlights;
  std::vector <MaterialPtr> m_Materials;
  std::vector <pSceneOb> m_SceneObjects;
  std::vector <QVector3D> m_Lines;
  std::vector <QVector3D> m_Points;

  std::shared_ptr<PinConstraint>  m_pinnCstr_1 = nullptr;
  std::shared_ptr<PinConstraint>  m_pinnCstr_2 = nullptr;
  std::shared_ptr<PinConstraint>  m_pinnCstr_3 = nullptr;
  std::shared_ptr<PinConstraint>  m_pinnCstr_4 = nullptr;

  pSceneOb m_pickedObject;
};



inline MaterialPtr Scene::getMaterial(int _index){ return m_Materials[_index]; };
inline pSceneOb Scene::getSceneObjectFromIndex(int _index){ return m_SceneObjects[_index]; };
inline LightPtr Scene::addPointLight(){ return addPointLight(QVector3D(0,0,0), QVector3D(1,1,1)); };


#endif // SCENE_H
