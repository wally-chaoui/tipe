#ifndef SPHERE
#define SPHERE

#include "vector3.h"

//Typedefs
struct Sphere{
    vector3 position;
    double radius;
    double reflectance;
};
typedef struct Sphere sphere;

//Prototypes
sphere s_create(vector3 position, double radius, double reflectance);
bool s_intersects(sphere s, vector3 v);
double s_intersection_distance(sphere s, vector3 v);
vector3 s_intersection_normal(sphere s, vector3 v);

#endif