#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector3D>
#include <QDebug>

#include "dynamics/dynamicUtils.h"
#include "dynamics/dynamicObject.h"

class AbstractConstraint;

class Particle : public std::enable_shared_from_this<Particle>
{
public:
    Particle();
    Particle(float _x, float _y, float _z, int _ID);

    void setRadius(float _radius);
    void setMass(float _mass);
    void setCell(int _i, int _j, int _k);
    void setHash(size_t _hashv);
    int ID = 0;
    int bodyID = 0;

    QVector3D position();
    float radius();
    float mass();
    ParticlePtr pointer(Particle *ptr);

    void pinToPosition(const QVector3D &_pos);
    void endPinToPosition();

    void pointer(Particle** _ptr);
    const QMatrix4x4 getTransfrom();
    const QVector3D getTranslation();



//members :
    unsigned age;
    float w, r, m;
    float collisionGradLen = 0;
    size_t hash;
    QVector3D p, pp, v, vt, x, xt, qc;
    QVector3D collisionVector;
    int3 cell;

    std::list<ParticlePtr> m_NonCollisionParticles;
    std::vector<ConstraintPtr> m_CollisionConstraints;

    std::vector<ConstraintPtr> m_PreConditionConstraints;
    std::vector<ConstraintWeakPtr> m_Constraints;

};

inline QVector3D Particle::position(){ return x; };
inline float Particle::radius(){ return r; };

#endif // PARTICLE_H
