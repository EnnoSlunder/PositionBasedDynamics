#ifndef SINGLEPARTICLE_H
#define SINGLEPARTICLE_H

#include "dynamics/dynamicUtils.h"
#include "dynamics/dynamicObject.h"

class SingleParticle : public DynamicObject
{
public:
    SingleParticle(ParticlePtr _p);

    const QMatrix4x4 getTransfrom();
    const QVector3D getTranslation();

    virtual void pinToPosition(const QVector3D &_pos);
    virtual void endPinToPosition();
    ParticlePtr pointer(Particle *ptr);
    std::vector<ParticleWeakPtr>& getParticles();
    int numParticles();

private:
    QMatrix4x4 m_mat4;
    ParticlePtr m_particle;
    float m_radius;
};

#endif // SINGLEPARTICLE_H
