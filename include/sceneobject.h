#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <memory>

#include <QMatrix4x4>

#include "transform.h"
#include "shape.h"
#include "model.h"
#include "utils.h"
#include "dynamics/dynamicObject.h"
#include "dynamics/constraint.h"

//#include "activeobject.h"

class Scene;
class ActiveObject;
class Model;

class SceneObject
{

public:
    SceneObject();
    SceneObject(Scene *_scene, ShapePtr _Shape);
    SceneObject(Scene *_scene, ShapePtr _Shape, const QVector3D &_pos);
    SceneObject(Scene *_scene, ShapePtr _Shape, const QVector3D &_pos, const QQuaternion &_rot);
    SceneObject(Scene *_scene, ModelPtr _Model, const uint _materialID, const QVector3D &_pos);
    SceneObject(Scene *_scene, ModelPtr _Model, const uint _materialID, const QVector3D &_pos, const QQuaternion &_rot);
    SceneObject(std::string _path);

    void bind();
    void draw();
    void drawPoints();
    void drawOld();
    void release();
    void update();

    void isActive(bool _isActive);
    bool isActive();
    void setRadius(float _r);
    void setTranslation(const QVector3D &_dt);
    void setRotation(const QVector3D &_rt);
    void setRotation(const QQuaternion &_rt);
    void translate(const QVector3D &_dt);
    void setScale(const QVector3D &_s);
    void rotate(const QQuaternion &_rot);
    void makeDynamic(DynamicObjectPtr _dynamicObject);
    bool isDynamic();
    void isDynamic(bool _isDynamic);
    bool isPinned();
    void isPinned(bool _pinned);
    bool isHidden();
    void isHidden(bool _isHidden);

    void setActiveObject(ActiveObject *_activeObject);
    void setPinConstraint(std::shared_ptr<PinConstraint> _pinConstraint);
    void setModel(ModelPtr _model);
    void setID(uint _id);
    void notify();

    float getRadius();
    DynamicObjectPtr dynamicObject();
    const QMatrix4x4 getMatrix();
    const QVector3D getPos();
    const Transform getTransform();
    uint getMaterialID();
    std::shared_ptr<PinConstraint> getPinConstraint();
    int getNumConstraints();
    ShapePtr shape();
    ModelPtr model();
    uint getID();

//    bool m_IsDirty = true;

private:
    float m_radius = 0.5;

    bool m_IsActive = false;
    bool m_IsPinned = false;
    bool m_IsHidden = false;
    bool m_IsDirty = true;
    bool m_IsDynamic = false;

    QMatrix4x4 m_ModelMatrix;

    uint m_ID;
    uint m_MaterialID;
    ShapePtr pShape;
    ModelPtr pModel;
    MaterialPtr pMaterial;
    Transform m_Transform;
    Scene *pScene;

    DynamicObjectPtr pDynamicObject = nullptr;
    std::shared_ptr<PinConstraint>  m_pinConstraint = nullptr;
    ActiveObject *activeObject;

};



inline uint SceneObject::getMaterialID(){ return m_MaterialID;};
inline const QVector3D SceneObject::getPos(){ return m_Transform.translation(); };
inline std::shared_ptr<Shape> SceneObject::shape(){return pShape; };

inline bool SceneObject::isDynamic(){ return m_IsDynamic; };


inline void SceneObject::setRotation(const QVector3D &_dt){m_Transform.setRotation(_dt); m_IsDirty=true;};
inline void SceneObject::translate(const QVector3D &_dt){m_Transform.translate(_dt); m_IsDirty=true;};
inline void SceneObject::rotate(const QQuaternion &_rot){m_Transform.rotate(_rot); m_IsDirty=true;};
;
inline const Transform SceneObject::getTransform(){return m_Transform;};

inline void SceneObject::drawOld(){ pShape->drawOld(); };

#endif // SCENEOBJECT_H
