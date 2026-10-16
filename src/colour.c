#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "colour.h"

colour c_create_random() {
    int p = rand() % 50;
    int r = rand() % 255;
    int g = p;
    int b = 100 - p;
    colour c = {.r = r, .g = g, .b = b, .brightness = 1};
    return c;
}

colour c_create_white() {
    return (colour){.r = 255, .g = 255, .b = 255, .brightness = 1};
}

colour c_create_gradient(double min, double max, double x) {
    double d = max - min;
    double p = (x - min) / d;
    double b_p = 1.0 - 2.0 * fabs(p - 0.0);
    double g_p = 1.0 - 2.0 * fabs(p - 0.5);
    double r_p = 1.0 - 2.0 * fabs(p - 1.0);
    double r; double g; double b;
    if (r_p > 0) r = 255.0 * r_p; else r = 0.0;
    if (g_p > 0) g = 255.0 * g_p; else g = 0.0;
    if (b_p > 0) b = 255.0 * b_p; else b = 0.0;
    return (colour){.r = r, .g = g, .b = b};
}
