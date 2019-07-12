#include "camera.h"
#include <cmath>
#include <QDebug>




const QVector3D Camera3D::LocalForward(0.0f, 0.0f, -1.0f);
const QVector3D Camera3D::LocalUp(0.0f, 1.0f, 0.0f);
const QVector3D Camera3D::LocalRight(1.0f, 0.0f, 0.0f);

const float m_pi = 3.14159265;


Camera3D::Camera3D()
    : m_dirty(true), m_worldPos(m_translation)
{
    m_pivot = QVector3D(0.0, 0.0, 0.0);
}


// Transform By (Add/Scale)
void Camera3D::translate(const QVector3D &dt)
{
  m_dirty = true;
  m_translation += dt;
}

void Camera3D::rotate(const QQuaternion &dr)
{
  m_dirty = true;
  m_rotation = dr * m_rotation;
}

void Camera3D::rotateArcBall(const QPoint &_mousePos, const QPoint &_mouseTriggeredPos, const int _radius)
{
    //https://pixeladventuresweb.wordpress.com/2016/10/04/arcball-controller/
    QVector3D P1, P2, _axis;
    QQuaternion _rot;

    float _x2 = std::abs(_mousePos.x());
    float _y2 = std::abs(_mousePos.y());
    float _x1 = std::abs(_mouseTriggeredPos.x());
    float _y1 = std::abs(_mouseTriggeredPos.y());
    float _z1, _z2, _angle, _dot;

    float _tempRadius = _radius;

    float _rTest1 = std::sqrt(std::pow(_x1,2) + std::pow(_y1, 2));
    float _rTest2 = std::sqrt(std::pow(_x2,2) + std::pow(_y2, 2));

    if(_rTest2 <= _tempRadius)
    {
        _z2 = std::sqrt(std::pow(_tempRadius,2) - std::pow(_x2,2) - std::pow(_y2,2));
    }
    else
    {
        _z2 = 0.0;
    }
    if(_rTest1 <= _tempRadius)
    {
        _z1 = std::sqrt(std::pow(_tempRadius,2) - std::pow(_x1,2) - std::pow(_y1,2));
    }
    else
    {
        _z1 = 0.0;
    }
    P1 = QVector3D(_mouseTriggeredPos.x(), _mouseTriggeredPos.y(), _z1);
    P2 = QVector3D(_mousePos.x(), _mousePos.y(), _z2);
    _axis = QVector3D::crossProduct(P1,P2);
    _dot = QVector3D::dotProduct(P1.normalized(),P2.normalized());
    _angle = std::acos(_dot) * (180 / m_pi);
    _rot = QQuaternion::fromAxisAndAngle(_axis, _angle);

    m_rotation = _rot * m_startRotation;
    m_deltaRotation = _rot;

    m_dirty = true;
}

void Camera3D::orbit(const QPoint &_mousePos, const QPoint &_mouseTriggeredPos)
{
    QQuaternion _rotY, _rotX;
    QMatrix4x4 _orbitA, _orbitB;
    _orbitA.setToIdentity();
    _orbitB.setToIdentity();

    float _x2 = _mousePos.x();
    float _y2 = _mousePos.y();
    float _x1 = _mouseTriggeredPos.x();
    float _y1 = _mouseTriggeredPos.y();
    float dx = (_x2 - _x1) * 1;
    float dy = (_y2 - _y1) * 1;


    _orbitA.rotate(dx, QVector3D(0, 1, 0));

    _orbitB.rotate(-dy, QVector3D(1,0,0));

    _rotY = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), dx);
    _rotX = QQuaternion::fromAxisAndAngle(right(), -dy);

    m_tumble = m_tumbleStart * _orbitA;
    m_SecondTumble = m_SecondTumbleStart * _orbitB;

    QQuaternion _test = quaternionFromMat4x4(_orbitA);

     m_dirty = true;
}

void Camera3D::orbit2(const QPoint &_mousePos, const QPoint &_mouseTriggeredPos)
{
    QQuaternion _rotY, _rotX;
    QMatrix4x4 _orbit;
    _orbit.setToIdentity();

    float _x2 = _mousePos.x();
    float _y2 = _mousePos.y();
    float _x1 = _mouseTriggeredPos.x();
    float _y1 = _mouseTriggeredPos.y();
    float dx = (_x2 - _x1) * 1;
    float dy = (_y2 - _y1) * 1;

    _orbit.rotate(-dy, right());
    _orbit.rotate(dx, QVector3D(0, 1, 0));

    _rotY = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), dx);
    _rotX = QQuaternion::fromAxisAndAngle(right(), -dy);

     m_dirty = true;
}



void Camera3D::track(const QPoint &_mousePos)
{
    // (T) move delta cal in to input manager
    QPoint deltaMouse = _mousePos - m_prevMousePos;
    m_prevMousePos = _mousePos;
    QVector3D dx = -0.01f * deltaMouse.x() * right();
    QVector3D dy = -0.01f * deltaMouse.y() * up();
    translate(dx + dy);
    m_pivot +=  (dx + dy);
    m_dirty = true;
}

void Camera3D::trackStart(const QPoint &_mousePos)
{
    m_prevMousePos = _mousePos;
}

void Camera3D::reset(const QPoint &_mousePos)
{
    m_world.setToIdentity();
    m_translation = QVector3D(0,0,6);
    m_rotation.setVector(0,0,0);
    m_rotation.setScalar(1);
    m_pivot = QVector3D(0,0,0);
    m_prevMousePos = QPoint(0,0);
    m_tumble.setToIdentity();
    m_dirty = true;
}

// Transform To (Setters)
void Camera3D::setTranslation(const QVector3D &t)
{
  m_dirty = true;
  m_translation = t;
}

void Camera3D::setRotation(const QQuaternion &r)
{
  m_dirty = true;
  m_rotation = r;
}

void Camera3D::arcBallStart()
{
    m_startRotation = m_rotation;
    m_PreWorldPos = m_worldPos;
    m_PrePivotToCam = m_pivotToCam;
    m_tumbleStart = m_tumble;
    m_SecondTumbleStart = m_SecondTumble;
}

void Camera3D::dolly(const QPoint &_mousePos)
{
    QPoint deltaMouse = _mousePos - m_prevMousePos;
    m_prevMousePos = _mousePos;
    QVector3D dz = -0.01f * deltaMouse.x() * forward();
    translate(dz);
    m_pivot +=  (-0.01f * deltaMouse.x() * forward());
    m_dirty = true;
}

// Accessors
const QMatrix4x4 &Camera3D::toMatrix()
{
  if (m_dirty)
  {
    m_dirty = false;
    m_world.setToIdentity();
    m_world.translate(m_pivot);
    m_world.translate(-m_translation);

    m_world *= m_SecondTumble;
    m_world *= m_tumble;

    m_world.translate(-m_pivot);

    QMatrix4x4 myRotMat = m_SecondTumble * m_tumble;
    m_worldPos = myRotMat.transposed() * (m_translation - m_pivot) + m_pivot;
  }
  return m_world;
}

const QMatrix4x4 &Camera3D::toMatrixProjection()
{
  if (m_dirty)
  {
    m_dirty = false;
    m_world.setToIdentity();
    m_world.translate(m_pivot);
    m_world.translate(-m_translation);

    m_world *= m_SecondTumble;
    m_world *= m_tumble;

    m_world.translate(-m_pivot);

    QMatrix4x4 myRotMat = m_SecondTumble * m_tumble;
    m_worldPos = myRotMat.transposed() * (m_translation - m_pivot) + m_pivot;
  }
  return m_world;
}

// Queries
QVector3D Camera3D::forward() const
{
    QMatrix4x4 myRotMat = m_SecondTumble * m_tumble;
    QMatrix4x4 my4 = myRotMat.transposed();

    QVector4D testRight = QVector4D(0,0,1,1);
    QVector4D up = my4 * testRight;
    return QVector3D(up.x(), up.y(), up.z());
}

QVector3D Camera3D::up() const
{
    QMatrix4x4 myRotMat = m_SecondTumble * m_tumble;
    QMatrix4x4 my4 = myRotMat.transposed();

    QVector4D testRight = QVector4D(0,1,0,1);
    QVector4D up = my4 * testRight;
    return QVector3D(up.x(), up.y(), up.z());
}

QVector3D Camera3D::right() const
{
    QMatrix4x4 myRotMat = m_SecondTumble * m_tumble;
    QMatrix4x4 my4 = myRotMat.transposed();

    QVector4D testRight = QVector4D(1,0,0,1);
    QVector4D right = my4 * testRight;
    return QVector3D(right.x(), right.y(), right.z());
}

QVector3D Camera3D::worldPos() const
{
    return m_worldPos;
}

// Qt Streams
QDebug operator<<(QDebug dbg, const Camera3D &transform)
{
  dbg << "Camera3D\n{\n";
  dbg << "Position: <" << transform.translation().x() << ", " << transform.translation().y() << ", " << transform.translation().z() << ">\n";
  dbg << "Rotation: <" << transform.rotation().x() << ", " << transform.rotation().y() << ", " << transform.rotation().z() << " | " << transform.rotation().scalar() << ">\n}";
  return dbg;
}

QDataStream &operator<<(QDataStream &out, const Camera3D &transform)
{
  out << transform.m_translation;
  out << transform.m_rotation;
  return out;
}

QDataStream &operator>>(QDataStream &in, Camera3D &transform)
{
  in >> transform.m_translation;
  in >> transform.m_rotation;
  transform.m_dirty = true;
  return in;
}
