#include "include/dynamics/RigidBody.h"

RigidBody::RigidBody()
{

}

RigidBody::RigidBody(ModelPtr _model)
{
    // deep copy model instead of
    m_model = std::make_shared<Model>();
    m_model->clone(_model);
    m_model->setHidden(false);
}

void RigidBody::addParticle(const QVector3D &_localPos, const ParticleWeakPtr _particle)
{
    m_particles.push_back(_particle);
    m_restShape.push_back(_localPos);
}

ConstraintPtr RigidBody::createConstraint()
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
                }
            });
        }
    }
    return smCstr;
}


void RigidBody::pinToPosition(const QVector3D &_pos)
{

}

void RigidBody::endPinToPosition()
{

}

void RigidBody::updateModelBuffers()
{
    for(unsigned int i = 0; i < m_model->getNumShapes(); i++)
    {
        ShapePtr shape = m_model->getShape(i);
        for(int i=0; i < shape->getVertsMap().size();  i++)
        {
            QVector3D position = m_particles[i].lock()->getTranslation();
            for(auto vertIdx : shape->getVertsMap()[i])
            {
                shape->setVertexPositionAtIndex(vertIdx, position);
            }
        }
        shape->updateVertexBuffer();
    }
}

ModelPtr RigidBody::getModel()
{
    return  m_model;
}

const QMatrix4x4 RigidBody::getTransfrom()
{
    updateModelBuffers();
    QMatrix4x4 identity;
    identity.setToIdentity();
    return  identity;
}

const QVector3D RigidBody::getTranslation()
{
    return QVector3D(0,0,0);
}

std::vector<ParticleWeakPtr> &RigidBody::getParticles()
{
    return m_particles;
}

int RigidBody::numParticles()
{
    return m_particles.size();
}

