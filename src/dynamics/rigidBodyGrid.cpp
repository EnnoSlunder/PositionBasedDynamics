#include "include/dynamics/rigidBodyGrid.h"

RigidBodyGrid::RigidBodyGrid()
{

}

RigidBodyGrid::RigidBodyGrid(ModelPtr _model)
{

}


void RigidBodyGrid::addParticle(const QVector3D &_localPos, const ParticleWeakPtr _particle)
{
    m_particles.push_back(_particle);
    m_restShape.push_back(_localPos);
}

ConstraintPtr RigidBodyGrid::createConstraint()
{
    auto smCstr = std::make_shared<ShapeMatchingConstraint>(this);
    std::weak_ptr<ShapeMatchingConstraint> smCstrWeak = smCstr;
    for(auto p : m_particles)
    {
        if(auto particle = p.lock())
        {
            particle->m_Constraints.push_back(smCstrWeak);
            for_each(m_particles.begin(), m_particles.end(), [&particle](ParticleWeakPtr pNeighbour)
            {
                if(particle != pNeighbour.lock())
                {
                    std::shared_ptr<Particle> sharedNeighbour = pNeighbour.lock();
                    particle->m_NonCollisionParticles.push_back(sharedNeighbour);
//                    particle->m_NonCollisionParticles.push_back(pNeighbour);
                }
            });
        }
    }
    return smCstr;
}

void RigidBodyGrid::pinToPosition(const QVector3D &_pos)
{

}

void RigidBodyGrid::endPinToPosition()
{

}

void RigidBodyGrid::updateModelBuffers()
{


}

const QMatrix4x4 RigidBodyGrid::getTransfrom()
{

    return  m_t;
}

const QVector3D RigidBodyGrid::getTranslation()
{
    return QVector3D(0,0,0);
}

std::vector<ParticleWeakPtr> &RigidBodyGrid::getParticles()
{
    return m_particles;
}

int RigidBodyGrid::numParticles()
{
    return m_particles.size();
}
