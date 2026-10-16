#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "light.h"
#include "vector3.h"

light l_create(vector3 position, double power) {
    light l = {.position = position, .power = power};
    return l;
}
