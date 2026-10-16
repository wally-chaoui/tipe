#ifndef EDGE
#define EDGE

#include "vector3.h"

//Typedefs
struct Edge {
    struct Edge* onext;
    struct Edge* rot; //ATTENTION ne pas toucher
    int name;
    int index;
    vector3 origin;
};
typedef struct Edge edge;

//Prototypes
edge* e_onext(edge* e);
edge* e_rot(edge* e);
edge* e_sym(edge* e);
edge* e_anti_rot(edge* e);
edge* e_anti_onext(edge* e);
edge* e_dnext(edge* e);
edge* e_anti_dnext(edge* e);
edge* e_lnext(edge* e);
edge* e_rnext(edge* e);
edge* e_anti_lnext(edge* e);
edge* e_anti_rnext(edge* e);
edge* e_make_edge();
void e_splice(edge* a, edge* b);
edge* e_connect(edge* a, edge* b);
void e_remove(edge* e);
void e_swap(edge* e);
void e_print(edge* e);
void e_set_endpoints(edge* e, vector3 o, vector3 d);
void e_rename(edge* e, int base_name);

#endif