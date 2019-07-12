#ifndef RigidBody_H
#define RigidBody_H

#include "dynamicObject.h"
#include "dynamics/dynamicUtils.h"
#include "dynamics/constraint.h"

class RigidBody : public DynamicObject,  public std::enable_shared_from_this<RigidBody>
{
public:
    RigidBody();
    RigidBody(ModelPtr _model);

    void addParticle(const QVector3D &_localPos, const ParticleWeakPtr _particle);
    ConstraintPtr createConstraint();

    // virtuals
    void pinToPosition(const QVector3D &_pos);
    void endPinToPosition();
    void updateModelBuffers();

    ModelPtr getModel();
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

    std::vector<ParticleWeakPtr> m_particles;

};

#endif // RigidBody_H
