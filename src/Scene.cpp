#include <QFile>
#include <QString>

#include "Scene.h"


#include <iostream>

int Scene::numCreation = 0;

Scene::Scene(GLWidget *_widget) : AbstractScene(_widget)
{
}

Scene::~Scene()
{

}

void Scene::initialize()
{
  m_CollisionDetect =  CollisionDetection();
  AbstractScene::initialize();

  {
      SCR_WIDTH = widget()->width() * 2;
      SCR_HEIGHT = widget()->height() * 2;
      qDebug()<<"widget"<<SCR_WIDTH<<SCR_HEIGHT;
  }
  QtOpenGLinitialize();
  DynamicsInitialize();
  setupScene();
}

void Scene::addShape(Scene *_scene, std::string _name, const QVector3D *_data, int _size)
{
    if(_scene->m_ShapePool[_name])
    {
        qDebug()<<_name.data()<<" already exists in ShapePool";
        return;
    }
    // create new smart pointer + instanciate new Shape
    auto pShape = std::make_shared<Shape>();
    // allocate data on the GPU
    pShape->allocate(_data, _size);
    // add Shape to Pool
    _scene->m_ShapePool[_name] = pShape;
}

ModelPtr Scene::addModel(Scene *_scene, std::string _name, std::string _path)
{
    if(_scene->m_ModelPool[_name])
    {
        qDebug()<<_name.data()<<" already exists in ModelPool";
        return nullptr;
    }
    auto pModel = std::make_shared<Model>();
    // load the model using assimp
    pModel->loadModel(_path);
    m_ModelPool[_name] = pModel;
    return pModel;
}

pSceneOb Scene::addSceneObjectFromModel(std::string _name,  uint _materialID,  const QVector3D &_pos, const QQuaternion &_rot)
{
    auto pModel = getModelFromPool(_name);
    if(pModel == nullptr)
    {
        qDebug()<<"WARNING: COULD NOT ADD SceneObject";
        return nullptr;
    }
    auto pSO = std::make_shared<SceneObject>(this, pModel, _materialID ,_pos, _rot);
    m_SceneObjects.push_back(pSO);

    // pass them the activeObject instance, so they can notify their observer
    pSO->setActiveObject(widget()->activeObject());
    numCreation++;
    pSO->setID(numCreation);
    return pSO;
}

pSceneOb Scene::addSceneObjectFromParticle(const DynamicObjectPtr _particle, ParticlePtr _p,  int matID)
{
    auto pModel = getModelFromPool("sphere");
    if(pModel == nullptr)
    {
        qDebug()<<"WARNING: COULD NOT ADD SceneObject as Particle";
        return nullptr;
    }

    auto pSO = std::make_shared<SceneObject>(this, pModel, matID , _particle->getTranslation());
    pSO->setActiveObject(widget()->activeObject());
    pSO->setRadius(_p->radius());

    numCreation++;
    pSO->setID(numCreation);
    m_SceneObjects.push_back(pSO);
    pSO->makeDynamic(_particle);
    pSO->isHidden(false);

    return pSO;
}

LightPtr Scene::addPointLight(const QVector3D &_pos, const QVector3D &_color)
{
    auto pLight = std::make_shared<Light>();
    pLight->position = _pos;
    pLight->color = _color;
    m_Pointlights.push_back(pLight);
    return pLight;
}

MaterialPtr Scene::addMaterial(const QVector3D &_ambient, const QVector3D &_diffuse, const QVector3D &_specular, float _shininess)
{
    auto pMaterial = std::make_shared<Material>();
    pMaterial->ambient = _ambient;
    pMaterial->diffuse = _diffuse;
    pMaterial->specular = _specular;
    pMaterial->shininess = _shininess;
    m_Materials.push_back(pMaterial);
    return pMaterial;
}

void Scene::addLine(const QVector3D &_start, const QVector3D &_end)
{
    m_Lines.push_back(_start);
    m_Lines.push_back(_end);
}

void Scene::drawLines()
{
    updateLinesVBO();
    {
      m_flat_program->setUniformValue("Color", QVector3D(0.0,0.8,0.0));
      m_lines_vao->bind();
      glDrawArrays(GL_LINES, 0, m_Lines.size() * 4);
      m_lines_vao->release();
    }
}

void Scene::drawLinesAsPolygons()
{
    QMatrix4x4 tmp;
    tmp.setToIdentity();
    m_wireframe_lines_program->bind();
    m_wireframe_lines_program->setUniformValue("projection", m_projection_matrix);
    m_wireframe_lines_program->setUniformValue("view", m_arcCamera.toMatrix());
    m_wireframe_lines_program->setUniformValue("model", tmp);
    m_lines_vao->bind();
    glDrawArrays(GL_LINES, 0, m_Lines.size() * 4);
    m_lines_vao->release();
}

void Scene::drawPoints()
{

}

void Scene::drawGeometryShader()
{
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     m_wireframe_program->bind();
     m_wireframe_program->setUniformValue("projection", m_projection_matrix);
     m_wireframe_program->setUniformValue("view", m_arcCamera.toMatrix());
     m_wireframe_program->setUniformValue("model",  m_SceneObjects[1]->getMatrix());
     pointsVAO->bind();
     glDrawArrays(GL_POINTS, 0, somePoints.size());
     pointsVAO->release();
}

uint Scene::width()
{
    return  SCR_WIDTH;
}

uint Scene::height()
{
    return  SCR_HEIGHT;
}

ShapePtr Scene::getShapeFromPool(std::string _key)
{
    ShapeMap::const_iterator got = m_ShapePool.find(_key);
    if ( got == m_ShapePool.end())
    {
        qDebug()<< "WARNING: NO SHAPE WAS FOUND";
    }
    else
    {
        auto _pShape = got->second;
        return _pShape;
    }
    return nullptr;
}

ModelPtr Scene::getModelFromPool(std::string _key)
{
    ModelMap::const_iterator got = m_ModelPool.find(_key);
    if ( got == m_ModelPool.end())
    {
        qDebug()<< "WARNING: NO MODEL WAS FOUND for getModelFromPool()"<<QString::fromStdString( _key);
    }
    else
    {
        auto _pShape = got->second;
        return _pShape;
    }
    return nullptr;
}

pSceneOb Scene::getPointerFromSceneObject(const SceneObject *_sceneObject)
{
    for (pSceneOb i : m_SceneObjects)
    {
        if(i.get() == _sceneObject)
            return i;
    }

    return nullptr;
}

DynamicsWorld* Scene::dynamicsWorld()
{
    return m_DynamicsWorld;
}

Ray Scene::castRayFromCamera(float ndcX, float ndcY, float depthZ)
{
    QVector4D ray_clip = QVector4D(ndcX,ndcY,depthZ, 1);
    QVector4D ray_eye = m_projection_matrix.inverted() * ray_clip;
    ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), -1, 0);
    QVector4D ray_wor = m_arcCamera.toMatrix().inverted() * ray_eye;
    QVector3D ray_world = QVector3D(ray_wor.x(), ray_wor.y(), ray_wor.z());
    QVector3D origin = m_arcCamera.toMatrix().inverted() * QVector3D(0,0,0);

    Ray cameraRay;
    cameraRay.Origin = origin;
    cameraRay.Dir = ray_world;
    return  cameraRay;
}

pSceneOb Scene::pickObject(float ndcX, float ndcY)
{
    Ray cameraRay = castRayFromCamera(ndcX, ndcY);
    float min_t = 0.0;
    float index = 0;

    if(m_pickedObject)
        m_pickedObject->isActive(false);
    m_pickedObject.reset();

    // Ray Sphere check
    for(uint i = 1; i < m_SceneObjects.size(); i++)
    {
        float t = 0.0;
        QVector3D point;
        m_CollisionDetect.intersectRaySphere(cameraRay.Origin,
                                             cameraRay.Dir.normalized(),
                                             m_SceneObjects[i]->getPos(),
                                             point, m_SceneObjects[i]->getRadius(),
                                             t);

        if(t > 0.0)
        {
            if(min_t == 0.0)
            {
                m_pickedObject = m_SceneObjects[i];
                m_pickedObject->isActive(true);
                min_t = t;
                index = i;
            }
            if(t < min_t )
            {
                // deactivate previous to avoid confusion with strong overlapping spheres
                m_pickedObject->isActive(false);
                m_pickedObject = m_SceneObjects[i];
                m_pickedObject->isActive(true);
                min_t = t;
                index = i;
            }
        }
    }

    if(index <= 0)
    {
        pSceneOb empty;
        empty.reset();
        widget()->activeObject()->notify(empty);
        return nullptr;
    }
    return m_SceneObjects[index];
}

int3 Scene::readPixel(uint _x, uint _y)
{
    int3 pixel;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, tmpFbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    glReadPixels(_x, _y, 1, 1, GL_RGB, GL_FLOAT, &pixel);
    return  pixel;
}

void Scene::updateLinesVBO()
{
// Get lines from dynamicsWorld
    m_Lines.clear();
    for(QVector3D* _vec : m_DynamicsWorld->m_debugLines)
    {
        QVector3D vec = *_vec;
        m_Lines.push_back(vec);
    }

    m_lines_vao->bind();
    m_lines_vbo.create();
    m_lines_vbo.bind();

    m_lines_vbo.allocate(m_Lines.data(),  m_Lines.size() * sizeof(QVector3D));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                 // index
                          3,                 // size of attr
                          GL_FLOAT,          // datatype of each component
                          GL_FALSE,          // normalized
                          sizeof(QVector3D), //byte offset between consecutive generic vertex attributes
                          nullptr);
    m_lines_vao->release();
}

void Scene::updatePointsVBO()
{
    m_Points.clear();
}

void Scene::initFramebuffer()
{
    uint test;
        glGenFramebuffers(1, &test);
        glBindFramebuffer(GL_FRAMEBUFFER, test);

    bool ms = true;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &texture);
    if(ms){ glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);}
    else{glBindTexture(GL_TEXTURE_2D, texture);}

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    if(ms){ glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);}
    else{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);}

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(ms){ glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);}
    else{glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);}

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qCritical()<<"fbo in initFramebuffer"<< (glCheckFramebufferStatus(GL_FRAMEBUFFER));

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qCritical()<<"intermediateFBO in initFramebuffer"<< (glCheckFramebufferStatus(GL_FRAMEBUFFER));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &tmpFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, tmpFbo);

    glGenTextures(1, &tmpText);
    glBindTexture(GL_TEXTURE_2D, tmpText);
    qDebug()<<"tmpFBO res"<<SCR_WIDTH<<SCR_HEIGHT;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmpText, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qCritical()<<"tmpFbo in initFramebuffer"<< (glCheckFramebufferStatus(GL_FRAMEBUFFER));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::debug(const QVector3D &_pos)
{
}

void Scene::QtOpenGLinitialize()
{
    if(widget())
        widget()->makeCurrent();

    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.10f, 0.0f, 0.33f, 1.0f);

    m_arcCamera.translate(0.0f, 0.0f, 10.0f);
    m_arcCamera.SetWorldPos(QVector3D(0.0f, 0.0f, 10.0f));
    m_arcCamera.SetPivot(QVector3D(0.0f, 0.0f, 0.0f));
    m_arcCamera.SetPivotToCam(QVector3D(0,0,10));
    m_arcCamera.arcBallStart();

//----build, compline and link shaders
    m_activeProgram = new QOpenGLShaderProgram;
    m_activeProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phongWireframe.vert");
    m_activeProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phongWireframe.frag");
    m_activeProgram->link();

    m_screen_program = new QOpenGLShaderProgram;
    m_screen_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/screen.vert");
    m_screen_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/screen.frag");
    m_screen_program->link();

    m_lighting_program = new QOpenGLShaderProgram();
    m_lighting_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phongCalcNormals.vert");
    m_lighting_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phongCalcNormals.frag");
    m_lighting_program->link();

    m_flat_program = new QOpenGLShaderProgram();
    m_flat_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/flat.vert");
    m_flat_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/flat.frag");
    m_flat_program->link();

    m_manipulator_program = new QOpenGLShaderProgram();
    m_manipulator_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/manipulator.vert");
    m_manipulator_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/manipulator.frag");
    m_manipulator_program->link();

    m_picking_program = new QOpenGLShaderProgram();
    m_picking_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/picking.vert");
    m_picking_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/picking.frag");
    m_picking_program->link();

    QOpenGLShader vertshader(QOpenGLShader::Vertex);
    QOpenGLShader geoShader(QOpenGLShader::Geometry);
    QOpenGLShader fragShader(QOpenGLShader::Fragment);

    bool success1 = vertshader.compileSourceFile(":/shader/WireframeG.vert");
    bool success2 = geoShader.compileSourceFile(":/shader/WireframeG.geom");
    bool success3 = fragShader.compileSourceFile(":/shader/WireframeG.frag");

    m_wireframe_program = new QOpenGLShaderProgram();
    m_wireframe_program->addShader(&vertshader);
    m_wireframe_program->addShader(&geoShader);
    m_wireframe_program->addShader(&fragShader);
    bool success4 = m_wireframe_program->link();

    if(!success1 || !success2 || !success3 || !success4)
    {
        qDebug()<<"---------------SHADER DID NOT COMPILE"<<"VS: "<<success1<<"GS: "<<success2<<"FS: "<<success3<<"Link: "<<success4;
        qDebug()<<m_wireframe_program->log();
    }

    QOpenGLShader vertshaderLines(QOpenGLShader::Vertex);
    QOpenGLShader geoShaderLines(QOpenGLShader::Geometry);
    QOpenGLShader fragShaderLines(QOpenGLShader::Fragment);

    success1 = vertshaderLines.compileSourceFile(":/shader/WireframeLinesG.vert");
    success2 = geoShaderLines.compileSourceFile(":/shader/WireframeLinesG.geom");
    success3 = fragShaderLines.compileSourceFile(":/shader/WireframeLinesG.frag");

    m_wireframe_lines_program = new QOpenGLShaderProgram();
    m_wireframe_lines_program->addShader(&vertshaderLines);
    m_wireframe_lines_program->addShader(&geoShaderLines);
    m_wireframe_lines_program->addShader(&fragShaderLines);
    success4 = m_wireframe_program->link();

    if(!success1 || !success2 || !success3 || !success4)
    {
        qDebug()<<"---------------SHADER m_wireframe_lines_program DID NOT COMPILE"<<"VS: "<<success1<<"GS: "<<success2<<"FS: "<<success3<<"Link: "<<success4;
        qDebug()<<m_wireframe_program->log();
    }

    //----prepare a QuadPlane
    m_quad_vao = new QOpenGLVertexArrayObject();
    m_quad_vao->create();
    m_quad_vbo.create();
    m_quad_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_quad_vao->bind();
    m_quad_vbo.bind();
    m_quad_vbo.allocate(quad, 30 * sizeof(GLfloat));
    // tell simple shader how to interpret the quadPlane
    m_screen_program->setAttributeBuffer("position", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_screen_program->enableAttributeArray("position");
    m_screen_program->setAttributeBuffer("uv", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    m_screen_program->enableAttributeArray("uv");
    m_quad_vbo.release();
    m_quad_vao->release();

    initFramebuffer();

    somePoints.push_back(QVector3D(0,2,0));
    somePoints.push_back(QVector3D(0,4,0));
    somePoints.push_back(QVector3D(1,0,0));
    somePoints.push_back(QVector3D(0,0,1));

    pointsVAO = new QOpenGLVertexArrayObject();
    pointsVAO->create();
    pointsVBO.create();
    pointsVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    pointsVAO->bind();
    pointsVBO.bind();
    pointsVBO.allocate(somePoints.data(), somePoints.size() * sizeof (QVector3D));

    m_wireframe_program->enableAttributeArray("Position");
    m_wireframe_program->setAttributeBuffer("Position",
                                           GL_FLOAT,                    // type
                                           0,                           // offset = start
                                           3,                           // tuplesize num of components per vert
//                                           3 * sizeof(GLfloat));        //stride num of bytes between verts. 0 = densly packed
                                           sizeof(Vertex));
    m_wireframe_program->enableAttributeArray("Normal");
    m_wireframe_program->setAttributeBuffer("Normal",
                                           GL_FLOAT,                    // type
                                           offsetof(Vertex, Normal),                           // offset = start
                                           3,                           // tuplesize num of components per vert
                                           sizeof(Vertex));        //stride num of bytes between verts. 0 = densly packed

    pointsVBO.release();
    pointsVAO->release();

    m_lines_vao = new QOpenGLVertexArrayObject();
    m_lines_vao->create();
    m_lines_vbo.create();
    m_lines_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    pointsVAO->bind();
    updateLinesVBO();
}

void Scene::DynamicsInitialize()
{
    if(!m_DynamicsWorld)
        m_DynamicsWorld = new DynamicsWorld();

    m_DynamicsWorld->initialize(this);
}

void Scene::resize(int width, int height)
{
    if(widget())
    {
        SCR_WIDTH = widget()->width() * 2;
        SCR_HEIGHT = widget()->height() * 2;
    }
    else
    {
        SCR_WIDTH = 720;
        SCR_HEIGHT = 720;
    }

    if(mainpulator->m_framebuffer)
    {
        mainpulator->m_framebuffer->resize(SCR_WIDTH, SCR_HEIGHT);
//        mainpulator->m_framebuffer->init();
    }

    m_projection_matrix.setToIdentity();
    m_projection_matrix.perspective(40.0f, width / float(height), 0.01f, 1000.0f);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);

    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
}

void Scene::paint()
{
    QVector4D null = QVector4D(0,0,0,1);
    QMatrix4x4 model;
    model.setToIdentity();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glViewport ( 0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glDisable(GL_MULTISAMPLE);
        if(mainpulator)
        {
            mainpulator->drawPickingBuffer();
        }

        glDisable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glEnable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                  m_lighting_program->bind();
                  m_lighting_program->setUniformValue("ProjectionMatrix", m_projection_matrix);
                  m_lighting_program->setUniformValue("ViewMatrix", m_arcCamera.toMatrix());
                  m_lighting_program->setUniformValue("viewPos", m_arcCamera.worldPos());
                  m_lighting_program->setUniformValue("numPointLights", int(m_Pointlights.size()));
                  m_lighting_program->setUniformValue("numMaterials", int(m_Materials.size()));

                  // set Position + color for all pointsLights
                  for(uint i = 0; i < m_Pointlights.size(); i++)
                  {
                        std::string uniFName = "PointLights[" + std::to_string(i) +"]";
                        m_lighting_program->setUniformValue((uniFName+".position").c_str(), m_Pointlights[i]->position);
                        m_lighting_program->setUniformValue((uniFName+".color").c_str(), m_Pointlights[i]->color);
                  }
                  m_lighting_program->setUniformValue("objectColor", 1.0f, 0.5f, 0.31f);

                // draw all SceneObjects
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                for(uint i = 1; i < m_SceneObjects.size(); i++)
                {
                    if(m_pickedObject == m_SceneObjects[i])
                    {
                        continue;
                    }
                    if(m_SceneObjects[i]->isHidden())
                        continue;
                    uint matID = m_SceneObjects[i]->getMaterialID();
                    m_lighting_program->setUniformValue("mMaterial.ambient", m_Materials[matID]->ambient );
                    m_lighting_program->setUniformValue("mMaterial.diffuse", m_Materials[matID]->diffuse );
                    m_lighting_program->setUniformValue("mMaterial.specular", m_Materials[matID]->specular );
                    m_lighting_program->setUniformValue("mMaterial.shininess", m_Materials[matID]->shininess );
                    m_lighting_program->setUniformValue("ModelMatrix",  m_SceneObjects[i]->getMatrix());
                    m_SceneObjects[i]->draw();
                }

            //-------------------------Draw Active---------------------------------------------------------------------------
            if(m_pickedObject)
            {
                m_activeProgram->bind();
                for(uint i = 0; i < m_Pointlights.size(); i++)
                {
                      std::string uniFName = "PointLights[" + std::to_string(i) +"]";
                      m_activeProgram->setUniformValue((uniFName+".position").c_str(), m_Pointlights[i]->position);
                      m_activeProgram->setUniformValue((uniFName+".color").c_str(), m_Pointlights[i]->color);
                }
                uint matID = m_pickedObject->getMaterialID();
                m_activeProgram->bind();
                m_activeProgram->setUniformValue("ProjectionMatrix", m_projection_matrix);
                m_activeProgram->setUniformValue("ViewMatrix", m_arcCamera.toMatrix());
                m_activeProgram->setUniformValue("viewPos", m_arcCamera.worldPos());
                m_activeProgram->setUniformValue("numPointLights", int(m_Pointlights.size()));
                m_activeProgram->setUniformValue("numMaterials", int(m_Materials.size()));

                m_activeProgram->setUniformValue("wireFrameColor", QVector3D(0,1,0) );
                m_activeProgram->setUniformValue("overlayColor", QVector4D(0.1,0.5,0.2,1) );
                m_activeProgram->setUniformValue("mMaterial.ambient", m_Materials[matID]->ambient );
                m_activeProgram->setUniformValue("mMaterial.diffuse", m_Materials[matID]->diffuse );
                m_activeProgram->setUniformValue("mMaterial.specular", m_Materials[matID]->specular );
                m_activeProgram->setUniformValue("mMaterial.shininess", m_Materials[matID]->shininess );
                m_activeProgram->setUniformValue("ModelMatrix",  m_pickedObject->getMatrix());
                m_pickedObject->draw();
                m_activeProgram->setUniformValue("wireFrameColor", QVector3D(0,0,0) );
                m_activeProgram->setUniformValue("overlayColor", QVector4D(0,0,0,0) );
            }

            //-------------------------Draw Wireframe-----------------------------------------------------------
              m_flat_program->bind();
              m_flat_program->setUniformValue("ProjectionMatrix", m_projection_matrix);
              m_flat_program->setUniformValue("ViewMatrix", m_arcCamera.toMatrix());
              m_flat_program->setUniformValue("Color", QVector3D(0.3,0.3,0.3));
              if(m_SceneObjects.size() > 0)
              {
                  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                  m_flat_program->setUniformValue("ModelMatrix",  m_SceneObjects[0]->getMatrix());
                  m_SceneObjects[0]->draw();
              }

//          drawLines();
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          updateLinesVBO();
          drawLinesAsPolygons();

         glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
         glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
         glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.055f, 1.0f, 1.0f, 1.0f);
        glViewport ( 0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

        m_screen_program->bind();
        m_quad_vao->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_quad_vao->release();
        m_screen_program->release();

        if(mainpulator)
        {
                mainpulator->draw();
        }
}

void Scene::updateSceneObjects()
{
    for(uint i = 0; i < m_SceneObjects.size(); i++)
    {
        m_SceneObjects[i]->update();

        // animate collider spheres
//        if(i==1)
//            m_pinnCstr_1->setPositon(QVector3D(sin(widget()->elapsedTime() * 0.002) * 10 , 0,0));
//        if(i==2)
//            m_pinnCstr_2->setPositon(QVector3D(sin(widget()->elapsedTime() * 0.003) * 12 , 0, 5));
//        if(i==3)
//            m_pinnCstr_3->setPositon(QVector3D(0 , 17 , sin((widget()->elapsedTime() + 1500) * 0.001) * 20));
//        if(i==4)
//            m_pinnCstr_4->setPositon(QVector3D(2 , 17 , sin(widget()->elapsedTime() * 0.001) * 16 ));
    }
}

void Scene::drawScreenQuad()
{
    glViewport ( 0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw a quad to the screen
    m_screen_program->bind();
    m_view_position_texture->bind(0);
    m_quad_vao->bind();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_quad_vao->release();
    m_screen_program->release();
}

void Scene::setDynamicsWorld(DynamicsWorld *_world)
{
    m_DynamicsWorld = _world;
}

void Scene::setupScene()
{
    mlog<<"setup Scene";
         QVector3D pointLightA(0,25,0);
         QVector3D pointLightB(10,25,0);
         QVector3D pointLightC(0,15,10);

         pointLightA *= 10;
         pointLightB *= 10;
         pointLightC *= 10;

         addPointLight(pointLightA, QVector3D(0.8, 1.0, 0.8));
         addPointLight(pointLightB, QVector3D(0.6, 0.6, 1.0));
         addPointLight(pointLightC, QVector3D(0.5, 0.5, 0.5));

         // blue tone
        addMaterial(    QVector3D(0.01f , 0.04f ,0.1f),   // ambient
                        QVector3D(0.1f , 0.3f ,0.8f),    // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        32.0 );                          // shininess

        addMaterial(    QVector3D(0.01f , 0.02f ,0.1f),   // ambient
                        QVector3D(0.1f , 0.0f ,0.3f),    // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        32.0 );

        addMaterial(    QVector3D(0.01f , 0.01f ,0.07f),   // ambient
                        QVector3D(0.1f , 0.01f ,0.2f),    // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        32.0 );

        addMaterial(    QVector3D(0.01f , 0.01f ,0.07f),   // ambient
                        QVector3D(0.2f , 0.21f ,0.8f),    // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        32.0 );

        // organge tone
        addMaterial(    QVector3D(0.1f , 0.01f ,0.01f),   // ambient
                        QVector3D(0.5f , 0.2f ,0.07f),   // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        20.0 );                          // shininess

        addMaterial(    QVector3D(0.1f , 0.0f ,0.05f),   // ambient
                        QVector3D(0.5f , 0.0f ,0.3f),   // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        20.0 );

        // green tone
        addMaterial(    QVector3D(0.0f , 0.05f ,0.0f),   // ambient
                        QVector3D(0.2f , 0.5f ,0.05f),   // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        15.0 );

//        addMaterial(    QVector3D(0.0f , 0.05f ,0.0f),   // ambient
//                        QVector3D(0.1f , 0.3f ,0.1f),   // diffuse
//                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
//                        15.0 );

        // yellow
        addMaterial(    QVector3D(0.1f , 0.1f ,0.0f),   // ambient
                        QVector3D(0.6f , 0.4f ,0.1f),   // diffuse
                        QVector3D(1.0f , 1.0f ,1.0f),    // specular
                        15.0 );

        addMaterial(    QVector3D(0.2f , 0.2f ,0.1f),   // ambient
                        QVector3D(0.3f , 0.3f ,0.1f),   // diffuse
                        QVector3D(0.0f , 0.6f ,0.3f),    // specular
                        60.0 );

//         for(int i =0; i<10; i++)
//         {
//             QVector3D ambient = QVector3D(randfinRange(0.1*i,1), randfinRange(0.1*i,1) * 0.1, randfinRange(0.1*i,1)*0.1);
//             mlog<<"ambient"<< ambient;
//             QVector3D diffuse = QVector3D(ambient.x() - 0.3, ambient.y()  -0.3, ambient.z() -0.3);
//             addMaterial(    diffuse,   // ambient
//                            ambient,    // diffuse
//             QVector3D(1.0f , 1.0f ,1.0f),    // specular
//             32.0 );
//         }

    //    MAKE MODEL TO RENDER
        addModel(this, "grid", "/Users/enno/Dev/Grid100.obj");
       addModel(this, "grid1", "/Users/enno/Dev/Cube10.obj");

       addModel(this, "teapot", "/Users/enno/Dev/teapod_high.obj");

//       addModel(this, "cloth", "/Users/enno/Dev/Grid_3x3.obj");
//       addModel(this, "cloth", "/Users/enno/Dev/Grid_1681points.obj");

//       addModel(this, "cloth", "/Users/enno/Dev/Grid_441points.obj");
       //Grid_441points_tri.obj Quad_497_remeshed.obj
//       addModel(this, "cloth", "/Users/enno/Dev/Grid_225_tri.obj");
       addModel(this, "cloth", "/Users/enno/Dev/Quad_497_remeshed.obj");
//       addModel(this, "cloth", "/Users/enno/Dev/Quad_1907_remeshed.obj");
       addModel(this, "cloth2", "/Users/enno/Dev/Quad_1935_big_remeshed.obj");

//       addModel(this, "cube", "/Users/enno/Dev/Cube_98.obj");
//       addModel(this, "cube", "/Users/enno/Dev/Cube_26.obj");
       addModel(this, "cube", "/Users/enno/Dev/Cube_8.obj");
       addModel(this, "quad", "/Users/enno/Dev/Quad_1_tri.obj");

       addModel(this, "sphere", "/Users/enno/Dev/Icosahedronf4.obj");

       addModel(this, "Vector", "/Users/enno/Dev/VectorShape.obj");
       addModel(this, "triangle", "/Users/enno/Dev/triangle.obj");
       addModel(this, "torus", "/Users/enno/Dev/torus.obj");
       addModel(this, "Circle", "/Users/enno/Dev/TorusShape.obj");
       addModel(this, "Plane", "/Users/enno/Dev/PlaneShape.obj");
       addModel(this, "Axis", "/Users/enno/Dev/AxisShape.obj");
       addModel(this, "bunny", "/Users/enno/Dev/bunny_low.obj");
       addModel(this, "bunny_high", "/Users/enno/Dev/Bunny_394.obj");
       addModel(this, "rubberyoy", "/Users/enno/Dev/rubbertoy_2.obj");

       addModel(this, "cube_high", "/Users/enno/Dev/Cube_125.obj");
       addModel(this, "cube_Test", "/Users/enno/Dev/Cube_42.obj");

       addModel(this, "brick1_high", "/Users/enno/Dev/BrickX1_216.obj");
       addModel(this, "brick2_high", "/Users/enno/Dev/BrickX2_216.obj");

       // ONlY RENDER WITH addSceneObjectFromModel(), otherwise crash (WIP)
       pSceneOb grid = addSceneObjectFromModel("grid", 1, QVector3D(0, 0 ,0 ), QQuaternion(1,0,0,0));
//       grid->setScale(QVector3D(5,5,5));

//       auto sceneObject1 = addSceneObjectFromModel("grid1", 0, QVector3D(0,3,0), QQuaternion(0.8,0.3,0.3,0.1));
       QQuaternion rot = QQuaternion::fromEulerAngles(QVector3D(0,0,0));
       QQuaternion rot2 = QQuaternion::fromEulerAngles(QVector3D(0, 0, 45));

       float s = 8;
       auto sceneObjectBallCollider = addSceneObjectFromModel("sphere", m_Materials.size()-1 , QVector3D(-6,0,0), rot);
       sceneObjectBallCollider->setScale(QVector3D(s,s,s));
       DynamicObjectPtr dynObj = m_DynamicsWorld->addDynamicObjectAsNonUniformParticle(sceneObjectBallCollider, s/2);
//       Particle *ptr = nullptr;
//       auto particleSmartPointer = dynObj->pointer(ptr);
//       m_pinnCstr_1 = std::make_shared<PinConstraint>(particleSmartPointer, QVector3D(0,0,0));
//       particleSmartPointer->m_Constraints.push_back(m_pinnCstr_1);

//       auto sceneObjectBallColliderB = addSceneObjectFromModel("sphere", m_Materials.size()-2 , QVector3D(0,0,5), rot);
//       sceneObjectBallColliderB->setScale(QVector3D(s,s,s));
//       DynamicObjectPtr dynObj2 = m_DynamicsWorld->addDynamicObjectAsNonUniformParticle(sceneObjectBallColliderB, s/2);
//       Particle *ptr2 = nullptr;
//       auto particleSmartPointer2 = dynObj2->pointer(ptr2);
//       m_pinnCstr_2 = std::make_shared<PinConstraint>(particleSmartPointer2, QVector3D(0,0,0));
//       particleSmartPointer2->m_Constraints.push_back(m_pinnCstr_2);

//       float s2 = 2.0;
//       auto sceneObjectBallColliderC = addSceneObjectFromModel("sphere", m_Materials.size()-3 , QVector3D(0,17,0), rot);
//       sceneObjectBallColliderC->setScale(QVector3D(s2,s2,s2));
//       DynamicObjectPtr dynObj3 = m_DynamicsWorld->addDynamicObjectAsNonUniformParticle(sceneObjectBallColliderC, s2/2);
//       Particle *ptr3 = nullptr;
//       auto particleSmartPointer3 = dynObj3->pointer(ptr3);
//       m_pinnCstr_3 = std::make_shared<PinConstraint>(particleSmartPointer3, QVector3D(0,17,0));
//       particleSmartPointer3->m_Constraints.push_back(m_pinnCstr_3);

//       auto sceneObjectBallColliderD = addSceneObjectFromModel("sphere", m_Materials.size()-4 , QVector3D(2,17,0), rot);
//       sceneObjectBallColliderD->setScale(QVector3D(s2,s2,s2));
//       DynamicObjectPtr dynObj4 = m_DynamicsWorld->addDynamicObjectAsNonUniformParticle(sceneObjectBallColliderD, s2/2);
//       Particle *ptr4 = nullptr;
//       auto particleSmartPointer4 = dynObj4->pointer(ptr4);
//       m_pinnCstr_4 = std::make_shared<PinConstraint>(particleSmartPointer4, QVector3D(2,17,0));
//       particleSmartPointer4->m_Constraints.push_back(m_pinnCstr_4);

//       auto sceneObjectRT = addSceneObjectFromModel("bunny_high", 2, QVector3D(0,9.5,0), rot2);
//       m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectRT , "/Users/enno/Dev/Bunny_394_volumeGrad.obj", 2);

//          auto sceneObjectRT = addSceneObjectFromModel("sphere", 2, QVector3D(0,9.5,0), rot2);
//          m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectRT , "/Users/enno/Dev/BrickY2_test.obj", 2);

//         auto sceneObjectRT = addSceneObjectFromModel("rubberyoy", 1, QVector3D(0,18,0), rot);
//         m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectRT , "/Users/enno/Dev/rubbertoy_2_volumesample.obj", 2);


//         auto teapot = addSceneObjectFromModel("teapot", 2, QVector3D(0,6,0), rot);
//         m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(teapot , "/Users/enno/Dev/teapod_high_volumesample.obj", 2);

//// cloth mass
//        for(int i=0; i < 1; i++)
//        {
//            QQuaternion rotx = QQuaternion::fromEulerAngles(QVector3D(0, 0, 0));
//            auto sceneObject_cloth = addSceneObjectFromModel("cloth2", 0, QVector3D(i*5, 12 , i*5), rotx);
//            sceneObject_cloth->setScale(QVector3D(0.6,0.6,0.6));
//            m_DynamicsWorld->addDynamicObjectAsSoftBody(sceneObject_cloth);
//        }

//// Ropes
//       m_DynamicsWorld->addRope(QVector3D(3,0,0), QVector3D(-12,0,0), 26);
//       m_DynamicsWorld->addRope(QVector3D(3,5,0), QVector3D(-12,5,0), 6);

////// Rigid Body Grid Bunnies
//     for(int i=0; i < 2; i++)
//     {
//         float x,y,z;
//         x= randfinRange(-5,5);
//         y= randfinRange(20,50);
//         z= randfinRange(-5,5);
//         QQuaternion rotX = QQuaternion::fromEulerAngles(QVector3D(rand() % 90,rand() % 90,rand() % 90));
//     auto sceneObjectX = addSceneObjectFromModel("bunny_high", (i%m_Materials.size()), QVector3D(x, y, z), rotX);
//      m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectX , "/Users/enno/Dev/Bunny_394_volumeGrad.obj", (i%3));
//     }


/// Rigid Body Grid Cubes
//     for(int i=0; i < 2; i++)
//     {
//         float x,y,z;
//         x= randfinRange(-5,5);
//         y= randfinRange(2,30);
//         z= randfinRange(-5,5);
////         QQuaternion rotX = QQuaternion::fromEulerAngles(0,0,0);

//         QQuaternion rotX = QQuaternion::fromEulerAngles(QVector3D(rand() % 90,rand() % 90,rand() % 90));
////     auto sceneObjectX = addSceneObjectFromModel("cube_Test", (i%3), QVector3D(x, y, z), rotX);
////     m_DynamicsWorld->addDynamicObjectAsRigidBody(sceneObjectX);

//        auto sceneObjectX = addSceneObjectFromModel("brick2_high", (i%3), QVector3D(x, y, z), rotX);
//        m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectX , "/Users/enno/Dev/BrickX2_420_volumesample.obj", (i%3));
//     }

/// Rigid Body Bunnies
//       for(int i=0; i < 2; i++)
//       {
//           float x,y,z;
//           x= randfinRange(-5,5);
//           y= randfinRange(10,30);
//           z= randfinRange(-5,5);
//           QQuaternion rotX = QQuaternion::fromEulerAngles(QVector3D(rand() % 90,rand() % 90,rand() % 90));
//       auto sceneObject1 = addSceneObjectFromModel("bunny", (i%3), QVector3D(x, y, z), rotX);
//        m_DynamicsWorld->addDynamicObjectAsRigidBody(sceneObject1);
//       }

//// RigidBody Rain
//        for(int i=0; i < 10; i++)
//        {
//            QQuaternion rotX = QQuaternion::fromEulerAngles(QVector3D(rand() % 90,rand() % 90,rand() % 90));
//            auto sceneObjectX = addSceneObjectFromModel("cube", (i%4), QVector3D(rand() % 4, rand() % 60 + 10, rand() % 4), rotX);
//            m_DynamicsWorld->addDynamicObjectAsRigidBody(sceneObjectX, (i%3));
//        }

 /// Rigid Body Stack or Wall Grid
     int row = 6;
     int column = 8;
      for(int i=0; i < row; i++)
     {
         QQuaternion rotX = QQuaternion::fromEulerAngles(QVector3D(0,0,0));
         for(int j=0; j < column; j++)
         {
             float aabbY = 1.6;
             float aabbX = 3.8;
             float centerOffset = 1;
             float x = (centerOffset + aabbX *  j);
             float y = (centerOffset + aabbY *  i) + 0.15 ;
             if(i % 2 > 0){
                 if(j==0){
                     auto sceneObjectHalf = addSceneObjectFromModel("brick1_high", ((j+i+int(randfinRange(0,i)))%(m_Materials.size()-1)), QVector3D(x - 1.0 , y ,0), rotX);
                     m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectHalf , "/Users/enno/Dev/BrickX1_216_volumesample.obj", (i%3));
                 }
                 x += 2.0;
             }
             auto sceneObjectX = addSceneObjectFromModel("brick2_high", ((j+i+int(randfinRange(0,i)))%(m_Materials.size()-1)), QVector3D(x , y ,0), rotX);
             m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectX , "/Users/enno/Dev/BrickX2_216_volumesample.obj", (i%3));
             if(j == column-1 && i %  2 == 0){
                 auto sceneObjectHalf = addSceneObjectFromModel("brick1_high", ((j+i+int(randfinRange(0,i)))%(m_Materials.size()-1)), QVector3D(x + 3.0 , y ,0), rotX);
                 m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectHalf , "/Users/enno/Dev/BrickX1_216_volumesample.obj", (i%3));
             }
         }
     }

/// Rigid Body Stack or Wall
//       for(int i=0; i < 4; i++)
//        {
//            QQuaternion rotX = QQuaternion::fromEulerAngles(QVector3D(0,0,0));
//            for(int j=0; j < 8; j++)
//            {
//                float aabbY = 2 - 0.15;
//                float aabbX = 2;
//                float centerOffset = 1;
//                float x = (centerOffset + aabbX *  j);
//                if(i % 2 > 0)
//                    x += 0.5;
//                float y = (centerOffset + aabbY *  i) ;
//                float z;
//                auto sceneObjectX = addSceneObjectFromModel("cube", (i%4), QVector3D(x , y ,0), rotX);
//                m_DynamicsWorld->addDynamicObjectAsRigidBody(sceneObjectX, (i%3));
//            }
//        }


/// Rigid BodyGrid  Stack
//        for(int i=0; i < 3; i++)
//        {
//            QQuaternion rotX = QQuaternion::fromEulerAngles(QVector3D(0,0,0));
//            auto sceneObjectX = addSceneObjectFromModel("cube_high", (i%4), QVector3D(0.01*randfinRange(-1,1) ,(0 + 3* i),0), rotX);
////            auto sceneObjectX = addSceneObjectFromModel("cube", (i%4), QVector3D(0 ,(1.5 + 3* i),0), rotX);
//             m_DynamicsWorld->addDynamicObjectAsRigidBodyGrid(sceneObjectX , "/Users/enno/Dev/Cube_125_volumeGrad.obj", (i%3));
//        }

//// Two Particle pairs for friction
//       auto sphere2 = addSceneObjectFromModel("sphere", 1, QVector3D(0.4, 1.5, 0), rot);
//       m_DynamicsWorld->addDynamicObjectAsParticle(sphere2);

//        auto sphere1 = addSceneObjectFromModel("sphere", 2, QVector3D(0, 0.5, 0), rot);
//        m_DynamicsWorld->addDynamicObjectAsParticle(sphere1);

//        auto sphere4 = addSceneObjectFromModel("sphere", 0, QVector3D(2, 0.5, 0), rot);
//        m_DynamicsWorld->addDynamicObjectAsParticle(sphere4);

//        auto sphere3 = addSceneObjectFromModel("sphere", 1, QVector3D(2.4, 1.5, 0), rot);
//        m_DynamicsWorld->addDynamicObjectAsParticle(sphere3);


//// particle pyramid
//       auto sphere1 = addSceneObjectFromModel("sphere", 0, QVector3D(-1, 0.5, 0), rot);
//       auto sphere2 = addSceneObjectFromModel("sphere", 0, QVector3D(0, 0.5, 0), rot);
//       auto sphere3 = addSceneObjectFromModel("sphere", 0, QVector3D(1, 0.5, 0), rot);
//       auto sphere4 = addSceneObjectFromModel("sphere", 1, QVector3D(-0.5, 1.35, 0), rot);
//       auto sphere5 = addSceneObjectFromModel("sphere", 1, QVector3D(0.5, 1.35, 0), rot);
//       auto sphere6 = addSceneObjectFromModel("sphere", 2, QVector3D(0, 2.20, 0), rot);

//       m_DynamicsWorld->addDynamicObjectAsParticle(sphere1);
//       m_DynamicsWorld->addDynamicObjectAsParticle(sphere2);
//       m_DynamicsWorld->addDynamicObjectAsParticle(sphere3);
//       m_DynamicsWorld->addDynamicObjectAsParticle(sphere4);
//       m_DynamicsWorld->addDynamicObjectAsParticle(sphere5);
//       m_DynamicsWorld->addDynamicObjectAsParticle(sphere6);

//// Particle Rain
//        for(int i=0; i < 1000; i++)
//        {
//            auto sphere1 = addSceneObjectFromModel("sphere", (i%3), QVector3D(randfinRange(0,5), randfinRange(2,255), randfinRange(0,5)), rot);
//            m_DynamicsWorld->addDynamicObjectAsParticle(sphere1);
//        }

//// particle Cube Pile
//        int dim = 5;
//       for(int k=0; k< dim ; k++){
//           for(int i=0; i < dim; i++){
//               for(int j=0; j < dim; j++){
//                   float rand = randfinRange(-1,1) * 0.01;
//                   auto sphere1 = addSceneObjectFromModel("sphere", (i%3), QVector3D(i+rand, k+0.5, j+rand), rot);
//                   m_DynamicsWorld->addDynamicObjectAsParticle(sphere1);
//               }//           }
//       }

//// cloth
//        auto sceneObjectC1 = addSceneObjectFromModel("quad", 1, QVector3D(0, 20.0, 0), rot);
//        m_DynamicsWorld->addDynamicObjectAsRigidBody(sceneObjectC1);

//        auto sceneObjectC2 = addSceneObjectFromModel("cloth", 1, QVector3D(0, 15, 0), rot2);
//        m_DynamicsWorld->addDynamicObjectAsSoftBody(sceneObjectC2, 0.5);

//        auto sceneObjectC3 = addSceneObjectFromModel("cloth2", 0, QVector3D(5, 20.5, 0), rot2);
//        m_DynamicsWorld->addDynamicObjectAsSoftBody(sceneObjectC3, 2.5);


       ModelPtr _vectorShape = getModelFromPool("Vector");
       mainpulator = new Manipulator(this, _vectorShape, m_manipulator_program);

       //        addSceneObjectFromModel("sphere", 3, pointLightA, rot);
       //        addSceneObjectFromModel("sphere", 3, pointLightB, rot);
       //        addSceneObjectFromModel("sphere", 3, pointLightC, rot);

       mlog<<"NUm particles:"<<m_DynamicsWorld->pCount;



}


