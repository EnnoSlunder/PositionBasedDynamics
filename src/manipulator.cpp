#include <math.h>

#include "include/manipulator.h"

#include "model.h"
#include "Scene.h"
#include "GLWidget.h"
#include "activeobject.h"


Manipulator::Manipulator(Scene* _scene, ModelPtr _vectorModel, QOpenGLShaderProgram* _program)
    : scene(_scene),
      vecotorModel(_vectorModel),
      m_shaderProgram(_program)
{
    localX = QMatrix4x4();
    localX.rotate(-90, QVector3D(0,0,1));

    localZ = QMatrix4x4();
    localZ.rotate(90, QVector3D(1,0,0));

    m_window = scene->widget();
    m_pickingProgram = scene->m_picking_program;
    m_framebuffer = new Framebuffer(_scene);

    axisModel = scene->getModelFromPool("Axis");
    circleModel = scene->getModelFromPool("Circle");
    planeModel = scene->getModelFromPool("Plane");

    worldSpace = false;

    m_activeObject = scene->widget()->activeObject();
    m_activeObject->setManipulator(this);

}

void Manipulator::draw()
{
    if(!isActive())
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer->m_msfbo);

    m_shaderProgram->bind();

    updateGlobalScale();

    QVector3D camPlaneN = (scene->m_arcCamera.worldPos() - m_Transform.translation()).normalized();
    QVector3D camPlaneO = m_Transform.translation();

    m_shaderProgram->setUniformValue("pN", camPlaneN);
    m_shaderProgram->setUniformValue("pO", camPlaneO);
    m_shaderProgram->setUniformValue("ProjectionMatrix", scene->m_projection_matrix);
    m_shaderProgram->setUniformValue("ViewMatrix", scene->m_arcCamera.toMatrix());
    m_shaderProgram->setUniformValue("test",  scene->m_arcCamera.worldPos());

    if(mMode.translate && mMode.rotate)
    {
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix()  * globalScale);
        m_shaderProgram->setUniformValue("color", QVector3D(0,1,0) );
        if(currentState == TRANSLATE_Y)
            m_shaderProgram->setUniformValue("color", QVector3D(1,1,0) );
        vecotorModel->draw();
        m_shaderProgram->setUniformValue("color", QVector3D(0,1,0) );
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix()  * globalScale);
        if(currentState == ROTATE_Y)
            m_shaderProgram->setUniformValue("color", QVector3D(1,1,0) );
        circleModel->draw();

        m_shaderProgram->setUniformValue("color", QVector3D(1,0,0) );
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localX * globalScale);
        if(currentState == TRANSLATE_X)
            m_shaderProgram->setUniformValue("color", QVector3D(1,1,0) );
        vecotorModel->draw();
        m_shaderProgram->setUniformValue("color", QVector3D(1,0,0) );
        if(currentState == ROTATE_X)
            m_shaderProgram->setUniformValue("color", QVector3D(1,1,0) );
        circleModel->draw();

        m_shaderProgram->setUniformValue("color", QVector3D(0,0,1) );
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localZ * globalScale);
        if(currentState == TRANSLATE_Z)
            m_shaderProgram->setUniformValue("color", QVector3D(1,1,0) );
        vecotorModel->draw();
        m_shaderProgram->setUniformValue("color", QVector3D(0,0,1) );
        if(currentState == ROTATE_Z)
            m_shaderProgram->setUniformValue("color", QVector3D(1,1,0) );
        circleModel->draw();
    }

    if(mMode.viewPlane)
    {
        updateLocalView();
        m_shaderProgram->setUniformValue("color", QVector3D(0.9,0.8,0) );
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localView * globalScale);
        if(currentState == TRANSLATE_VIEWPLANE)
            m_shaderProgram->setUniformValue("color", QVector3D(1,1,0) );
        planeModel->draw();
    }

}

void Manipulator::drawPickingBuffer()
{
    if(!isActive())
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer->m_fbo);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pickingProgram->bind();

    QMatrix4x4 scaleXZ;
    scaleXZ.scale(QVector3D(10,1,10));
    QMatrix4x4 scaleY;
    scaleY.scale(QVector3D(1,10,1));

    if(mMode.translate && mMode.rotate)
    {
        m_pickingProgram->setUniformValue("color", QVector3D(0,0,0));
        m_pickingProgram->setUniformValue("ProjectionMatrix", scene->m_projection_matrix);
        m_pickingProgram->setUniformValue("ViewMatrix", scene->m_arcCamera.toMatrix());

        m_pickingProgram->setUniformValue("IDindex",  TRANSLATE_Y);
        m_pickingProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * scaleXZ *  globalScale);
            vecotorModel->draw();
        m_pickingProgram->setUniformValue("IDindex",  ROTATE_Y);
        m_pickingProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * scaleY *  globalScale);
            circleModel->draw();

        m_pickingProgram->setUniformValue("IDindex",  TRANSLATE_X);
        m_pickingProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localX * scaleXZ *globalScale);
            vecotorModel->draw();
        m_pickingProgram->setUniformValue("IDindex",  ROTATE_X);
        m_pickingProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localX * scaleY *globalScale);
            circleModel->draw();

        m_pickingProgram->setUniformValue("IDindex",  TRANSLATE_Z);
        m_pickingProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localZ  * scaleXZ * globalScale);
            vecotorModel->draw();
        m_pickingProgram->setUniformValue("IDindex",  ROTATE_Z);
        m_pickingProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localZ  * scaleY * globalScale);
            circleModel->draw();
    }

    if(mMode.viewPlane)
    {
        m_pickingProgram->setUniformValue("IDindex", TRANSLATE_VIEWPLANE );
        m_pickingProgram->setUniformValue("ModelMatrix",   m_Transform.toMatrix() * localView * globalScale);
        planeModel->draw();
    }

}

void Manipulator::debugDraw()
{
    if(!isActive())
        return;
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer->m_msfbo);
    m_shaderProgram->bind();

    updateGlobalScale();

    QVector3D camPlaneN = (scene->m_arcCamera.worldPos() - m_Transform.translation()).normalized();
    QVector3D camPlaneO = m_Transform.translation();

    m_shaderProgram->setUniformValue("pN", camPlaneN);
    m_shaderProgram->setUniformValue("pO", camPlaneO);
    m_shaderProgram->setUniformValue("ProjectionMatrix", scene->m_projection_matrix);
    m_shaderProgram->setUniformValue("ViewMatrix", scene->m_arcCamera.toMatrix());
    m_shaderProgram->setUniformValue("test",  scene->m_arcCamera.worldPos());

    QMatrix4x4 scaleXZ;
    scaleXZ.scale(QVector3D(10,1,10));
    QMatrix4x4 scaleY;
    scaleY.scale(QVector3D(1,10,1));

    if(mMode.translate && mMode.rotate)
    {
        m_shaderProgram->setUniformValue("color", QVector3D(0,0,0));
        m_shaderProgram->setUniformValue("ProjectionMatrix", scene->m_projection_matrix);
        m_shaderProgram->setUniformValue("ViewMatrix", scene->m_arcCamera.toMatrix());

        m_shaderProgram->setUniformValue("IDindex",  TRANSLATE_Y);
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * scaleXZ *  globalScale);
            vecotorModel->draw();
        m_shaderProgram->setUniformValue("IDindex",  ROTATE_Y);
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * scaleY *  globalScale);
            circleModel->draw();

        m_shaderProgram->setUniformValue("IDindex",  TRANSLATE_X);
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localX * scaleXZ *globalScale);
            vecotorModel->draw();
        m_shaderProgram->setUniformValue("IDindex",  ROTATE_X);
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localX * scaleY *globalScale);
            circleModel->draw();

        m_shaderProgram->setUniformValue("IDindex",  TRANSLATE_Z);
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localZ  * scaleXZ * globalScale);
            vecotorModel->draw();
        m_shaderProgram->setUniformValue("IDindex",  ROTATE_Z);
        m_shaderProgram->setUniformValue("ModelMatrix",  m_Transform.toMatrix() * localZ  * scaleY * globalScale);
            circleModel->draw();
    }

}


Transform Manipulator::getTransform()
{
    return m_Transform;
}

void Manipulator::setTransform(const Transform &_transform)
{
    m_Transform = _transform;
}

void Manipulator::setRotation(const QQuaternion &_rot)
{
    m_Transform.setRotation(_rot);
}

void Manipulator::setTranslation(const QVector3D &_translation)
{
    m_Transform.setTranslation(_translation);
}

void Manipulator::setState(State _state)
{
    currentState = _state;
}

void Manipulator::setActive(bool _active)
{
    m_isActive = _active;
}

bool Manipulator::isActive()
{
    return m_isActive;
}

void Manipulator::update()
{
    if(!isActive())
        return;

    if(m_window)
    {
        if(isDraging)
            return;

        float pixelf[3];
        QPoint screenMouse = m_window->getMouseScreenCoords();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer->m_fbo);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(screenMouse.x(), screenMouse.y(), 1, 1, GL_RGB, GL_FLOAT, &pixelf);

        QQuaternion rot;
        switch(uint(pixelf[0]))
        {
            case NONE:
                currentState = NONE;
                localAxis = QVector3D(0,1,0);
                break;

            case TRANSLATE_X:
                currentState = TRANSLATE_X;
                if(worldSpace){localAxis = QVector3D(1,0,0); break;}
                rot = m_Transform.rotation();
                localAxis = rot.rotatedVector(QVector3D(1,0,0));
                break;

            case TRANSLATE_Y:
                currentState = TRANSLATE_Y;
                if(worldSpace){localAxis = QVector3D(0,1,0); break;}
                rot = m_Transform.rotation();
                localAxis = rot.rotatedVector(QVector3D(0,1,0));
                break;

            case TRANSLATE_Z:
                currentState = TRANSLATE_Z;
                if(worldSpace){localAxis = QVector3D(0,0,1); break;}
                rot = m_Transform.rotation();
                localAxis = rot.rotatedVector(QVector3D(0,0,1));
                break;

            case ROTATE_X:
                currentState = ROTATE_X;
                if(worldSpace){localAxis = QVector3D(1,0,0); break;}
                rot = m_Transform.rotation();
                localAxis = rot.rotatedVector(QVector3D(1,0,0));
                break;

            case ROTATE_Y:
                currentState = ROTATE_Y;
                if(worldSpace){localAxis = QVector3D(0,1,0); break;}
                rot = m_Transform.rotation();
                localAxis = rot.rotatedVector(QVector3D(0,1,0));
                break;

            case ROTATE_Z:
                currentState = ROTATE_Z;
                if(worldSpace){localAxis = QVector3D(0,0,1); break;}
                rot = m_Transform.rotation();
                localAxis = rot.rotatedVector(QVector3D(0,0,1));
                break;

            case TRANSLATE_VIEWPLANE:
                currentState = TRANSLATE_VIEWPLANE;
                break;
        }
    }
}

void Manipulator::updateLocalView()
{
    QVector3D camPlaneN = (scene->m_arcCamera.worldPos() - m_Transform.translation()).normalized();
    QVector3D localUp = QVector3D::crossProduct(camPlaneN, QVector3D(0,1,0));
    QQuaternion rotY = QQuaternion::rotationTo(QVector3D(0,1,0), camPlaneN);
    QQuaternion rotX = QQuaternion::rotationTo(QVector3D(1,0,0), localUp);

    QQuaternion currentRot = m_Transform.rotation();
    QMatrix4x4 tmp;
    tmp.rotate(currentRot);
    QMatrix4x4 rotInv = tmp.inverted();

    localView.setToIdentity();
    localView = rotInv * localView;
    localView.rotate(rotY);
    localView.rotate(rotX);
    localView.scale(0.4,0.4,0.4);
}

void Manipulator::updateGlobalScale()
{
    QVector3D camWPos = scene->m_arcCamera.worldPos();
    QVector3D centerPos = m_Transform.translation();
    QVector3D camToModel = centerPos- camWPos;
    double d = camToModel.length();

    globalScale.setToIdentity();
    globalScale.scale((d / 6));
}

void Manipulator::startDrag()
{
    isDraging = true;
    m_activeObject->processInput(ActiveObject::LM_CLICKED);

    // A ray from a click on the screen into the scene. For Translation and Rotation operations
    QPointF currentMouseNDC = m_window->getMouseNDCCoords();
    Ray camRay = scene->castRayFromCamera(currentMouseNDC.x(), currentMouseNDC.y());

    if(currentState == TRANSLATE_VIEWPLANE)
    {
        QVector4D null = QVector4D(0,0,0,1);
        QVector4D ndcNull = scene->m_projection_matrix * scene->m_arcCamera.toMatrix() * m_Transform.toMatrix() * null ;
        ndcNull = ndcNull / ndcNull.w();
        m_dragStartOffset = QVector3D(ndcNull.x() - currentMouseNDC.x() ,
                                            ndcNull.y() - currentMouseNDC.y() ,
                                            0);

        m_viewPlaneNormalStart = (scene->m_arcCamera.worldPos() - m_Transform.translation()).normalized();
        return;
    }

    if(currentState == TRANSLATE_X || currentState == TRANSLATE_Y || currentState == TRANSLATE_Z )
    {
        QVector3D axis = localAxis.normalized();
        Ray tRay;
        tRay.Origin = m_Transform.translation();
        tRay.Dir = axis.normalized();
        QVector3D closestPoint = scene->m_CollisionDetect.closetPointFromRayToRay(camRay, tRay);
        float d = (closestPoint - m_Transform.translation()).length();
        m_dragStart = m_Transform.translation();
        m_dragStartOffset = m_Transform.translation() + (axis * d);
    }

    if(currentState == ROTATE_X || currentState == ROTATE_Y || currentState == ROTATE_Z)
    {
        QVector3D PointRotPlane = scene->m_CollisionDetect.intersectRayPlane(localAxis, m_Transform.translation(), camRay);
        m_dragStartRotVec = PointRotPlane - m_Transform.translation();
        m_dragStartRot = m_Transform.rotation();
    }
}

void Manipulator::endDrag()
{
    isDraging = false;    
    m_activeObject->processInput(ActiveObject::LM_RELEASED);
}

void Manipulator::drag()
{
    if(currentState == NONE)
        return;

    if(currentState == TRANSLATE_X || currentState == TRANSLATE_Y || currentState == TRANSLATE_Z )
    {
        dragTranslate();
    }
    else if(currentState == ROTATE_X || currentState == ROTATE_Y || currentState == ROTATE_Z )
    {
        dragRotate();
    }
    else if(currentState == TRANSLATE_VIEWPLANE)
    {
        dragTranslateViewPlane();
    }

    if(m_activeObject)
    {
        m_activeObject->notify(m_Transform);
        m_activeObject->processInput(ActiveObject::LM_PRESSED);
    }
}

void Manipulator::dragRotate()
{
    QPointF currentMouseNDC = m_window->getMouseNDCCoords();
    Ray camRay = scene->castRayFromCamera(currentMouseNDC.x(), currentMouseNDC.y());
    QVector3D PointRotPlane = scene->m_CollisionDetect.intersectRayPlane(localAxis, m_Transform.translation(), camRay);
    QVector3D rotateTo = PointRotPlane - m_Transform.translation();

    QQuaternion WorldtoWorldNew = QQuaternion::rotationTo(m_dragStartRotVec, rotateTo);
    QQuaternion finalRot =  WorldtoWorldNew * m_dragStartRot;

    m_Transform.setRotation(finalRot);

}



void Manipulator::dragTranslate()
{
    QPointF currentMouseNDC = m_window->getMouseNDCCoords();
    Ray camRay = scene->castRayFromCamera(currentMouseNDC.x(), currentMouseNDC.y());

    QVector3D axis = localAxis;
    Ray tRay;
    tRay.Origin = m_Transform.translation();
    tRay.Dir = axis.normalized();

    QVector3D closestPoint = scene->m_CollisionDetect.closetPointFromRayToRay(camRay, tRay);
    QVector3D target = (m_dragStartOffset - closestPoint);
    float rdis = target.length() ;

    double dot = QVector3D::dotProduct(target, axis);
    float sign = signbit(dot) ? 1 : -1;
    float ds = rdis * sign;
    axis.normalize();

    QVector3D t;
    switch(currentState)
    {
        case NONE:
            break;

        case TRANSLATE_X:
            t = m_dragStart + (axis * ds);
            m_Transform.setTranslation(t);
            break;
        case TRANSLATE_Y:
            t = m_dragStart + (axis * ds);
            m_Transform.setTranslation(t);
            break;
        case TRANSLATE_Z:
            t = m_dragStart + (axis * ds);
            m_Transform.setTranslation(t);
            break;
    }
}

void Manipulator::dragTranslateViewPlane()
{
    QPointF currentMouseNDC = m_window->getMouseNDCCoords();
    QVector3D null = QVector3D(0,0,0);
    QVector3D ndcNull = scene->m_projection_matrix * scene->m_arcCamera.toMatrix() * m_Transform.toMatrix() * null;

    QVector3D mouseToObject = QVector3D(ndcNull.x() - currentMouseNDC.x() ,
                                        ndcNull.y() - currentMouseNDC.y() ,
                                        0);
    Ray camRay = scene->castRayFromCamera(currentMouseNDC.x() + m_dragStartOffset.x(), currentMouseNDC.y() + m_dragStartOffset.y());

    QVector3D pN = m_viewPlaneNormalStart;
    QVector3D PointRotPlane = scene->m_CollisionDetect.intersectRayPlane(pN, m_Transform.translation(), camRay);
    m_Transform.setTranslation(PointRotPlane);
}
















