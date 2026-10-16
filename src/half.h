#ifndef HALF
#define HALF

#include "vector3.h"

//Typedefs
struct Half {
    struct Half* onext;
    struct Half* fnext;
    struct Half* sym;
    struct Half* dual;
    int name;
    vector3 origin;
};
typedef struct Half half;

//Prototypes
half* h_onext(half* h);
half* h_fnext(half* h);
half* h_sym(half* h);
half* h_dual(half* h);
half* h_enext(half* h);
half* h_adj(half* h);
half* h_oprev(half* h);
half* h_fprev(half* h);
half* h_eprev(half* h);

half* h_make_half(vector3 p, vector3 d);
void h_connect(half* h1, half* h2);
void h_disconnect(half* h);
void h_half_splice(half* h1, half* h2);
half* h_make_edge(vector3 p1, vector3 p2, vector3 d);
void h_snap(half* h1, half* h2);
half* h_make_degenerate_edge(vector3 p, vector3 d);
void h_splice(half* h1, half* h2);
half* h_make_complex_edge(vector3 p1, vector3 p2, vector3 i, vector3 e);
void h_complex_splice(half* h1, half* h2);
half* h_make_face(vector3 p1, vector3 p2, vector3 p3, vector3 d);
void h_sew(half* h1, half* h2_sym);

half* h_mevvfs(vector3 p1, vector3 p2, vector3 i, vector3 e);
half* h_mev(vector3 p, half* h);
half* h_mef(half* h1, half* h2);
void h_join_by_face(half* h1, half* h2);

void h_rename(half* h, int name);
void h_print(half* h);

#endif