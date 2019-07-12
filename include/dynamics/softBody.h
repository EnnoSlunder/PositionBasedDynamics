#ifndef SOFTBODY_H
#define SOFTBODY_H

#include "dynamicObject.h"
#include "dynamics/dynamicUtils.h"
#include "dynamics/constraint.h"

class SoftBody : public DynamicObject
{
public:
    SoftBody();
    SoftBody(ModelPtr _model);

    void addParticle(const QVector3D &_localPos, const ParticleWeakPtr _particle);
    std::vector<std::set<int> > createConstraintNetwork();
    void turnOffSelfCollision();

    void updateModelBuffers();

    ModelPtr getModel();
    const QMatrix4x4 getTransfrom();
    std::vector<ParticleWeakPtr>& getParticles();
    int numParticles();

private:
    ModelPtr m_model;
    std::vector<ParticleWeakPtr> m_particles;
};

#endif // SOFTBODY_H
