#ifndef VECTOR3
#define VECTOR3

//Typedefs
struct Vector3{
    double x;
    double y;
    double z;
};
typedef struct Vector3 vector3;

//Prototypes
vector3 v_create(double x, double y, double z);
double v_norm(vector3 v);
//void v_normalise(vector3* v);
vector3 v_normalise(vector3 v);
vector3 v_rotate(vector3 v, double x_rot, double y_rot, double z_rot);
vector3 v_add(vector3 v1, vector3 v2);
vector3 v_sub(vector3 v1, vector3 v2);
vector3 v_mul(vector3 v, double t);
double v_dot(vector3 v1, vector3 v2);
vector3 v_cross(vector3 v1, vector3 v2);
void v_print(vector3 v);
vector3 v_min(vector3* tab, int n_tab);
vector3 v_max(vector3* tab, int n_tab);

#endif