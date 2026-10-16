#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "delaunay3.h"
#include "vector3.h"
#include "half.h"
#include "triangle.h"

half* d3_create_tetrahedron(vector3 a, vector3 b, vector3 c, vector3 d) {
    vector3 i = v_create(0.0, 0.0, 0.0);
    vector3 e = v_create(999, 999, 999);
    
    half* e1 = h_mevvfs(a, b, i, e);
    half* e2 = h_mev(c, h_sym(e1));
    half* e3 = h_mef(h_sym(e2), e1);

    half* e4 = h_mev(d, e1);
    half* e5 = h_mef(h_sym(e4), e3);
    half* e6 = h_mef(h_sym(e4), e2);

    return e1;
}

half* d3_cube() {
    vector3 p1 = v_create(1, -1, -1);
    vector3 p2 = v_create(1, 1, -1);
    vector3 p3 = v_create(1, 1, 1);
    vector3 p4 = v_create(1, -1, 1);
    vector3 p5 = v_create(0, -1, -1);
    vector3 p6 = v_create(0, 1, -1);
    vector3 p7 = v_create(0, 1, 1);
    vector3 p8 = v_create(0, -1, 1);
    vector3 o = v_create(0.0, 0.0, 0.-1);

    half* e1 = h_mevvfs(p1, p2, o, o);
    half* e2 = h_mev(p3, h_sym(e1));
    half* e3 = h_mev(p4, h_sym(e2));
    half* e4 = h_mef(h_sym(e3), e1);
    h_print(e1);
    h_print(e1->fnext);
    h_print(e1->fnext->fnext);
    h_print(e1->fnext->fnext->fnext);
    
    half* e5 = h_mev(p5, h_onext(e1));
    half* e6 = h_mev(p6, h_onext(e2));
    half* e7 = h_mev(p7, h_onext(e3));
    half* e8 = h_mev(p8, h_onext(e4));

    half* e9 = h_mef(h_sym(e6), h_sym(e5));
    half* e10 = h_mef(h_sym(e7), e9);
    half* e11 = h_mef(h_sym(e8), e10);
    half* e12 = h_mef(h_sym(e5), e11);

    return e1;
}

half* d3_test() {
    vector3 a = v_create(500, 100, -500);
    vector3 b = v_create(500, -100, -500);
    vector3 c = v_create(1000, 100, -500);
    vector3 d = v_create(750, 0, -300);
    //return d3_create_tetrahedron(a, b, c, d);
    half* e0 = d3_create_tetrahedron(a, b, c, d);

    vector3 q = v_create(600, 200, -500);
    half* e1 = d3_create_tetrahedron(q, a, c, d);
    //e0: a->b
    //e1: q->a
    //We want to join faces repr by c->a on the right and a->c on the left
    half* e00 = h_sym(h_fnext(h_sym(e0)));
    half* e11 = h_sym(h_fnext(h_fnext(h_sym(e1))));
    h_join_by_face(e00, e11);

    printf("Tests\n");
    h_print(e00);
    h_print(e11);
    h_print(h_fnext(h_dual(e00)));

    return e0;
}

half* d3_bounding_tetrahedron() {
    vector3 a = v_create(0, 9999, -5000);
    vector3 b = v_create(0, -9999, -5000);
    vector3 c = v_create(9999, 0, -5000);
    vector3 d = v_create(0, 0, 9999);
    return d3_create_tetrahedron(a, b, c, d);
}

triangle d3_extract_dual_face(half* h) {
    half* h1 = h_dual(h);
    half* h2 = h_fnext(h1);
    half* h3 = h_fnext(h2);
    vector3 a = h1->origin;
    vector3 b = h2->origin;
    vector3 c = h3->origin;
    return t_create(a, b, c);
}

triangle d3_extract_face(half* h) {
    half* h1 = h;
    half* h2 = h_fnext(h1);
    half* h3 = h_fnext(h2);
    h_print(h1);
    vector3 a = h1->origin;
    vector3 b = h2->origin;
    vector3 c = h3->origin;
    triangle t = t_create(a, b, c);
    printf("%lf\n", t.point_1.x);
    return t_create(a, b, c);
}

int d3_extract_triangles(triangle* triangles, half* d0) {
    int n_triangles = 0;
    triangles[0] = d3_extract_face(h_dual(d0));
    triangles[1] = d3_extract_face(h_sym(h_dual(d0)));
    triangles[2] = d3_extract_face(h_sym(h_fnext(h_dual(d0))));
    triangles[3] = d3_extract_face(h_sym(h_fnext(h_fnext(h_dual(d0)))));
    return 4;
}

void d3_insert(vector3 p, half* d0) {
    half* e0 = h_dual(d0);
    vector3 a = e0->origin;
    vector3 b = h_fnext(e0)->origin;
    vector3 c = h_sym(h_onext(h_onext(e0)))->origin;
    vector3 d = h_fnext(h_fnext(e0))->origin;
    v_print(a);
    v_print(b);
    v_print(c);
    v_print(d);
    half* e_base = d3_create_tetrahedron(a, b, c, p);
    half* e_front = d3_create_tetrahedron(a, b, p, d);
    half* e_left = d3_create_tetrahedron(c, a, p, d);
    half* e_right = d3_bounding_tetrahedron(b, c, p, d);
}
