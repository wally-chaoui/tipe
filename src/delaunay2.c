#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "delaunay2.h"
#include "vector3.h"
#include "edge.h"
#include "triangle.h"

#define BIG_ARRAY 99999

triangle d2_extract_dual_face(edge* e) {
    edge* e1 = e_rot(e);
    edge* e2 = e_rot(e_onext(e));
    edge* e3 = e_rot(e_onext(e_onext(e)));
    triangle t = t_create(e1->origin, e2->origin, e3->origin);
    //t.c = (colour){.r = 255, .g = 255, .b = 255, .brightness = 1};
    return t;
}

triangle d2_extract_left_face(edge* e) {
    edge* dual_edge = e_anti_rot(e);
    return d2_extract_dual_face(dual_edge);
}

double d2_triangle_area(vector3 a, vector3 b, vector3 c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

//Predicate: a, b, c in ccw order!
bool d2_in_circle(vector3 a, vector3 b, vector3 c, vector3 p) {
    return (a.x * a.x + a.y * a.y) * d2_triangle_area(b, c, p) -
    (b.x * b.x + b.y * b.y) * d2_triangle_area(a, c, p) +
    (c.x * c.x + c.y * c.y) * d2_triangle_area(a, b, p) -
    (p.x * p.x + p.y * p.y) * d2_triangle_area(a, b, c) > 0;
}

bool d2_ccw(vector3 a, vector3 b, vector3 c) {
    return d2_triangle_area(a, b, c) > 0;
}

bool d2_left_of(vector3 p, edge* e) {
    return d2_ccw(p, e->origin, e_sym(e)->origin);
}

edge* d2_locate(vector3 p, edge* e0) {
    edge* e = e0;
    while (true) {
        if (!d2_left_of(p, e)) {
            e = e_sym(e);
        }
        else if (d2_left_of(p, e_onext(e))) {
            e = e_onext(e);
        }
        else if (d2_left_of(p, e_anti_dnext(e))) {
            e = e_anti_dnext(e);
        }
        else {
            return e;
        }
    }
}

void d2_insert(vector3 p, edge* e0) {
    edge* e = d2_locate(p, e0);
    edge* base = e_make_edge();
    e_set_endpoints(base, e->origin, p);
    e_splice(base, e);
    edge* start = base;
    while (e_lnext(e) != start) {
        base = e_connect(e, e_sym(base));
        e = e_anti_onext(base);
    }
    while (true) {
        edge* t = e_anti_onext(e);
        if (!d2_left_of(e_sym(t)->origin, e) 
        && d2_in_circle(e->origin, e_sym(t)->origin, e_sym(e)->origin, p)) {
            e_swap(e);
            e = e_anti_onext(e);
        }
        else if (e_onext(e) == start) {
            return;
        }
        else {
            e = e_anti_lnext(e_onext(e));
        }
    }
}

bool d2_seen_node(edge* e) {
    edge* start = e;
    if (e->name == -1) {
        return true;
    }
    while (e_onext(e) != start) {
        e = e_onext(e);
        if (e->name == -1) {
            return true;
        }
    }
    return false;
}

int d2_extract_triangles(triangle* triangles, edge* outer_edge) {
    int n_triangles = 0;
    edge** pile = (edge**)malloc(sizeof(edge*) * BIG_ARRAY);
    int n_pile = 0;
    pile[n_pile] = outer_edge;
    n_pile = n_pile + 1;
    while (n_pile > 0 && n_pile < BIG_ARRAY) {
        edge* e = pile[n_pile - 1];
        n_pile = n_pile - 1;
        if (e->name != -1) {
            edge* next = e_sym(e);
            edge* start = next;
            while (e_onext(next) != start) {
                next = e_onext(next);
                pile[n_pile] = next;
                n_pile = n_pile + 1;
            }
        }
        if (!d2_seen_node(e)) {
            triangle t = d2_extract_dual_face(e);
            triangles[n_triangles] = t;
            n_triangles = n_triangles + 1;
        }
        e->name = -1;
    }
    return n_triangles;
}

edge* d2_bounding_triangle() {
    //Bounding edges
    edge* eAB = e_make_edge();
    e_set_endpoints(eAB, v_create(0, 99999, -5000), v_create(0, -99999, -5000));
    e_rename(eAB, 100);
    edge* eBC = e_make_edge();
    e_set_endpoints(eBC, v_create(0, -99999, -5000), v_create(99999, 0, -5000));
    e_rename(eBC, 200);
    edge* eCA = e_make_edge();
    e_set_endpoints(eCA, v_create(99999, 0, -5000), v_create(0, 99999, -5000));
    e_rename(eCA, 300);

    //Manually connecting edges
    eAB->onext = eCA->rot->rot;
    eBC->onext = eAB->rot->rot;
    eCA->onext = eBC->rot->rot;
    eAB->rot->rot->onext = eBC;
    eBC->rot->rot->onext = eCA;
    eCA->rot->rot->onext = eAB;
    
    //Manually connecting dual edges
    eAB->rot->onext = eCA->rot;
    eBC->rot->onext = eAB->rot;
    eCA->rot->onext = eBC->rot;
    eAB->rot->rot->rot->onext = eBC->rot->rot->rot;
    eBC->rot->rot->rot->onext = eCA->rot->rot->rot;
    eCA->rot->rot->rot->onext = eAB->rot->rot->rot;

    //Boundaries:
    //      9999          -1000
    //       C
    //
    // 9999       -9999  -1000
    //  A    0      B

    //Define arbitrary e0
    edge* e0 = eAB;

    return e0;
}