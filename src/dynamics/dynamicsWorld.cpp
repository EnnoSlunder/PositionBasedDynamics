
#include <stdio.h>

#include "dynamics/dynamicsWorld.h"

#include "Scene.h"

#include <Qdebug>

#include <omp.h>

#include "parameters.h"

Eigen::Matrix3f QMatrix3toEigen(const QMatrix3x3 &_qMat);
QMatrix3x3 EigenMatrix3toQt(const Eigen::Matrix3f &_eMat);


DynamicsWorld::DynamicsWorld()
    : m_hashGrid()
{
    qDebug()<<"DynaicsWorld ctor";
    m_simulate = false;
    m_dt = timeStepSize;
//      m_dt = 0.01;
//    m_dt = 0.1;
    m_frameCount = 0;
    m_DynamicsWorldController = new DynamicsWorldController(this);
    m_gravity = QVector3D(gravityDir.x(), gravityDir.y(), gravityDir.z());

    m_preConditionIteration = preConditionIterations;
    m_constraintIteration = constraintIterations;
    m_pbdDamping = pbd_Damping;
}

void DynamicsWorld::initialize()
{
    qDebug()<<"DynamicsWorld initialize ";
//    Eigen::Vector3d test(1,2,3);

//    mlog<<"helloo"<<test.x()<<test.y();

    m_CollisionDetect = CollisionDetection();
    Plane groundPlane;
    groundPlane.Normal = QVector3D(0, 1, 0);
    groundPlane.Offset = QVector3D(0, 0, 0);
    addPlane(groundPlane);

    float offset = 7;

//    Plane pX;
//    pX.Normal = QVector3D(-1, 0, 0);
//    pX.Offset = QVector3D(offset, 0, 0);
//    addPlane(pX);

//    Plane nX;
//    nX.Normal = QVector3D(1, 0, 0);
//    nX.Offset = QVector3D(-offset, 0, 0);
//    addPlane(nX);

//    Plane pZ;
//    pZ.Normal = QVector3D(0, 0, -1);
//    pZ.Offset = QVector3D(0, 0, offset);
//    addPlane(pZ);

//    Plane nZ;
//    nZ.Normal = QVector3D(0, 0, 1);
//    nZ.Offset = QVector3D(0, 0, -offset);
//    addPlane(nZ);

//    auto nSpring = std::make_shared<DistanceEqualityConstraint>(m_Particles[0], m_Particles[1]);
    for( ParticlePtr p : m_Particles)
    {
        p->pp = QVector3D(0,0,0);
    }
}

void DynamicsWorld::initialize(Scene *_scene)
{
    m_scene = _scene;
    initialize();
}

void DynamicsWorld::update()
{
    float dt = m_dt;

    if(!m_simulate)
        return;
//    mlog<<" ---------------void DynamicsWorld::update()----------------";

    // PBD Loop start
    // explicit Euler integration step (5)


    for( ParticlePtr p : m_Particles)
    {
        // e.G. gravity 0, 1, 0
//        QVector3D forceExt = QVector3D(0, -9.8, 0);
        QVector3D forceExt  = m_gravity;
        p->v = p->v + dt * p->w * forceExt;

        if(isnan(p->v.x()) || isnan(p->v.y()) || isnan(p->v.z())){
        }
    }

    // damp Velocities (6)
    pbdDamping();

    for( ParticlePtr p : m_Particles)
    {
        p->p = p->x + dt * p->v;
    }

    collisionCheckAll();

    // Constraint dirty to do something
    for( ParticlePtr p : m_Particles)
    {
        for( ConstraintWeakPtr c : p->m_Constraints){
            if(auto constraint = c.lock()){
                constraint->setDirty(true);
            }
        }
    }

    // Preconditioning (solve particle plane cstrs once)
    for(int i=0; i < m_preConditionIteration; i++)
    {
        for( ParticlePtr p : m_Particles)
        {
            for( ConstraintPtr c : p->m_PreConditionConstraints)
            {
                c->project();
            }

        }
    }
    m_frameCount++;

    // Solver Iteration (9)
    int nthreads, tid, test;
    for(int i=0; i<m_constraintIteration; i++)
    {
//        #pragma omp parallel for
        for(int j=0; j < m_Particles.size(); j++)
        {
    //        for( ConstraintWeakPtr c : p->m_Constraints)
            auto p = m_Particles[j];

            for( ConstraintWeakPtr c : p->m_Constraints)
            {
                if(auto constraint = c.lock()){
                    {
                        constraint->project();
                    }
                }
            }
        }

        for( ParticlePtr p : m_Particles)
        {
            for( ConstraintWeakPtr c : p->m_Constraints)
            {
                if(auto constraint = c.lock()){
                    {
                        constraint->setDirty(true);
                    }
                }
            }

            for( ConstraintPtr c : p->m_CollisionConstraints)
            {
                c->project();
//                c->setDirty(true);
            }
        }
    }

    //delte collisions
    for( ParticlePtr p : m_Particles)
    {
        p->m_CollisionConstraints.clear();
        p->m_PreConditionConstraints.clear();
    }



    // Apply correction (13,14)
    for( ParticlePtr p : m_Particles)
    {
        QVector3D xp = (p->p - p->x);

        // sleep
        if(xp.length() < 0.003){
            p->v = QVector3D(0,0,0);
            continue;
        }

        p->v = xp / dt;
        p->x = p->p;

    }

    for( ParticlePtr p : m_NonUniformParticles)
    {
        for(auto c : p->m_Constraints)
        {
            if(auto constraint = c.lock())
                constraint->project();
        }
        p->x = p->p;
    }


    //     modify velocity (16)
    //    for( ParticlePtr p : m_Particles)
    //    {
    //        p->v = p->v + p->pp;
    //        p->pp = QVector3D(0,0,0);
    //    }


}

void DynamicsWorld::info()
{
    qDebug()<<"p1: "<<m_Particles[0]->x<<m_Particles[0]->ID<<m_Particles[0]->r;
}

DynamicsWorldController* DynamicsWorld::controller()
{
    if(m_DynamicsWorldController)
        return m_DynamicsWorldController;
    return nullptr;
}

void DynamicsWorld::setSimulate(bool _isSimulating)
{
    m_simulate = _isSimulating;
}

void DynamicsWorld::setAllParticlesMass(float _m)
{
    for(auto p : m_Particles)
    {
        if(p->mass() <= 0.001)
            continue;
        p->setMass(_m);
    }
}

void DynamicsWorld::setAllDistanceConstraintStretch(float _globalStretch)
{
//    for(auto c : m_Constraints)
}

void DynamicsWorld::setAllDistanceConstraintCompress(float _globalCompress)
{

}

void DynamicsWorld::step()
{
    m_simulate = true;
    update();
    m_simulate = false;
}

int DynamicsWorld::getTimeStepSizeMS()
{
    return m_dt * 1000;
}

void DynamicsWorld::pbdDamping()
{
    int i = 0;
    for(auto dObject : m_DynamicObjects)
    {
        i++;
        QVector3D xcm, vcm, ri, L, w;
        QMatrix3x3 I;
        float totalMass = 0;

        if(dObject->numParticles() < 1)
            continue;

        for(auto p : dObject->getParticles())
        {
            if(auto particle = p.lock())
            {
                xcm += particle->m * particle->x;
                vcm += particle->m * particle->v;
                totalMass += particle->m;
            }
        }

        if(totalMass <= 0.001)
            return;

        xcm /= totalMass;
        vcm /= totalMass;

        for(auto p : dObject->getParticles())
        {
            if(auto particle = p.lock())
            {
                ri = particle->x - xcm;
                L += QVector3D::crossProduct(ri, (particle->m * particle->v));
                QMatrix3x3 rt;
                rt(0,0) =            0 ; rt(0,1) =       -ri[2] ; rt(0,2) =        ri[1] ;
                rt(1,0) =         ri[2]; rt(1,1) =            0 ; rt(1,2) =       -ri[0] ;
                rt(2,0) =        -ri[1]; rt(2,1) =        ri[0] ; rt(2,2) =            0 ;

                Eigen::Matrix3f rtMat = QMatrix3toEigen(rt);
                Eigen::Matrix3f IMat = rtMat * rtMat.transpose() * particle->m;
                I += EigenMatrix3toQt(IMat);
            }
        }
        Eigen::Vector3f wM = QMatrix3toEigen(I).inverse() * Eigen::Vector3f(L.x(), L.y(), L.z());
        w = QVector3D(wM.x(), wM.y(), wM.z());

        int i = 0;
        for(auto p : dObject->getParticles())
        {
            if(auto particle = p.lock())
            {
                ri = particle->x - xcm;
                QVector3D dv = vcm + (QVector3D::crossProduct(w, ri)) - particle->v;
                i++;
                particle->v +=  (m_pbdDamping * dv);
            }
        }
    }
}

void DynamicsWorld::compare(ParticlePtr _a)
{
    mlog<<"compare";
}

/*
 *
 * from https://stackoverflow.com/questions/2047414/advantages-of-stdfor-each-over-for-loop
 *
 * for_each(a.begin(), a.end(), a_item_handler);
is better than

for(auto& item: a) {
    a_item_handler(a);
}
*/

void DynamicsWorld::addDynamicObject(pSceneOb _sceneObject)
{

}

DynamicObjectPtr DynamicsWorld::addDynamicObjectAsParticle(pSceneOb _sceneObject)
{
    pCount++;
    QVector3D pos = _sceneObject->getPos();
    auto pParticle = std::make_shared<Particle>(pos.x(), pos.y(), pos.z(), pCount);
    pParticle->ID = pCount;
    pParticle->bodyID = pCount;
    pParticle->setRadius(_sceneObject->getRadius());
//    pParticle->setMass(0);
    mlog<<"New Particle: "<<pParticle->w<<" ID: "<<pParticle->ID ;
    m_Particles.push_back(pParticle);
//    SingleParticle pDynamicObject(pParticle);
    std::shared_ptr<SingleParticle> pDynamicObject = std::make_shared<SingleParticle>(pParticle);
    pDynamicObject->mID = pCount;
    m_DynamicObjects.push_back(pDynamicObject);
    _sceneObject->makeDynamic(pDynamicObject);
    // std::move (?)
    return pDynamicObject;
}

DynamicObjectPtr DynamicsWorld::addDynamicObjectAsRigidBody(pSceneOb _sceneObject, int color)
{
    if(!_sceneObject->model())
        return nullptr;

    // clones it's own model object with all data. (cause model will me deformed)
    auto nRB = std::make_shared<RigidBody>(_sceneObject->model());
    objectCount++;
    ModelPtr model = nRB->getModel();
    _sceneObject->setModel(nRB->getModel());

    for(unsigned int i = 0; i < model->getNumShapes(); i++)
    {
        ShapePtr shape = model->getShape(i);
        for(auto point : shape->getPoints())
        {
            QVector3D pos = _sceneObject->getMatrix() * point;
            pCount++;
            auto nParticle = std::make_shared<Particle>(pos.x(), pos.y(), pos.z(), 33);
            nParticle->ID = pCount;
            nParticle->bodyID = objectCount;
            nParticle->setRadius(_sceneObject->getRadius());

            nParticle->setMass(0.1);

            m_Particles.push_back(nParticle);
            nRB->addParticle(point, nParticle);

            std::shared_ptr<SingleParticle> pDynamicObject = std::make_shared<SingleParticle>(nParticle);
            auto sO = m_scene->addSceneObjectFromParticle(pDynamicObject, nParticle, color);
            sO->isHidden(true);
        }
    }
    auto smCstr = nRB->createConstraint();
    m_Constraints.push_back(smCstr);

    m_DynamicObjects.push_back(nRB);

    nRB->updateModelBuffers();
    _sceneObject->makeDynamic(nRB);

    return nRB;
}

void DynamicsWorld::addDynamicObjectAsRigidBodyGrid(pSceneOb _sceneObject, std::string _path, int _color)
{
    FILE * file = std::fopen(_path.c_str(), "r");
    if( file == nullptr ){
        mlog<<"Impossible to open the file !\n";
        return;
    }

    std::vector<QVector3D> verts;
    std::vector<QVector3D> normals;

    while( 1 ){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.


        if ( std::strcmp( lineHeader, "v" ) == 0 ){
            float x,y,z;
            fscanf(file, "%f %f %f\n", &x, &y, &z );
            verts.push_back(QVector3D(x,y,z));
        // else : parse lineHeader
        }

        if ( std::strcmp( lineHeader, "vn" ) == 0 ){
            float x,y,z;
            fscanf(file, "%f %f %f\n", &x, &y, &z );
            normals.push_back(QVector3D(x,y,z));
        // else : parse lineHeader
        }
    }

    if(normals.size() != verts.size())
        mlog<<"warning -------verts are not normals";

    // make rigid body instance
    auto nRBG = std::make_shared<RigidBodyGrid>();
    objectCount++;

    int i = 0;
    for(auto v : verts)
    {
         pCount++;
         auto nParticle = std::make_shared<Particle>(v.x(), v.y(), v.z(), 33);
         nParticle->ID = pCount;
         nParticle->bodyID = objectCount;

         nParticle->collisionGradLen = normals[i].length();

//         if(nParticle->collisionGradLen <= 0.01)
//             nParticle->collisionGradLen += 0.01;
//         nParticle->collisionGradLen *= 1.5;

//         mlog<<"p "<<i<<nParticle->collisionGradLen;

         nParticle->collisionVector = (normals[i]).normalized();
         nParticle->collisionVector = (normals[i] * 1000000).normalized();

//         nParticle->collisionVector = QVector3D(0,1,0);

//         qDebug()<<"collision normal : "<<pCount<< " = "<< nParticle->collisionVector <<nParticle->collisionGradLen;
//         nParticle->setMass(0.1);

         m_Particles.push_back(nParticle);
         nRBG->addParticle(v, nParticle);

         std::shared_ptr<SingleParticle> pDynamicObject = std::make_shared<SingleParticle>(nParticle);
         auto sO = m_scene->addSceneObjectFromParticle(pDynamicObject, nParticle, _color);
         sO->isHidden(true);
         i++;
    }

    auto smCstr = nRBG->createConstraint();
    for(auto pt : nRBG->getParticles())
    {
        if(ParticlePtr p = pt.lock())
        {
            QVector3D tmp = p->x;
            QVector4D pos = QVector4D(p->x.x(), p->x.y(), p->x.z(), 1);
            pos = _sceneObject->getMatrix() * pos;
            p->x = QVector3D(pos.x(), pos.y(), pos.z());
            p->p = QVector3D(pos.x(), pos.y(), pos.z());
        }
    }
    m_Constraints.push_back(smCstr);
    m_DynamicObjects.push_back(nRBG);

    _sceneObject->makeDynamic(nRBG);
//    _sceneObject->isHidden(true);
    smCstr->project();
    objectCount++;
}

DynamicObjectPtr DynamicsWorld::addDynamicObjectAsNonUniformParticle(pSceneOb _sceneObject, float radius)
{
    pCount++;
    auto nParticle = std::make_shared<Particle>(_sceneObject->getPos().x(), _sceneObject->getPos().y(), _sceneObject->getPos().z(), 33);
    nParticle->setRadius(radius);
    nParticle->setMass(0);
    nParticle->p = _sceneObject->getPos();
    nParticle->x = _sceneObject->getPos();
    m_NonUniformParticles.push_back(nParticle);
    std::shared_ptr<SingleParticle> pDynamicObject = std::make_shared<SingleParticle>(nParticle);
    _sceneObject->makeDynamic(pDynamicObject);

    nParticle->ID =  991;

    return pDynamicObject;
}


DynamicObjectPtr DynamicsWorld::addDynamicObjectAsSoftBody(pSceneOb _sceneObject, float _mass)
{
    if(!_sceneObject->model())
        return nullptr;

     auto nSB = std::make_shared<SoftBody>(_sceneObject->model());
     objectCount++;
     ModelPtr model = nSB->getModel();
      _sceneObject->setModel(nSB->getModel());

     for(unsigned int i = 0; i < model->getNumShapes(); i++)
     {
         ShapePtr shape = model->getShape(i);
         for(auto point : shape->getPoints())
         {
             QVector3D pos = _sceneObject->getMatrix() * point;
             pCount++;
             auto nParticle = std::make_shared<Particle>(pos.x(), pos.y(), pos.z(), 33);
             nParticle->ID = pCount;
             nParticle->bodyID = objectCount;

             m_Particles.push_back(nParticle);
             nSB->addParticle(point, nParticle);

             std::shared_ptr<SingleParticle> pDynamicObject = std::make_shared<SingleParticle>(nParticle);
             auto sO = m_scene->addSceneObjectFromParticle(pDynamicObject, nParticle);
             sO->isHidden(true);
         }
     }
     std::vector< std::set<int> > constraintIdxs  = nSB->createConstraintNetwork();

     for(auto set : constraintIdxs)
     {
         if(set.size() < 2)
             continue;

         std::set<int>::iterator itA, itB;
         itA = set.begin();
         itB = set.begin();
         std::advance(itB, 1);

         int A = *itA;
         int B = *itB;

         ParticlePtr p1 = nSB->getParticles()[A].lock();
         ParticlePtr p2 = nSB->getParticles()[B].lock();

         std::shared_ptr<DistanceEqualityConstraint> nCstrPtr = addDistanceEqualityConstraint(p1, p2);
     };

     m_DynamicObjects.push_back(nSB);
     nSB->turnOffSelfCollision();
     nSB->updateModelBuffers();
     _sceneObject->makeDynamic(nSB);
     return nSB;
}

void DynamicsWorld::addRope(const QVector3D &_start, const QVector3D &_end, int _numParticles)
{
        QVector3D line = _end - _start;
        QVector3D n  = line.normalized();
        float length = line.length();
        float step = length / _numParticles;
        objectCount++;
        ParticlePtr prevP = nullptr;

        for(int i=0; i <= _numParticles; i++)
        {
            QVector3D pos = _start + (i * step * n);
            pCount++;
            auto nParticle = std::make_shared<Particle>(pos.x(), pos.y(), pos.z(), 33);
            nParticle->ID = pCount;
            nParticle->bodyID = objectCount;
            m_Particles.push_back(nParticle);

            std::shared_ptr<SingleParticle> pDynamicObject = std::make_shared<SingleParticle>(nParticle);
            auto sO = m_scene->addSceneObjectFromParticle(pDynamicObject, nParticle);
            sO->isHidden(true);

            if(!prevP)
            {
                prevP = nParticle;
                continue;
            }
            addDistanceEqualityConstraint(prevP, nParticle);
            prevP = nParticle;
        }
}


ParticlePtr DynamicsWorld::addParticle(float _x, float _y, float _z)
{
    pCount++;
    auto pParticle  = std::make_shared<Particle>(_x, _y, _z, pCount);
    m_Particles.push_back(pParticle);
    return pParticle;
}

void DynamicsWorld::addPlane(const Plane &_plane)
{
    m_Planes.push_back(_plane);
}

void DynamicsWorld::collisionCheckAll()
{
    m_hashGrid.clear();

    for( ParticlePtr p : m_Particles)
    {
        collisionCheck( p);
    }
}

void DynamicsWorld::collisionCheck(ParticlePtr p)
{
        int3 pCell = m_hashGrid.pointToCell(
                    p->position().x(),
                    p->position().y(),
                    p->position().z() );

        size_t pHash = m_hashGrid.hashFunction(pCell);
           p->setHash(pHash);
        m_hashGrid.insert(pHash, p);

        int level = 1;
        for(int y = -1 ; y <= level ; y++)
        {
            for(int x = -1 ; x <= level ; x++)
            {
                for(int z = -1 ; z <= level ; z++)
                {
                    int3 nCell;
                    nCell.i = pCell.i + x;
                    nCell.j = pCell.j + y;
                    nCell.k = pCell.k + z;
                    size_t hash = m_hashGrid.hashFunction(nCell);
                    if(m_hashGrid.cellExists(hash))
                    {
                        std::unordered_map< size_t , std::list< ParticlePtr >>::iterator test = m_hashGrid.m_buckets.find(hash);
                        for(auto np : test->second)
                        {
                            if(p->bodyID != np->bodyID)
                                checkSphereSphere(p,np);
                        }
                    }
                }
            }
        }
        for(auto plane : m_Planes)
        {
            checkSpherePlane(p, plane);
        }

        for(auto nonUParticle  : m_NonUniformParticles)
        {
            checkSphereSphere(p, nonUParticle);
        }
}

void DynamicsWorld::checkSphereSphere(const ParticlePtr p1, const ParticlePtr p2)
{
    float d;
    if(m_CollisionDetect.checkSphereSphere(p1->p, p2->p, d, p1->radius(), p2->radius())){
        addParticleParticleConstraint(p1, p2);
        addFrictionConstraint(p1, p2);
    }

    float d2;
    if(m_CollisionDetect.checkSphereSphere(p1->x, p2->x, d2,p1->radius(), p2->radius())){
        addParticleParticlePreConditionConstraint(p1, p2);
    }
}

void DynamicsWorld::generateData()
{

}

int DynamicsWorld::frameCount()
{
    return m_frameCount;
}

QVector3D* DynamicsWorld::debugDrawLineData()
{
    return nullptr;
}

void DynamicsWorld::checkSpherePlane(const ParticlePtr p1, const Plane &_plane)
{
    float dist = m_CollisionDetect.distanceFromPointToPlane(p1->p, _plane.Normal, (_plane.Offset + (p1->r *_plane.Normal)));
    if(dist > 0)
        return;

    QVector3D qc = m_CollisionDetect.intersectRayPlane(p1->x, p1->p, _plane.Normal, (_plane.Offset + (p1->r *_plane.Normal)));

    if((p1->x - p1->p).length() < 0.0001)
        return;

    if(isnan(qc.x()))
        return;

    auto hsCstr = std::make_shared<HalfSpaceConstraint>(p1, qc, _plane.Normal);
    p1->m_CollisionConstraints.push_back(hsCstr);
    addHalfSpaceFrictionConstraint(p1, _plane.Offset, _plane.Normal);
    addHalfSpacePreConditionConstraint(p1, qc, _plane.Normal);

}

std::shared_ptr<DistanceEqualityConstraint> DynamicsWorld::addDistanceEqualityConstraint(const ParticlePtr _p1, const ParticlePtr _p2)
{
    auto nSpring = std::make_shared<DistanceEqualityConstraint>(_p1, _p2);
    float d = (_p1->x - _p2->x).length();
    nSpring->setRestLength(d);
    m_Constraints.push_back(nSpring);
    _p1->m_Constraints.push_back(nSpring);
    _p2->m_Constraints.push_back(nSpring);

    m_debugLines.push_back(&_p1->x);
    m_debugLines.push_back(&_p2->x);

    return nSpring;
}

std::shared_ptr<PinTogetherConstraint> DynamicsWorld::addPinTogetherConstraint(std::vector<ParticlePtr> &_vec)
{
    std::shared_ptr<PinTogetherConstraint> ptCstr = std::make_shared<PinTogetherConstraint>(_vec);
    for(auto p : _vec)
    {
        p->m_Constraints.push_back(ptCstr);
    }
    m_Constraints.push_back(ptCstr);
    return  ptCstr;
}

void DynamicsWorld::addParticleParticleConstraint(const ParticlePtr _p1, const ParticlePtr _p2)
{
    auto ppCstr = std::make_shared<ParticleParticleConstraint>(_p1, _p2);
    _p1->m_CollisionConstraints.push_back(ppCstr);
    _p2->m_CollisionConstraints.push_back(ppCstr);
}

void DynamicsWorld::addParticleParticlePreConditionConstraint(const ParticlePtr _p1, const ParticlePtr _p2)
{
    auto ppCstr = std::make_shared<ParticleParticlePreConditionConstraint>(_p1, _p2);
    _p1->m_PreConditionConstraints.push_back(ppCstr);
    _p2->m_PreConditionConstraints.push_back(ppCstr);
}

void DynamicsWorld::addFrictionConstraint(const ParticlePtr _p1, const ParticlePtr _p2)
{
    auto fCstr = std::make_shared<FrictionConstraint>(_p1, _p2);
    _p1->m_CollisionConstraints.push_back(fCstr);
    _p2->m_CollisionConstraints.push_back(fCstr);

}

void DynamicsWorld::addHalfSpaceFrictionConstraint(const ParticlePtr _p1, const QVector3D _o, const QVector3D _n)
{
    auto fCstr = std::make_shared<HalfSpaceFrictionConstraint>(_p1, QVector3D(0,0,0), QVector3D(0,1,0));
    _p1->m_CollisionConstraints.push_back(fCstr);
}

void DynamicsWorld::addHalfSpacePreConditionConstraint(const ParticlePtr _p1, const QVector3D _qc, const QVector3D _planeNormal)
{
    auto HsPreCCstr = std::make_shared<HalfSpacePreConditionConstraint>(_p1, _qc, _planeNormal);
    _p1->m_PreConditionConstraints.push_back(HsPreCCstr);
}

void DynamicsWorld::deleteConstraint(const ConstraintPtr _constraint)
{
    for(auto p : _constraint->m_Particles)
    {
        if(auto particle = p.lock())
        {
            std::vector< ConstraintWeakPtr>::const_iterator it = particle->m_Constraints.begin();
            std::vector< ConstraintWeakPtr>::const_iterator itEnd = particle->m_Constraints.end();
            for(it; it != itEnd; ++it)
            {
                if(it->lock() == _constraint)
                {
                    particle->m_Constraints.erase(it);
                }
            }
        }
    }
    m_Constraints.erase(
                std::remove_if(
                    m_Constraints.begin(),
                    m_Constraints.end(),
                    [&](const ConstraintPtr c){return c == _constraint;}),
                m_Constraints.end()
                );

}


Eigen::Matrix3f QMatrix3toEigen(const QMatrix3x3 &_qMat)
{
    Eigen::Matrix3f mat;
    mat(0,0) = _qMat(0,0); mat(0,1) = _qMat(0,1); mat(0,2) = _qMat(0,2);
    mat(1,0) = _qMat(1,0); mat(1,1) = _qMat(1,1); mat(1,2) = _qMat(1,2);
    mat(2,0) = _qMat(2,0); mat(2,1) = _qMat(2,1); mat(2,2) = _qMat(2,2);
    return mat;
}

QMatrix3x3 EigenMatrix3toQt(const Eigen::Matrix3f &_eMat)
{
    QMatrix3x3 mat;
    mat(0,0) = _eMat(0,0); mat(0,1) = _eMat(0,1); mat(0,2) = _eMat(0,2);
    mat(1,0) = _eMat(1,0); mat(1,1) = _eMat(1,1); mat(1,2) = _eMat(1,2);
    mat(2,0) = _eMat(2,0); mat(2,1) = _eMat(2,1); mat(2,2) = _eMat(2,2);
    return mat;
}












