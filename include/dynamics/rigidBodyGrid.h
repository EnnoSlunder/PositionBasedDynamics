#ifndef RIGIDBODYGRID_H
#define RIGIDBODYGRID_H

#include "dynamicObject.h"
#include "dynamics/dynamicUtils.h"
#include "dynamics/constraint.h"

class RigidBodyGrid : public DynamicObject
{
public:
    RigidBodyGrid();
    RigidBodyGrid(ModelPtr _model);

    void addParticle(const QVector3D &_localPos, const ParticleWeakPtr _particle);
    ConstraintPtr createConstraint();

    // virtuals
    void pinToPosition(const QVector3D &_pos);
    void endPinToPosition();
    void updateModelBuffers();
    const QMatrix4x4 getTransfrom();
    const QVector3D getTranslation();

    std::vector<ParticleWeakPtr>& getParticles();
    int numParticles();

private:
    friend ShapeMatchingConstraint;
    int id;

    std::vector<QVector3D> m_restShape;
    ModelPtr m_model;
    ShapePtr m_shape;
    QMatrix4x4 m_t;

    std::vector<ParticleWeakPtr> m_particles;
};

#endif // RIGIDBODYGRID_H
