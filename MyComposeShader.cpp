#include "include/GShader.h"
#include "MyBlendModes.h"

class MyComposeShader: public GShader {
public: 
    MyComposeShader(GShader* s0, GShader* s1) {
        shader0 = s0; 
        shader1 = s1; 
    }

    bool isOpaque() override {
        return shader0->isOpaque() && shader1->isOpaque(); 
    }

    bool setContext(const GMatrix& ctm) override {
        return shader0->setContext(ctm) && shader1->setContext(ctm);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPixel row0[count];
        GPixel row1[count];
        shader0->shadeRow(x, y, count, row0);
        shader1->shadeRow(x, y, count, row1);
        for (int i = 0; i < count; i++) {
            row[i] = GPixel_PackARGB(
                MyBlendModes::GDiv255(GPixel_GetA(row0[i]) * GPixel_GetA(row1[i])),
                MyBlendModes::GDiv255(GPixel_GetR(row0[i]) * GPixel_GetR(row1[i])),
                MyBlendModes::GDiv255(GPixel_GetG(row0[i]) * GPixel_GetG(row1[i])),
                MyBlendModes::GDiv255(GPixel_GetB(row0[i]) * GPixel_GetB(row1[i]))
            );
        }
    }

private: 
    GShader* shader0; 
    GShader* shader1; 
}; 

GShader* GCreateComposeShader(GShader* s0, GShader* s1) {
    return (new MyComposeShader(s0, s1));
}