#include "sceneobject.h"

#include <QDebug>

#include "Scene.h"
#include "activeobject.h"

SceneObject::SceneObject()
{
}

SceneObject::SceneObject(Scene *_scene, ShapePtr _Shape) :
    pScene(_scene),
    pShape(_Shape)
{

}

SceneObject::SceneObject(Scene *_scene, ShapePtr _Shape,
                         const QVector3D &_pos) :
    pScene(_scene),
    pShape(_Shape)
{
    m_Transform.setTranslation(_pos);
    m_ModelMatrix = m_Transform.toMatrix();
}

SceneObject::SceneObject(Scene *_scene, ShapePtr _Shape,
                         const QVector3D &_pos,
                         const QQuaternion &_rot) :
    pScene(_scene),
    pShape(_Shape)
{

    m_Transform.setTranslation(_pos);
    m_Transform.rotate(_rot);
    m_ModelMatrix = m_Transform.toMatrix();
}

SceneObject::SceneObject(Scene *_scene, ModelPtr _Model, const uint _materialID, const QVector3D &_pos) :
    pScene(_scene),
    pModel(_Model),
    m_MaterialID(_materialID)
{
    m_Transform.setTranslation(_pos);
    m_ModelMatrix = m_Transform.toMatrix();
}

SceneObject::SceneObject(Scene *_scene,
                         ModelPtr _Model,
                         const uint _materialID,
                         const QVector3D &_pos,
                         const QQuaternion &_rot) :
    pScene(_scene),
    pModel(_Model),
    m_MaterialID(_materialID)
{
    m_Transform.setTranslation(_pos);
    m_Transform.rotate(_rot);
    m_ModelMatrix = m_Transform.toMatrix();
}

SceneObject::SceneObject(std::string _path)
{

}

void SceneObject::bind()
{
    pShape->bind();
}

void SceneObject::draw()
{
    if(pModel != nullptr)
    {
        pModel->draw();
    }
    else
    {
        pShape->drawOld();
    }
}

void SceneObject::drawPoints()
{
    if(pModel != nullptr)
    {
        pModel->drawPoints();
    }
}

void SceneObject::release()
{
    pShape->release();
}

void SceneObject::setTranslation(const QVector3D &_dt)
{
    m_Transform.setTranslation(_dt);
    m_IsDirty=true;
}

void SceneObject::update()
{
    if(isDynamic())
    {
        m_ModelMatrix = pDynamicObject->getTransfrom();
        setTranslation(pDynamicObject->getTranslation());

        if(m_IsActive)
        {
            notify();
        }
    }
    else if(m_IsDirty)
    {
        m_IsDirty = false;
        m_ModelMatrix =  m_Transform.toMatrix();

        if(m_IsActive)
        {
            notify();
        }
    }
}

void SceneObject::isActive(bool _isActive)
{
    m_IsActive = _isActive;
    m_IsDirty = true;
}

bool SceneObject::isActive()
{
    return m_IsActive;
}

void SceneObject::setRadius(float _r)
{
    setScale(QVector3D(2 * _r, 2 * _r, 2 * _r));
    m_radius = _r;
}

void SceneObject::makeDynamic(DynamicObjectPtr _dynamicObject)
{
    pDynamicObject = _dynamicObject;
    m_IsDynamic = true;
}

void SceneObject::isDynamic(bool _isDynamic)
{
    m_IsDynamic = _isDynamic;
}

bool SceneObject::isPinned()
{
    return m_IsPinned;
}

void SceneObject::isPinned(bool _pinned)
{
    m_IsPinned = _pinned;
}

bool SceneObject::isHidden()
{
    return m_IsHidden;
}

void SceneObject::isHidden(bool _isHidden)
{
    m_IsHidden = _isHidden;
}

void SceneObject::setRotation(const QQuaternion &_rt)
{
    m_Transform.setRotation(_rt);
}

void SceneObject::setActiveObject(ActiveObject *_activeObject)
{
    activeObject = _activeObject;
}

void SceneObject::setPinConstraint(std::shared_ptr<PinConstraint> _pinConstraint)
{
    m_pinConstraint = _pinConstraint;
}

void SceneObject::setModel(ModelPtr _model)
{
    pModel =  _model;
}

void SceneObject::setID(uint _id)
{
    m_ID = _id;
}

void SceneObject::notify()
{
    pSceneOb pSO = pScene->getPointerFromSceneObject(this);
    activeObject->notify(pSO);
}

float SceneObject::getRadius()
{
    return m_radius;
}

DynamicObjectPtr SceneObject::dynamicObject()
{
    return pDynamicObject;
}

const QMatrix4x4 SceneObject::getMatrix()
{
    return m_ModelMatrix;
}

std::shared_ptr<PinConstraint> SceneObject::getPinConstraint()
{
    return m_pinConstraint;
}

int SceneObject::getNumConstraints()
{
    if(!pDynamicObject) return 0;

    Particle *ptr = nullptr;
    auto particleSmartPointer = dynamicObject()->pointer(ptr);
    return particleSmartPointer->m_Constraints.size();
}

ModelPtr SceneObject::model()
{
    return pModel;
}

uint SceneObject::getID()
{
    return  m_ID;
}


void SceneObject::setScale(const QVector3D &_s)
{
    m_Transform.scale(_s);
    m_IsDirty=true;
    m_ModelMatrix.scale(_s);
}
