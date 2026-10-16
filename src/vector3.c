#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "vector3.h"

vector3 v_create(double x, double y, double z) {
    vector3 v = {.x = x, .y = y, .z = z};
    return v;
}

double v_norm(vector3 v) {
    return sqrt(pow(v.x, 2.0) + pow(v.y, 2.0) + pow(v.z, 2.0));
}

/*
void v_normalise(vector3* v) {
    double norm = v_norm(*v);
    v->x = v->x / norm;
    v->y = v->y / norm;
    v->z = v->z / norm;
}
*/

vector3 v_normalise(vector3 v) {
    double norm = v_norm(v);
    vector3 res = {.x = v.x / norm, .y = v.y / norm, .z = v.z / norm};
    return res;
}

vector3 v_rotate(vector3 v, double x_rot, double y_rot, double z_rot) {
    //Only for y rot
    double new_x = v.x * cos(y_rot) - v.z * sin(y_rot);
    double new_y = v.y;
    double new_z = v.x * sin(y_rot) + v.z * cos(y_rot);
    return v_create(new_x, new_y, new_z);
}

vector3 v_add(vector3 v1, vector3 v2) {
    vector3 v_out;
    v_out.x = v1.x + v2.x;
    v_out.y = v1.y + v2.y;
    v_out.z = v1.z + v2.z;
    return v_out;
}

vector3 v_sub(vector3 v1, vector3 v2) {
    vector3 v_out;
    v_out.x = v1.x - v2.x;
    v_out.y = v1.y - v2.y;
    v_out.z = v1.z - v2.z;
    return v_out;
}

vector3 v_mul(vector3 v, double t) {
    vector3 v_out;
    v_out.x = v.x * t;
    v_out.y = v.y * t;
    v_out.z = v.z * t;
    return v_out;
}

double v_dot(vector3 v1, vector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vector3 v_cross(vector3 v1, vector3 v2) {
    vector3 v_out;
    v_out.x = v1.y * v2.z - v1.z * v2.y;
    v_out.y = v1.z * v2.x - v1.x * v2.z;
    v_out.z = v1.x * v2.y - v1.y * v2.x;
    return v_out;
}

void v_print(vector3 v) {
    printf("Vector3: %lf %lf %lf\n", v.x, v.y, v.z);
}

vector3 v_min(vector3* tab, int n_tab) {
    double min_x = tab[0].x;
    double min_y = tab[0].y;
    double min_z = tab[0].z;
    for (int i = 0; i < n_tab; i++) {
        if (tab[i].x < min_x) {
            min_x = tab[i].x;
        }
        if (tab[i].y < min_y) {
            min_y = tab[i].y;
        }
        if (tab[i].z < min_z) {
            min_z = tab[i].z;
        }
    }
    return v_create(min_x, min_y, min_z);
}

vector3 v_max(vector3* tab, int n_tab) {
    double max_x = tab[0].x;
    double max_y = tab[0].y;
    double max_z = tab[0].z;
    for (int i = 0; i < n_tab; i++) {
        if (tab[i].x > max_x) {
            max_x = tab[i].x;
        }
        if (tab[i].y > max_y) {
            max_y = tab[i].y;
        }
        if (tab[i].z > max_z) {
            max_z = tab[i].z;
        }
    }
    return v_create(max_x, max_y, max_z);
}
