#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include <vector>
#include <QMatrix4x4>
#include <QVector3D>

#include "utils.h"
#include "dynamics/dynamicUtils.h"

typedef QVector3D Vec3;

class Scene;

class CollisionDetection
{
public:
    CollisionDetection();
    CollisionDetection(Scene *_scene);

    bool checkRaySphere(const Vec3 &_o, const Vec3 &_d, const Vec3 &_p, float r);
    bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);
    bool checkSphereSphere(const Vec3 &_s1, const Vec3 &_s2, float &d, float _r1, float _r2);

    double distanceFromPointToPlane(const Vec3 &_p, const Vec3 &_n, const Vec3 &_o);
    float distancaneFromIntersectionRayToSphere(const Vec3 &_o, const Vec3 &_d, const Vec3 &_p, float r);

    bool intersectRaySphere(const Vec3 &_o, const Vec3 &_dir, const Vec3 &_center, Vec3 &point, const float _radius, float &t);

    Vec3 intersectRayPlane(const Vec3 &_r1, const Vec3 &_r2, const Vec3 &_n, const Vec3 &_o);
    Vec3 intersectRayPlane(const Vec3 &_n, const Vec3 &_pO, const Ray &_ray);
    Vec3 intersectRayPlaneRec(const Vec3 &_n, const Vec3 &_pO, const Ray &_ray);
    Vec3 intersectLinePlane(const Vec3 &_n, const Vec3 &_pO, const Vec3 &_A, const Vec3 &_B);

    Vec3 closetPointFromRayToRay(const Ray &_r1, const Ray &_r2);

};


#endif // COLLISIONDETECTION_H
