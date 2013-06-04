#include "transforms.h"

cord_t cartToCylind(cord_t c) {
    int r = sqrt(c.cart.x*c.cart.x + c.cart.y*c.cart.y);
    int p;
    if ( c.cart.x >= 0 ) {
        p = asin(c.cart.y/r);
    } else if ( c.cart.x < 0) {
        p = -1*asin(c.cart.y/r) + M_PI;
    } else {
        p = 0;
    }
    int z = c.cart.z;

    c.cyl.p = p;
    c.cyl.r = r;
    c.cyl.z = z;
    return c;
}

cord_t cylToCart(cord_t c) {
    int x = c.cyl.r*cos(c.cyl.p);
    int y = c.cyl.r*sin(c.cyl.p);
    int z = c.cyl.z;
    c.cart.x = x; 
    c.cart.y = y; 
    c.cart.z = z;
    return c;
}
   
