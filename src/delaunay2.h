#ifndef DELAUNAY2
#define DELAUNAY2

#include "vector3.h"
#include "edge.h"
#include "triangle.h"

//Prototypes
triangle d2_extract_dual_face(edge* e);
triangle d2_extract_left_face(edge* e);
double d2_triangle_area(vector3 a, vector3 b, vector3 c);
bool d2_in_circle(vector3 a, vector3 b, vector3 c, vector3 p);
bool d2_ccw(vector3 a, vector3 b, vector3 c);
bool d2_left_of(vector3 p, edge* e);
edge* d2_locate(vector3 p, edge* e0);
void d2_insert(vector3 p, edge* e0);
bool d2_seen_node(edge* e);
int d2_extract_triangles(triangle* triangles, edge* outer_edge);
edge* d2_bounding_triangle();

#endif