#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "vector3.h"
#include "half.h"

//Navigation

half* h_onext(half* h) {
    return h->onext;
}

half* h_fnext(half* h) {
    return h->fnext;
}

half* h_sym(half* h) {
    return h->sym;
}

half* h_dual(half* h) {
    return h->dual;
}

half* h_enext(half* h) {
    return h_dual(h_fnext(h_dual(h)));
}

half* h_adj(half* h) {
    return h_sym(h_enext(h));
}

half* h_oprev(half* h) {
    //return h_fnext(h_sym(h)); Apparently not - why? TODO
    return h_dual(h_onext(h_dual(h)));
}

half* h_fprev(half* h) {
    //return h_sym(h_onext(h)); Apparently not - why? TODO
    return h_sym(h_dual(h_fnext(h_sym(h_dual(h)))));
}

half* h_eprev(half* h) {
    return h_sym(h_dual(h_fnext(h_dual(h_sym(h)))));
}

//Construction

//Level 0

half* h_make_half(vector3 p, vector3 d) {
    half* h1 = (half*)malloc(sizeof(half));
    half* h2 = (half*)malloc(sizeof(half));

    h1->onext = h1;
    h2->onext = h2;
    
    h1->fnext = h1;
    h2->fnext = h2;
    
    h1->sym = h1;
    h2->sym = h2;
    
    h1->dual = h2;
    h2->dual = h1;
    
    h1->origin = p;
    h2->origin = d;
    
    h1->name = 0;
    h2->name = 0;
    
    return h1;
}

void h_connect(half* h1, half* h2) {
    h1->sym = h2;
    h2->sym = h1;
}

void h_disconnect(half* h) {
    half* h_sym = h->sym;
    h->sym = h;
    h_sym->sym = h_sym;
}

void h_half_splice(half* h1, half* h2) {
    half* h1_onext = h1->onext;
    half* h2_onext = h2->onext;
    h2->onext = h1_onext;
    h1->onext = h2_onext;
    //NF CHANGE IS ALWAYS TRUE
    h1_onext->sym->fnext = h2;
    h2_onext->sym->fnext = h1;
}

//Level 1

int counter = 100;

half* h_make_edge(vector3 p1, vector3 p2, vector3 d) {
    half* h = h_make_half(p1, d);
    half* h_sym = h_make_half(p2, d);
    h_connect(h, h_sym);
    h->fnext = h_sym;
    h_sym->fnext = h;
    h_connect(h->dual, h_sym->dual);
    h->dual->fnext = h_sym->dual;
    h_sym->dual->fnext = h->dual;
    h_rename(h, counter);
    counter = counter + 100;
    return h;
}

void h_snap(half* h1, half* h2) {
    half* h_temp = h1->sym->fnext;
    h1->sym->fnext = h2->sym->fnext;
    h2->sym->fnext = h_temp;

    h_temp = h1->fnext;
    h1->fnext = h2->fnext;
    h2->fnext = h_temp;

    h_temp = h1->sym;
    h_connect(h1, h2->sym);
    h_connect(h2, h_temp);
}

half* h_make_degenerate_edge(vector3 p, vector3 d) {
    half* h = h_make_half(p, d);
    half* h_sym = h_make_half(p, d);
    h_connect(h, h_sym);
    h_half_splice(h, h_sym);
    h_connect(h->dual, h_sym->dual);
    h_half_splice(h->dual, h_sym->dual);
    return h;
}

void h_splice(half* h1, half* h2) {
    h_half_splice(h1->onext->dual, h2->onext->dual);
    h_half_splice(h1, h2);
}

//Level 2

half* h_make_complex_edge(vector3 p1, vector3 p2, vector3 i, vector3 e) {
    half* h_int = h_make_edge(p1, p2, i);
    half* h_ext = h_make_edge(p1, p2, e);
    h_snap(h_int->dual->sym, h_ext->dual);
    return h_int;
}

void h_complex_splice(half* h1, half* h2) {
    half* h1_temp = h1->dual->sym->dual;
    half* h2_temp = h2->dual->sym->dual;
    h_splice(h1, h2);
    h_splice(h1_temp, h2_temp);
    h_snap(h1->dual->sym, h2_temp->dual);
    h_snap(h2->dual->sym, h1_temp->dual);
}

half* h_make_face(vector3 p1, vector3 p2, vector3 p3, vector3 d) {
    half* h1 = h_make_degenerate_edge(p1, d);
    half* h2 = h_make_degenerate_edge(p2, d);
    h_snap(h1->onext, h2);
    half* h3 = h_make_degenerate_edge(p3, d);
    h_snap(h1->onext, h3);
    return h1;
}

void h_sew(half* h1, half* h2_sym) {
    half* h1_sym = h1->sym;
    half* h2 = h2_sym->sym;
    half* h1_oprev = h_oprev(h1);
    half* h2_oprev = h_oprev(h2);
    half* h1_sym_oprev = h_oprev(h1_sym);
    half* h2_sym_oprev = h_oprev(h2_sym);

    h_snap(h1, h2_sym);
    h_splice(h1_oprev, h2_sym_oprev);
    h_splice(h1_sym_oprev, h2_oprev);
}

//Euler operations

half* h_mevvfs(vector3 p1, vector3 p2, vector3 i, vector3 e) {
    return h_make_complex_edge(p1, p2, i, e);
}

half* h_mev(vector3 p, half* h) {
    half* h_new = h_make_complex_edge(h->origin, p, h_dual(h)->origin, h_dual(h_adj(h))->origin);
    h_complex_splice(h_new, h);
    return h_new;
}

half* h_mef(half* h1, half* h2) {
    half* h = h_make_complex_edge(h1->origin, h2->origin, h_dual(h1)->origin, h_dual(h_adj(h1))->origin);
    h_complex_splice(h, h1);
    h_complex_splice(h2, h_sym(h));
    return h;
}

void h_join_by_face(half* h1, half* h2) {
    //half* face_to_kill = h_adj(h1);

    half* f1 = h1;
    half* g1 = h2;
    
    half* f2 = h_fnext(h1);
    half* g2 = h_fnext(g1);
    
    half* f3 = h_fnext(f2);
    half* g3 = h_fnext(g2);
    
    if (h_adj(f1) != h_sym(h_adj(f1))) h_sew(h_adj(f1), h_sym(h_adj(g1)));
    if (h_adj(f2) != h_sym(h_adj(f2))) h_sew(h_adj(f2), h_sym(h_adj(g2)));
    if (h_adj(f3) != h_sym(h_adj(f3))) h_sew(h_adj(f3), h_sym(h_adj(g3)));

    h_snap(h_sym(h_dual(h_adj(h_fnext(f1)))), h_dual(h_adj(g1)));
    h_snap(h_sym(h_dual(h_adj(h_fnext(f2)))), h_dual(h_adj(g2)));
    h_snap(h_sym(h_dual(h_adj(h_fnext(f3)))), h_dual(h_adj(g3)));

    //Should kill face here...
}

//Other

void h_rename(half* h, int name) {
    h->name = name;
    h->sym->name = name + 2;
    h->dual->name = name + 1;
    h->dual->sym->name = name + 3;
}

void h_print(half* h) {
    printf("Half %i\n  origin = (%lf, %lf, %lf)\n  destination = (%lf, %lf, %lf)\n  onext: %i\n  fnext: %i\n  sym: %i\n  dual: %i\n",
        h->name,
        h->origin.x,
        h->origin.y,
        h->origin.z,
        h_sym(h)->origin.x,
        h_sym(h)->origin.y,
        h_sym(h)->origin.z,
        h_onext(h)->name,
        h_fnext(h)->name,
        h_sym(h)->name,
        h_dual(h)->name
    );
}
