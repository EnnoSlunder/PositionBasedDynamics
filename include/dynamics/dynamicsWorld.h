#ifndef DYNAMICSWORLD_H
#define DYNAMICSWORLD_H

#include <vector>
#include <memory>

#include <QDebug>
#include <QVector3D>

//#include <eigen3/Eigen/Dense>

#include "transform.h"
#include "utils.h"
#include "dynamics/dynamicUtils.h"
#include "sceneobject.h"
#include "hashgrid.h"
#include "dynamics/dynamicObject.h"
#include "dynamics/particle.h"
#include "dynamics/rigidBody.h"
#include "dynamics/rigidBodyGrid.h"
#include "dynamics/softBody.h"
#include "dynamics/singleParticle.h"
#include "dynamics/collisiondetection.h"
#include "dynamics/constraint.h"
#include "dynamicsWorldController.h"

typedef QVector3D Vec3;

class Scene;

class DynamicsWorld
{
    public:
        DynamicsWorld();
        void initialize();
        void initialize(Scene *_scene);
        void update();
        void info();
        DynamicsWorldController* controller();
        void setSimulate(bool _isSimulating);
        void setAllParticlesMass(float _m);
        void setAllDistanceConstraintStretch(float _globalStretch);
        void setAllDistanceConstraintCompress(float _globalCompress);
        void reset();
        void step();
        int getTimeStepSizeMS();

        void pbdDamping();
        void compare(ParticlePtr _a);

        void addDynamicObject(pSceneOb _sceneObject);
        DynamicObjectPtr addDynamicObjectAsParticle(pSceneOb _sceneObject);
        DynamicObjectPtr addDynamicObjectAsRigidBody(pSceneOb _sceneObject, int color=0);
        DynamicObjectPtr addDynamicObjectAsSoftBody(pSceneOb _sceneObject, float _mass = 1);

        void addRope(const QVector3D &_start, const QVector3D &_end, int _numParticles);
        void addDynamicObjectAsRigidBodyGrid(pSceneOb _sceneObject, std::string _path, int _color = 0);
        DynamicObjectPtr addDynamicObjectAsNonUniformParticle(pSceneOb _sceneObject, float radius);

        ParticlePtr getParticlePtrFromRawPtr (Particle *_ptr);
        ParticlePtr addParticle(float _x, float _y, float _z);
        void addPlane(const Plane &_plane);
        void collisionCheckAll();
        void collisionCheck(ParticlePtr p);

        void checkSphereSphere(const ParticlePtr p1, const ParticlePtr p2);
        void checkSpherePlane(const ParticlePtr p1, const Plane &_plane);

        std::shared_ptr<DistanceEqualityConstraint> addDistanceEqualityConstraint(const ParticlePtr _p1, const ParticlePtr _p2);
        std::shared_ptr<PinTogetherConstraint>      addPinTogetherConstraint(std::vector<ParticlePtr> &_vec);
        void addParticleParticleConstraint(const ParticlePtr _p1, const ParticlePtr _p2);
        void addParticleParticlePreConditionConstraint(const ParticlePtr _p1, const ParticlePtr _p2);
        void addFrictionConstraint(const ParticlePtr _p1, const ParticlePtr _p2);
        void addHalfSpaceFrictionConstraint(const ParticlePtr _p1, const QVector3D _o, const QVector3D _n);
        void addHalfSpacePreConditionConstraint(const ParticlePtr _p1, const const QVector3D _qc, const QVector3D _planeNormal);
        void deleteConstraint(const ConstraintPtr _constraint);
        void deleteParticle();

        void generateData();
        int frameCount();

        QVector3D* debugDrawLineData();

        int pCount = 0;
        int objectCount = 0;

//    private:
        bool m_simulate;
        int m_ID = 0;
        int m_frameCount;
        int m_preConditionIteration;
        int m_constraintIteration;
        float m_dt, m_pbdDamping;
        float m_frictionConstraintStatic, m_frictionConstraintDynamic, m_shapeMatchAttract,
              m_distanceConstraintCompress, m_DistanceConstraintStretch;

        QVector3D m_gravity;
        DynamicsWorldController         *m_DynamicsWorldController;
        std::vector <DynamicObjectPtr>  m_DynamicObjects;
        std::vector <ParticlePtr>       m_Particles;
        std::vector <ParticlePtr>       m_NonUniformParticles;
        std::vector <ConstraintPtr>     m_Constraints;
        std::vector <Plane>             m_Planes;

        std::vector<QVector3D*>         m_debugLines;

        HashGrid m_hashGrid;
        CollisionDetection m_CollisionDetect;

        Scene *m_scene;

};

#endif // DYNAMICSWORLD_H
