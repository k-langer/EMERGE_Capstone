#ifndef TRANSFORMS_H 
#define TRANSFORMS_H

#include <math.h>

typedef struct cartesian {
    double x;
    double y;
    double z;
} cart_t;

typedef struct cylindrical {
    double r;
    double p;
    double z;
} cyl_t;

typedef union cord {
    cart_t cart;
    cyl_t cyl;
} cord_t;
    
cord_t cartToCylind(cord_t c);
cord_t cylToCart(cord_t c);

#endif
