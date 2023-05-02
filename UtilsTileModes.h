#include "include/GMath.h"

#ifndef UtilsTileModes_DEFINED
#define UtilsTileModes_DEFINED

typedef float (*TileFunction) (float);

static float kClampFunction(float x) {
    return std::max(0.0f, std::min(0.999999999f, x)); 
}

static float kRepeatFunction(float x) {
    return x - GFloorToInt(x); 
}

static float kMirrorFunction(float x) {
    // // map 0 to 2 -> 0 to 1
    // x *= 0.5;
    // // construct repeat function but with 0 to 2 domain
    // int divisor = GFloorToInt(x/scale); 
    // x = x - GFloorToInt(scale * divisor);

    // if (x > 0.5) x = 1-x;
    // return x *= 1.99999f;

    x = x-1; 
    x = x- GFloorToInt(x*0.5f)*1.9999999f-1.0f; 
    return abs(x); 
}

#endif