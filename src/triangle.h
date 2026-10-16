#ifndef TRIANGLE
#define TRIANGLE

#include "vector3.h"
#include "colour.h"

//Typedefs
struct Triangle{
    vector3 point_1;
    vector3 point_2;
    vector3 point_3;
    colour c;
};
typedef struct Triangle triangle;

//Prototypes
triangle t_create(vector3 point_1, vector3 point_2, vector3 point_3);
bool t_intersects(triangle t, vector3 v);
double t_intersection_distance(triangle t, vector3 v);
vector3 t_intersection_normal(triangle t, vector3 v);

#endif