#ifndef LIGHT
#define LIGHT

#include "vector3.h"

//Typedefs
struct Light{
    vector3 position;
    double power;
};
typedef struct Light light;

//Prototypes
light l_create(vector3 position, double power);

#endif