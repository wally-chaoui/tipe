#ifndef COLOUR
#define COLOUR

//Typedefs
struct Colour {
    double r;
    double g;
    double b;
    double brightness;
};
typedef struct Colour colour;

//Prototypes
colour c_create_random();
colour c_create_white();
colour c_create_gradient(double min, double max, double x);

#endif