#ifndef ABSTRACTCONSTRAINT_H
#define ABSTRACTCONSTRAINT_H

#include <memory>
#include <vector>

#include  <QDebug>
#include <QVector3D>

#include "utils.h"

class Particle;

class AbstractConstraint
{
public:
    enum ConstraintType{
       NONE,
       HALFSPACE,
       HALFSPACE_PRE,
       PIN,
       PINTOGETHER,
       PARTICLEPARTICLE,
       PARTICLEPARTICLE_PRE,
       DISTANCE,
       SHAPEMATCH,
       SHAPEMATCH_RIGID,
       FRICTION,
       FRICTIONHALFSPACE
    };

    AbstractConstraint();
    inline virtual void project(){}
    inline virtual void projectPrevious(){}
    inline virtual float constraintFunction(){qDebug()<<" Abstract C"; return 1.0;}
    inline virtual QVector3D deltaP(){}
    inline ConstraintType type(){ return m_type;}
    void setDirty(bool _isDirty);

// members
    bool m_dirty = true;
    ConstraintType m_type;
    std::vector<ParticleWeakPtr> m_Particles;

};


#endif // ABSTRACTCONSTRAINT_H
