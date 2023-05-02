#include "include/GShader.h"
#include "include/GMatrix.h"
#include "MyBlendModes.h"
// #include "UtilsTileModes.h"

#ifndef MyTriColorShader_DEFINED
#define MyTriColorShader_DEFINED

class MyTriColorShader: public GShader {
public: 
    MyTriColorShader(GPoint points[3], GColor c[3]) {
        GPoint u = points[1] - points[0]; 
        GPoint v = points[2] - points[0]; 
        fExtraTransform = GMatrix(
            u.x(), v.x(), points[0].x(), 
            u.y(), v.y(), points[0].y()
        ); 
        for (int i = 0; i < 3; i++) {
            colors[i] = c[i];
        }
    }

    bool isOpaque() override {
        return colors[0].a == 1.0f && colors[1].a == 1.0f && colors[2].a == 1.0f; 
    }

    bool setContext(const GMatrix& ctm) override {
        return (ctm*fExtraTransform).invert(&localInverse); 
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        const float dx = localInverse[0]; 
        const float dy = localInverse[3]; 
        GPoint loc = localInverse * GPoint{x + 0.5f, y + 0.5f}; 

        GColor dc1 = colors[1] - colors[0]; 
        GColor dc2 = colors[2] - colors[0]; 
        GColor curColor = loc.x()*dc1 + loc.y()*dc2 + colors[0]; 
        GColor dc = dx*dc1 + dy*dc2; 

        for (int i = 0; i < count; ++i) {
            row[i] = MyBlendModes::premul(curColor.r, curColor.g, curColor.b, curColor.a); 
            curColor += dc; 
        }
    }

private: 
    GMatrix fExtraTransform; 
    GColor colors[3]; 
    GMatrix localInverse; 
}; 

GShader* GCreateTriColorShader(GPoint points[3], GColor colors[3]) {
    return (new MyTriColorShader(points, colors)); 
}

#endif