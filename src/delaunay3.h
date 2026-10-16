#ifndef DELAUNAY3
#define DELAUNAY3

#include "vector3.h"
#include "half.h"
#include "triangle.h"

//Prototypes
half* d3_create_tetrahedron(vector3 a, vector3 b, vector3 c, vector3 d);
half* d3_cube();
half* d3_test();
half* d3_bounding_tetrahedron();
triangle d3_extract_dual_face(half* h);
triangle d3_extract_face(half* h);
int d3_extract_triangles(triangle* triangles, half* d0);
void d3_insert(vector3 p, half* d0);

#endif