#include "include/GMatrix.h"
#include <cmath>

GMatrix::GMatrix() {
    fMat[0] = 1.0;    fMat[1] = 0.0;    fMat[2] = 0.0;
    fMat[3] = 0.0;    fMat[4] = 1.0;    fMat[5] = 0.0;
}

GMatrix GMatrix::Translate(float tx, float ty) {
    return GMatrix{
        1, 0, tx, 
        0, 1, ty
    }; 
}

GMatrix GMatrix::Scale(float sx, float sy) {
    return GMatrix{
        sx, 0, 0, 
        0, sy, 0
    }; 
}

GMatrix GMatrix::Rotate(float radians) {
    return GMatrix{
        cos(radians), -sin(radians), 0, 
        sin(radians), cos(radians), 0
    }; 
}

GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b) {
    return GMatrix{
        a[0]*b[0]+a[1]*b[3], a[0]*b[1]+a[1]*b[4], a[0]*b[2]+a[1]*b[5]+a[2], 
        a[3]*b[0]+a[4]*b[3], a[3]*b[1]+a[4]*b[4], a[3]*b[2]+a[4]*b[5]+a[5], 
    }; 
}

// original matrix should not share same memory address as inverse ?? (depends on implementation)
/**
 * 1. if determinant == 0, matrix isn't invertible
 * 2. calculate matrix's adjugate (transpose of matrix of cofactors)
 * 3. multiply matrix's adjugate by the reciprocal of the determinant
*/
bool GMatrix::invert(GMatrix* inverse) const {
    float determinant = fMat[0]*fMat[4]-fMat[1]*fMat[3]; 
    if (determinant == 0) return false; 

    float a = fMat[0]; 
    float b = fMat[1]; 
    float c = fMat[2]; 
    float d = fMat[3]; 
    float e = fMat[4]; 
    float f = fMat[5]; 

    inverse->fMat[0] = e/determinant; 
    inverse->fMat[1] = -b/determinant; 
    inverse->fMat[2] = (b*f-e*c)/determinant; 
    inverse->fMat[3] = -d/determinant; 
    inverse->fMat[4] = a/determinant; 
    inverse->fMat[5] = (c*d-a*f)/determinant; 
    
    return true; 
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    //assert(sizeof(dst) >= sizeof(src)); 
    for (int i = 0; i < count; i++) {
        GPoint srcPt = src[i]; 
        GPoint dstPt; 
        dstPt.fX = fMat[0]*(srcPt.x())+fMat[1]*(srcPt.y())+fMat[2]; 
        dstPt.fY = fMat[3]*(srcPt.x())+fMat[4]*(srcPt.y())+fMat[5];
        dst[i] = dstPt; 
    }
}
