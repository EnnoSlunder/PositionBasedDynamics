#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include  <QDebug>
#include <QVector3D>
#include <eigen3/Eigen/Dense>

#include "dynamics/abstractconstraint.h"
#include "dynamics/particle.h"
#include "dynamics/dynamicUtils.h"


typedef double Real;
using Matrix3r = Eigen::Matrix3f;
using Vector3r = Eigen::Matrix<float, 3, 1>;

const double eps = 1e-6;

class HalfSpaceConstraint : public AbstractConstraint
{
public:
    HalfSpaceConstraint(const QVector3D &_p, const QVector3D &_qc, const QVector3D &_n);
    HalfSpaceConstraint(const ParticlePtr _p, const QVector3D &_qc, const QVector3D &_n);

    void project();
    float constraintFunction();
    float constraintFunction(const QVector3D &_p);
    QVector3D deltaP();

private:
    float d;
    ParticlePtr pptr;
    QVector3D n, p, dp, qc;
};

class HalfSpacePreConditionConstraint : public AbstractConstraint
{
public:
    HalfSpacePreConditionConstraint(const ParticlePtr _p, const QVector3D &_qc, const QVector3D &_n);
    void project();
    float constraintFunction();
private:
    float d;
    ParticlePtr pptr;
    QVector3D n, p, dp, qc;
};

class PinConstraint : public AbstractConstraint
{
public:
    PinConstraint(const ParticlePtr _particle, const QVector3D &_pos );
    void project();
    float constraintFunction();
    void setPositon(const QVector3D &_pos);
    QVector3D deltaP();
private:
    QVector3D pinPosition;
    ParticlePtr particle;
};

class PinTogetherConstraint : public AbstractConstraint
{
public:
    PinTogetherConstraint( std::vector<ParticlePtr> &_particleVec);
    void project();
    float constraintFunction();

private:
    std::vector< ParticlePtr>       m_particles;
    QVector3D m_avrgPos;
};

class ParticleParticleConstraint : public AbstractConstraint
{
public:
    ParticleParticleConstraint(const ParticlePtr _p1, const ParticlePtr _p2);
    ParticleParticleConstraint(const ParticlePtr _p1, const ParticlePtr _p2, float _d);
    void project();
    float constraintFunction();
    QVector3D deltaP();
    QVector3D getSDFcollisionVector(QVector3D &_vec);

private:
    int useSDFCollision = false;
    int id;
    float d;
    ParticlePtr pptr1, pptr2;
};

class ParticleParticlePreConditionConstraint : public AbstractConstraint
{
public:
    ParticleParticlePreConditionConstraint(const ParticlePtr _p1, const ParticlePtr _p2);
    void project();
    float constraintFunction();
    QVector3D deltaP();
    QVector3D getSDFcollisionVector(QVector3D &_vec);
private:
    int useSDFCollision = false;
    int id;
    float d;
    ParticlePtr pptr1, pptr2;
};

class DistanceEqualityConstraint : public AbstractConstraint
{
public:
    DistanceEqualityConstraint(const ParticlePtr _p1, const ParticlePtr _p2 );

    float constraintFunction();
    QVector3D deltaP();
    void project();

    void setRestLength(float _d);
    float getRestLength();

private:
    float d, springLength;
    QVector3D Vp1, Vp2, springDir;
    ParticlePtr pptr1, pptr2;
    int stretchK, compressK;
};


class ShapeMatchingConstraint : public AbstractConstraint
{
public:
    ShapeMatchingConstraint();
    ShapeMatchingConstraint(RigidBody *_rigidbody);
    ShapeMatchingConstraint(RigidBodyGrid *_rigidbody);
    void project();
    float constraintFunction();
    void preCompute(int numParticles, std::vector<ParticleWeakPtr> &_particles, std::vector<QVector3D> &_restShape);

private:
    std::vector< ParticlePtr>       m_particles;
    std::vector< Eigen::Vector3f>   m_restPositions;

    Eigen::Vector3f cm, cmOrigin;

    RigidBody *m_rb;
    RigidBodyGrid *m_rbg;
    Eigen::Matrix3f Apq, Aqq, R;

    Eigen::Quaternionf q, qPrev;
};

class FrictionConstraint : public AbstractConstraint
{
public:
    FrictionConstraint(){}
    FrictionConstraint(const ParticlePtr _p1, const ParticlePtr _p2);

    void project();
    float constraintFunction();
private:
    ParticlePtr pptr1, pptr2;
    QVector3D m_collisionNormal;
};

class HalfSpaceFrictionConstraint : public AbstractConstraint
{
public:
    HalfSpaceFrictionConstraint(){}
    HalfSpaceFrictionConstraint(const ParticlePtr _p1, const QVector3D _o, const QVector3D _n);

    void project();
    float constraintFunction();
private:
    ParticlePtr pptr1, pptr2;
    QVector3D m_collisionNormal, planeOrigin;

};

void polarDecompositionStable(const Matrix3r &M, const double tolerance, Matrix3r &R);

double oneNorm(const Matrix3r &A);

double infNorm(const Matrix3r &A);



#endif // CONSTRAINT_H






















