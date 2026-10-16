#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "vector3.h"
#include "edge.h"

//Navigation

edge* e_onext(edge* e) {
    return e->onext;
}

edge* e_rot(edge* e) {
    return e->rot;
}

edge* e_sym(edge* e) {
    return e->rot->rot;
}

edge* e_anti_rot(edge* e) {
    return e->rot->rot->rot;
}

edge* e_anti_onext(edge* e) {
    return e_rot(e_onext(e_rot(e)));
}

edge* e_dnext(edge* e) {
    return e_sym(e_onext(e_sym(e)));
}

edge* e_anti_dnext(edge* e) {
    return e_anti_rot(e_onext(e_anti_rot(e)));
}

edge* e_lnext(edge* e) {
    return e_rot(e_onext(e_anti_rot(e)));
}

edge* e_rnext(edge* e) {
    return e_anti_rot(e_onext(e_rot(e)));
}

edge* e_anti_lnext(edge* e) {
    return e_sym(e_onext(e));
}

edge* e_anti_rnext(edge* e) {
    return e_onext(e_sym(e));
}

//Construction

edge* e_make_edge() {
    edge* e0 = (edge*)malloc(sizeof(edge));
    edge* e1 = (edge*)malloc(sizeof(edge));
    edge* e2 = (edge*)malloc(sizeof(edge));
    edge* e3 = (edge*)malloc(sizeof(edge));
    e0->onext = e0;
    e1->onext = e3;
    e2->onext = e2;
    e3->onext = e1;
    e0->rot = e1;
    e1->rot = e2;
    e2->rot = e3;
    e3->rot = e0;
    e0->name = 0;
    e1->name = 0;
    e2->name = 0;
    e3->name = 0;
    e0->index = 0;
    e1->index = 1;
    e2->index = 2;
    e3->index = 3;
    vector3 default_origin = v_create(0.0, 0.0, 0.0);
    e0->origin = default_origin;
    e1->origin = default_origin;
    e2->origin = default_origin;
    e3->origin = default_origin;
    return e0;
}

void e_splice(edge* a, edge* b) {
    edge* alpha = e_rot(e_onext(a));
    edge* beta = e_rot(e_onext(b));
    edge* a_next = e_onext(b);
    edge* b_next = e_onext(a);
    edge* alpha_next = e_onext(beta);
    edge* beta_next = e_onext(alpha);
    a->onext = a_next;
    b->onext = b_next;
    alpha->onext = alpha_next;
    beta->onext = beta_next;
}

edge* e_connect(edge* a, edge* b) {
    edge* e = e_make_edge();
    e->origin = e_sym(a)->origin;
    e_sym(e)->origin = b->origin;
    e_splice(e, e_lnext(a));
    e_splice(e_sym(e), b);
    return e;
}

void e_remove(edge* e) {
    e_splice(e, e_anti_onext(e));
    e_splice(e_sym(e), e_anti_onext(e_sym(e)));
}

void e_swap(edge* e) {
    edge* a = e_anti_onext(e);
    edge* b = e_anti_onext(e_sym(e));
    e_splice(e, a);
    e_splice(e_sym(e), b);
    e_splice(e, e_lnext(a));
    e_splice(e_sym(e), e_lnext(b));
    e->origin = e_sym(a)->origin;
    e_sym(e)->origin = e_sym(b)->origin;
}

void e_set_endpoints(edge* e, vector3 o, vector3 d) {
    e->origin = o;
    e_sym(e)->origin = d;
}

void e_rename(edge* e, int base_name) {
    e->name = base_name;
    e_rot(e)->name = base_name + 1;
    e_rot(e_rot(e))->name = base_name + 2;
    e_rot(e_rot(e_rot(e)))->name = base_name + 3;
}

//Other

void e_print(edge* e) {
    printf("Edge %i\n  index = %i\n  origin = (%lf, %lf, %lf)\n  destination = (%lf, %lf, %lf)\n  onext: %i\n",
        e->name,
        e->index,
        e->origin.x,
        e->origin.y,
        e->origin.z,
        e_sym(e)->origin.x,
        e_sym(e)->origin.y,
        e_sym(e)->origin.z,
        e_onext(e)->name
    );
}
