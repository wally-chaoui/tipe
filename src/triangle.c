#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "triangle.h"
#include "vector3.h"
#include "colour.h"

//COLOUR SELECT TODO
triangle t_create(vector3 point_1, vector3 point_2, vector3 point_3) {
    double avg_z = (point_1.z + point_2.z + point_3.z) / 3.0;
    colour c = c_create_white();
    triangle t = {.point_1 = point_1, .point_2 = point_2, .point_3 = point_3, .c = c};
    return t;
}

bool t_intersects(triangle t, vector3 v) {
    //Get n, direction of the normal vector of the plane created by t, facing away from the origin
    vector3 u_1 = v_sub(t.point_2, t.point_1);
    vector3 u_2 = v_sub(t.point_3, t.point_1);
    vector3 n_1 = v_cross(u_1, u_2);
    vector3 n_2 = v_normalise(n_1);
    vector3 n;
    if (v_dot(n_2, t.point_1) > 0) {
        n = n_2;
    }
    else {
        n = v_mul(n_2, -1);
    }
    //Get d, closest point of the plane from the origin (or 'distance')
    double d_param = v_dot(t.point_1, n);
    vector3 d = v_mul(n, d_param);
    //Verify if the plane is in front of the ray or not
    if (v_dot(v, n) < 0.001) {
        return false;
    }
    //Calculate p_param
    double a = v_dot(d, n);
    double b = v_dot(v, n);
    double p_param = a / b;
    vector3 p = v_mul(v, p_param);
    //Verify if p is in the triangle
    vector3 u_12 = v_sub(t.point_2, t.point_1);
    vector3 u_23 = v_sub(t.point_3, t.point_2);
    vector3 u_31 = v_sub(t.point_1, t.point_3);
    vector3 p_1 = v_sub(p, t.point_1);
    vector3 p_2 = v_sub(p, t.point_2);
    vector3 p_3 = v_sub(p, t.point_3);
    double k_1 = v_dot(n, v_cross(u_12, p_1));
    double k_2 = v_dot(n, v_cross(u_23, p_2));
    double k_3 = v_dot(n, v_cross(u_31, p_3));
    return ((k_1 > 0) && (k_2 > 0) && (k_3 > 0)) || ((k_1 < 0) && (k_2 < 0) && (k_3 < 0));
}

double t_intersection_distance(triangle t, vector3 v) {
    //Get n, direction of the normal vector of the plane created by t, facing away from the origin
    vector3 u_1 = v_sub(t.point_2, t.point_1);
    vector3 u_2 = v_sub(t.point_3, t.point_1);
    vector3 n_1 = v_cross(u_1, u_2);
    vector3 n_2 = v_normalise(n_1);
    vector3 n;
    if (v_dot(n_2, t.point_1) > 0) {
        n = n_2;
    }
    else {
        n = v_mul(n_2, -1);
    }
    //Get d, closest point of the plane from the origin (or 'distance')
    double d_param = v_dot(t.point_1, n);
    vector3 d = v_mul(n, d_param);
    //Get p_param, the scalar that verifies p = p_param . v
    double a = v_dot(d, n);
    double b = v_dot(v, n);
    double p_param = a / b;
    return p_param;
}

vector3 t_intersection_normal(triangle t, vector3 v) {
    //Triangle normal is actually independant from v
    //However, can be of both sides...
    //We want the normal facing opposite of v
    vector3 e1 = v_sub(t.point_2, t.point_1);
    vector3 e2 = v_sub(t.point_3, t.point_1);
    vector3 n1 = v_normalise(v_cross(e1, e2));
    if (v_dot(n1, v) < 0) {
        return n1;
    }
    else {
        return v_mul(n1, -1);
    }
}
