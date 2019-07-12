#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include "utils.h"



class Transform
{
public:

  uint updateCounter;
  Transform();

  // Transform By (Add/Scale)
  void translate(const QVector3D &dt);
  void translate(float dx, float dy, float dz);
  void scale(const QVector3D &ds);
  void scale(float dx, float dy, float dz);
  void scale(float factor);
  void rotate(const QQuaternion &dr);
  void rotate(float angle, const QVector3D &axis);
  void rotate(float angle, float ax, float ay, float az);
  void grow(const QVector3D &ds);
  void grow(float dx, float dy, float dz);
  void grow(float factor);

  // Transform To (Setters)
  void setTranslation(const QVector3D &t);
  void setTranslation(float x, float y, float z);
  void setScale(const QVector3D &s);
  void setScale(float x, float y, float z);
  void setScale(float k);

  void setRotation(const QVector3D &_eulerAngles);
  void setRotation(const QQuaternion &r);
  void setRotation(float angle, const QVector3D &axis);
  void setRotation(float angle, float ax, float ay, float az);

  // Accessors
  const QVector3D& translation() const;
  const QVector3D& scale() const;
  const QQuaternion& rotation() const;
  const QMatrix4x4& toMatrix();
  const QMatrix4x4& toInverseMatrix();

  // Queries
  QVector3D forward() const;
  QVector3D up() const;
  QVector3D right() const;

private:
  bool m_dirty;
  QVector3D m_translation;
  QVector3D m_scale;
  QQuaternion m_rotation;
  QMatrix4x4 m_world;

#ifndef QT_NO_DATASTREAM
  friend QDataStream &operator<<(QDataStream &out, const Transform &transform);
  friend QDataStream &operator>>(QDataStream &in, Transform &transform);
#endif
};

Q_DECLARE_TYPEINFO(Transform, Q_MOVABLE_TYPE);

// Transform By (Add/Scale)
inline void Transform::translate(float dx, float dy,float dz) { translate(QVector3D(dx, dy, dz)); }
inline void Transform::scale(float dx, float dy,float dz) { scale(QVector3D(dx, dy, dz)); }
inline void Transform::scale(float factor) { scale(QVector3D(factor, factor, factor)); }
inline void Transform::rotate(float angle, const QVector3D &axis) { rotate(QQuaternion::fromAxisAndAngle(axis, angle)); }
inline void Transform::rotate(float angle, float ax, float ay,float az) { rotate(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }
inline void Transform::grow(float dx, float dy, float dz) { grow(QVector3D(dx, dy, dz)); }
inline void Transform::grow(float factor) { grow(QVector3D(factor, factor, factor)); }

// Transform To (Setters)
inline void Transform::setTranslation(float x, float y, float z) { setTranslation(QVector3D(x, y, z)); }
inline void Transform::setScale(float x, float y, float z) { setScale(QVector3D(x, y, z)); }
inline void Transform::setScale(float k) { setScale(QVector3D(k, k, k)); }
inline void Transform::setRotation(float angle, const QVector3D &axis) { setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
inline void Transform::setRotation(float angle, float ax, float ay, float az) { setRotation(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }

// Accessors
inline const QVector3D& Transform::translation() const { return m_translation; }
inline const QVector3D& Transform::scale() const { return m_scale; }
inline const QQuaternion& Transform::rotation() const { return m_rotation; }

// Qt Streams
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Transform &transform);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &out, const Transform &transform);
QDataStream &operator>>(QDataStream &in, Transform &transform);
#endif

#endif // TRANSFORM3D_H
