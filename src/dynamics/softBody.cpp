#include "include/dynamics/softBody.h"

SoftBody::SoftBody()
{

}

SoftBody::SoftBody(ModelPtr _model)
{
    // clone the model. vertices are deformed (vbo is reallocated every frame) :(
    m_model = std::make_shared<Model>();
    m_model->clone(_model);
}

void SoftBody::addParticle(const QVector3D &_localPos, const ParticleWeakPtr _particle)
{
    m_particles.push_back(_particle);
}

std::vector<std::set<int> > SoftBody::createConstraintNetwork()
{
    std::vector< std::set<int> > constraintIdxPairs;
    ShapePtr shape = getModel()->getShape(0);
    std:: vector<unsigned int> vertIndices = shape->getIndices();
    int verticesSize = shape->getVertices().size()-1;

    for(int i=2; i < vertIndices.size(); i+=3 )
    {
        int vertIdxA = vertIndices[i-2];
        int vertIdxB = vertIndices[i-1];
        int vertIdxC = vertIndices[i];

        int poitIdxA = findKbyValueElement(shape->getVertsMap(), vertIdxA);
        int poitIdxB = findKbyValueElement(shape->getVertsMap(), vertIdxB);
        int poitIdxC = findKbyValueElement(shape->getVertsMap(), vertIdxC);

        std::set<int> edgeAB = {poitIdxA, poitIdxB};
        std::set<int> edgeBA = {poitIdxB, poitIdxC};
        std::set<int> edgeCA = {poitIdxC, poitIdxA};

        std::vector<std::set<int>> edges = {edgeAB, edgeBA, edgeCA};
        for(auto edge : edges)
        {
            if(!hasPair(constraintIdxPairs, edge))
                constraintIdxPairs.push_back(edge);
        }
    }
    return constraintIdxPairs;
}

void SoftBody::turnOffSelfCollision()
{
    for(auto p : m_particles)
    {
        if(auto particle = p.lock())
        {
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
}

void SoftBody::updateModelBuffers()
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

ModelPtr SoftBody::getModel()
{
    return m_model;
}

const QMatrix4x4 SoftBody::getTransfrom()
{
    updateModelBuffers();
    QMatrix4x4 identity;
    identity.setToIdentity();
    return  identity;
}

std::vector<ParticleWeakPtr> &SoftBody::getParticles()
{
    return m_particles;
}

int SoftBody::numParticles()
{
    return m_particles.size();
}

