#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "sphere.h"
#include "vector3.h"

sphere s_create(vector3 position, double radius, double reflectance) {
    sphere s = {.position = position, .radius = radius, .reflectance = reflectance};
    return s;
}

bool s_intersects(sphere s, vector3 v) {
    double tca = v_dot(s.position, v); //Scalar needed to reach mediatrice that intersects s.position
    if (tca < 0) {
        return false;
    }
    double d = sqrt(v_dot(s.position, s.position) - tca * tca); //Uses pythagorean theorem
    if (d < 0) {
        return false;
    }
    if (d > s.radius) {
        return false;
    }
    return true;
}

double s_intersection_distance(sphere s, vector3 v) {
    double tca = v_dot(s.position, v); //Scalar needed to reach mediatrice that intersects s.position
    double d = sqrt(v_dot(s.position, s.position) - tca * tca); //Distance between mediatrice and sphere
    double thc = sqrt(s.radius * s.radius - d * d); //Distance between intersection points, halved
    return tca - thc;
}

vector3 s_intersection_normal(sphere s, vector3 v) {
    double tca = v_dot(s.position, v); //Scalar needed to reach mediatrice that intersects s.position
    double d = sqrt(v_dot(s.position, s.position) - tca * tca); //Uses pythagorean theorem
    double thc = sqrt(s.radius * s.radius - d * d); //Distance between intersection points, halved
    vector3 normal = v_sub(v_mul(v, tca - thc), s.position);
    normal = v_normalise(normal);
    return normal;
}
