#include "include/dynamics/collisiondetection.h"

#include <Qdebug>

#include <math.h>

#include "Scene.h"

CollisionDetection::CollisionDetection()
{
}

double CollisionDetection::distanceFromPointToPlane(const Vec3 &_p, const Vec3 &_n, const Vec3 &_o)
{
    Vec3 temp = (_p - _o);
    double fDist (Vec3::dotProduct(temp, _n));
    return fDist;
}

Vec3 CollisionDetection::intersectRayPlane(const Vec3 &_r1, const Vec3 &_r2, const Vec3 &_n, const Vec3 &_o)
{
    Vec3 ray, planehit;
    float devisor, t;
    ray = (_r2 -_r1).normalized();
    devisor = Vec3::dotProduct(ray, _n);
    t = Vec3::dotProduct((_o -_r1), _n) / devisor;
    planehit = _r1 + (t) * ray;
    return planehit;
}

bool CollisionDetection::checkRaySphere(const Vec3 &_o, const Vec3 &_d, const Vec3 &_p, float r)
{
    Vec3 L = _o - _p;
    float a = Vec3::dotProduct(_d, _d);
    float b = 2 * Vec3::dotProduct(_d,L);
    float c = Vec3::dotProduct(L,L) - pow(r,2);

    float t0, t1;
    if (!solveQuadratic(a, b, c, t0, t1)) return false;

    if (t0 > t1) std::swap(t0, t1);

            if (t0 < 0) {
                t0 = t1; // if t0 is negative, let's use t1 instead
                if (t0 < 0) return false; // both t0 and t1 are negative
            }

            float t = t0;
    return true;
}

float CollisionDetection::distancaneFromIntersectionRayToSphere(const Vec3 &_o, const Vec3 &_d, const Vec3 &_p, float r)
{
    // from: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
    Vec3 L = _o - _p;
    float a = Vec3::dotProduct(_d, _d);
    float b = 2 * Vec3::dotProduct(_d,L);
    float c = Vec3::dotProduct(L,L) - pow(r,2);

    float t0, t1;
    if (!solveQuadratic(a, b, c, t0, t1)) return false;

    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0) return 0.0; // both t0 and t1 are negative
    }

    float t = t0;
    return t;
}

bool CollisionDetection::solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = - 0.5 * b / a;
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);

    return true;
}

bool CollisionDetection::intersectRaySphere(const Vec3 &_o, const Vec3 &_dir, const Vec3 &_center, Vec3 &point, const float _radius, float &t)
{
    // from: RealTime CollisionDetecton book [5.21]
    //  Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
    //  returns t value of intersection and intersection point q
    Vec3 m = _o - _center;
    float b = Vec3::dotProduct(m, _dir);
    float c = Vec3::dotProduct(m,m) - _radius * _radius;
    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if(c > 0.0f && b > 0.0f)
        return false;
    float discr = b * b -c;
    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f) return false;
    // Ray now found to intersect sphere, compute smallest t value of intersection
    t = -b - std::sqrt(discr);
    // If t is negative, ray started inside sphere so clamp t to zero
    if (t < 0.0f) t = 0.0f;

    point = _o + t * _dir;
    return true;

}

Vec3 CollisionDetection::intersectRayPlane(const Vec3 &_n, const Vec3 &_pO, const Ray &_ray)
{
    return intersectRayPlaneRec(_n, _pO, _ray);
}

Vec3 CollisionDetection::intersectRayPlaneRec(const Vec3 &_n, const Vec3 &_pO, const Ray &_ray)
{
    float denom = Vec3::dotProduct(_n, _ray.Dir);

    Vec3 pOlO = _pO - _ray.Origin;
    float t = Vec3::dotProduct(pOlO, _n) / denom;

    QVector3D result =  _ray.Origin + (_ray.Dir.normalized() * t);

    if(t > 0.001)
    {
        Ray _ray2;
        _ray2.Dir = _ray.Dir.normalized();
        _ray2.Origin = result;
        result = intersectRayPlaneRec(_n, _pO, _ray2);
    }
    return result;

}

Vec3 CollisionDetection::intersectLinePlane(const Vec3 &_n, const Vec3 &_pO, const Vec3 &_A, const Vec3 &_B)
{
    Vec3 AB = _B -_A;
    // this wont work anymore. Update PointPlane check to non abs() distance (goes < 0 now)
    double d = (distanceFromPointToPlane(QVector3D(0,0,0), _n, _pO));
    double t = (d - Vec3::dotProduct(_n, _A)) / Vec3::dotProduct(_n, AB);

    QVector3D _d ;
    if (t >= 0.0f && t <= 1.0f )
    {
        _d = _A + (t * AB);
        return _A + (t * AB);
    }
    else
    {
        _d = _A;
        return _A;
    }
}

Vec3 CollisionDetection::closetPointFromRayToRay(const Ray &_r1, const Ray &_r2)
{
    float p,q,r,s,t,d,e;
    Vec3 a,b,c;
    a = _r1.Dir.normalized();
    b = _r2.Dir.normalized();
    c = _r2.Origin - _r1.Origin;

    p = Vec3::dotProduct(a, b);
    q = Vec3::dotProduct(a, c);
    r = Vec3::dotProduct(b, c);
    s = Vec3::dotProduct(a, a);
    t = Vec3::dotProduct(b, b);

    d = (-p*r + q*t) / (s*t - p*p);
    e = (p*q) / (s*t - p*p) - (r / ( t - ((p*p) / s)));

    Vec3 point1 =  _r1.Origin +  (a * d);
    Vec3 point2 =  _r2.Origin + (b * e);

    Vec3 z = point2 - point1;

    Vec3 result = point1 + z;
    return result;
}

bool CollisionDetection::checkSphereSphere(const Vec3 &_s1, const Vec3 &_s2, float &d,float _r1, float _r2)
{
    float dist = (_s2 - _s1).length();
    float minDist = _r1 + _r2;
    bool isColliding =  (dist < minDist);
    if (isColliding)
        d = minDist - dist;
    float a = (_s2 - _s1).length();
    float b = _r1 + _r2;

    return isColliding;

}




























