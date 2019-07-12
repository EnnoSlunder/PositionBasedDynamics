#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include <vector>

#include <QVector3D>

#include <transform.h>
#include <model.h>

#include "dynamics/dynamicUtils.h"



class DynamicObject
{
public:
    DynamicObject();

    virtual void pinToPosition(const QVector3D &_pos);
    virtual void endPinToPosition();

    virtual ParticlePtr pointer(Particle *ptr){
        return nullptr;
    }

    virtual const QMatrix4x4 getTransfrom();
    virtual const QVector3D getTranslation();
    virtual std::vector<ParticleWeakPtr>& getParticles(){ std::vector<ParticleWeakPtr> vec; return vec; }
    virtual int numParticles(){};

// members :
    int mID;
};

#endif // DYNAMICOBJECT_H
