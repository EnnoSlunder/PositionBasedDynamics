#ifndef DYNAMICSWORLDCONTROLLER_H
#define DYNAMICSWORLDCONTROLLER_H

#include <QObject>

class Scene;

class DynamicsWorld;

class DynamicsWorldController : public QObject
{
Q_OBJECT
public:
    DynamicsWorldController();
    DynamicsWorldController(DynamicsWorld* _dynamics);

protected slots:
    void startStopSim();
    void stepSim();
    void resetSim();
    void setParticleMass(float _mass);
    void setGravityY(float _y);
    void setTimeStepSize(float _ts);
    void setPreConditionIteration(int _pciter);
    void setConstraintIteration(int _citer);
    void setPBDDamping(float _damp);
    void setDistanceConstraintStretch(float _stretch);
    void setDistanceConstraintCompress(float _compress);
    void setShapeMatchingConstraintAttract(float _attract);
private:
    bool m_simulating;
    DynamicsWorld *m_dynamicsWorld;

};

#endif // DYNAMICSWORLDCONTROLLER_H
