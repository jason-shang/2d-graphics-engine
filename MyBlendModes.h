#include "include/GMath.h"
#include "include/GPixel.h"

#ifndef MyBlendModes_DEFINED
#define MyBlendModes_DEFINED

class MyBlendModes {
public: 

static inline GPixel premul(float r, float g, float b, float a) {
    const float multiplier = 255.0f*a; 
    int newA = GRoundToInt(multiplier); 
    int newR = GRoundToInt(multiplier*r); 
    int newG = GRoundToInt(multiplier*g); 
    int newB = GRoundToInt(multiplier*b); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline uint8_t GDiv255(unsigned prod) {
    return (prod + 128) * 257 >> 16; 
}

static inline void assertARGB(int newA, int newR, int newG, int newB) {
    assert(newA >= 0 && newA <= 255); 
    assert(newR >= 0 && newR <= 255); 
    assert(newG >= 0 && newG <= 255); 
    assert(newB >= 0 && newB <= 255); 
}

static inline GPixel kClear(GPixel src, GPixel dst) {
    return GPixel_PackARGB(0, 0, 0, 0); 
}

static inline GPixel kSrc(GPixel src, GPixel dst) {
    return src; 
}

static inline GPixel kDst(GPixel src, GPixel dst) {
    return dst; 
}

static inline GPixel kSrcOver(GPixel src, GPixel dst) {
    int srcA = GPixel_GetA(src); 

    int newA = srcA+GDiv255(GPixel_GetA(dst)*(255-srcA));
    int newR = GPixel_GetR(src)+GDiv255(GPixel_GetR(dst)*(255-srcA)); 
    int newG = GPixel_GetG(src)+GDiv255(GPixel_GetG(dst)*(255-srcA));
    int newB = GPixel_GetB(src)+GDiv255(GPixel_GetB(dst)*(255-srcA)); 

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kDstOver(GPixel src, GPixel dst) {
    int dstA = GPixel_GetA(dst); 

    int newA = dstA+GDiv255(GPixel_GetA(src)*(255-dstA));
    int newR = GPixel_GetR(dst)+GDiv255(GPixel_GetR(src)*(255-dstA)); 
    int newG = GPixel_GetG(dst)+GDiv255(GPixel_GetG(src)*(255-dstA));
    int newB = GPixel_GetB(dst)+GDiv255(GPixel_GetB(src)*(255-dstA)); 

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kSrcIn(GPixel src, GPixel dst) {
    int dstA = GPixel_GetA(dst); 

    int newA = GDiv255(dstA * GPixel_GetA(src)); 
    int newR = GDiv255(dstA * GPixel_GetR(src)); 
    int newG = GDiv255(dstA * GPixel_GetG(src));
    int newB = GDiv255(dstA * GPixel_GetB(src));

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kDstIn(GPixel src, GPixel dst) {
    int srcA = GPixel_GetA(src); 

    int newA = GDiv255(srcA * GPixel_GetA(dst)); 
    int newR = GDiv255(srcA * GPixel_GetR(dst)); 
    int newG = GDiv255(srcA * GPixel_GetG(dst));
    int newB = GDiv255(srcA * GPixel_GetB(dst));

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kSrcOut(GPixel src, GPixel dst) {
    int dstA = GPixel_GetA(dst); 

    int newA = GDiv255(GPixel_GetA(src)*(255-dstA));
    int newR = GDiv255(GPixel_GetR(src)*(255-dstA)); 
    int newG = GDiv255(GPixel_GetG(src)*(255-dstA)); 
    int newB = GDiv255(GPixel_GetB(src)*(255-dstA));

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kDstOut(GPixel src, GPixel dst) {
    int srcA = GPixel_GetA(src); 

    int newA = GDiv255(GPixel_GetA(dst)*(255-srcA));
    int newR = GDiv255(GPixel_GetR(dst)*(255-srcA)); 
    int newG = GDiv255(GPixel_GetG(dst)*(255-srcA)); 
    int newB = GDiv255(GPixel_GetB(dst)*(255-srcA));

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kSrcATop(GPixel src, GPixel dst) {
    int srcA = GPixel_GetA(src); 
    int dstA = GPixel_GetA(dst); 

    int newA = GDiv255(dstA*GPixel_GetA(src)+(GPixel_GetA(dst)*(255-srcA)));
    int newR = GDiv255(dstA*GPixel_GetR(src)+(GPixel_GetR(dst)*(255-srcA))); 
    int newG = GDiv255(dstA*GPixel_GetG(src)+(GPixel_GetG(dst)*(255-srcA))); 
    int newB = GDiv255(dstA*GPixel_GetB(src)+(GPixel_GetB(dst)*(255-srcA)));

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kDstATop(GPixel src, GPixel dst) {
    int srcA = GPixel_GetA(src); 
    int dstA = GPixel_GetA(dst); 

    int newA = GDiv255(srcA*GPixel_GetA(dst)+(GPixel_GetA(src)*(255-dstA)));
    int newR = GDiv255(srcA*GPixel_GetR(dst)+(GPixel_GetR(src)*(255-dstA))); 
    int newG = GDiv255(srcA*GPixel_GetG(dst)+(GPixel_GetG(src)*(255-dstA))); 
    int newB = GDiv255(srcA*GPixel_GetB(dst)+(GPixel_GetB(src)*(255-dstA)));

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}

static inline GPixel kXor(GPixel src, GPixel dst) {
    int srcA = GPixel_GetA(src); 
    int dstA = GPixel_GetA(dst); 

    int newA = GDiv255(GPixel_GetA(src)*(255-dstA)+(GPixel_GetA(dst)*(255-srcA)));
    int newR = GDiv255(GPixel_GetR(src)*(255-dstA)+(GPixel_GetR(dst)*(255-srcA)));
    int newG = GDiv255(GPixel_GetG(src)*(255-dstA)+(GPixel_GetG(dst)*(255-srcA)));
    int newB = GDiv255(GPixel_GetB(src)*(255-dstA)+(GPixel_GetB(dst)*(255-srcA)));

    assertARGB(newA, newR, newG, newB); 
    return GPixel_PackARGB(newA, newR, newG, newB); 
}
}; 

#endif