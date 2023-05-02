#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GColor.h"
#include "include/GMatrix.h"
#include "MyBlendModes.h"
#include "UtilsTileModes.h"

class MyGradientShader: public GShader {
public: 
    MyGradientShader(GPoint p0, GPoint p1, int count, const GColor colorsArr[], GShader::TileMode tileMode) {
        float dx = p1.x() - p0.x(); 
        float dy = p1.y() - p0.y(); 
        localMatrix = GMatrix(dx, -dy, p0.x(), 
                                dy, dx, p0.y()); 

        myColors = new GColor[count]; 
        for (int i = 0; i < count; ++i) {
            myColors[i] = colorsArr[i]; 
        }
        colorsCount = count; 
        myTileMode = tileMode; 
    }

    bool isOpaque() override {
        for (int i = 0; i < colorsCount; ++i) {
            if (myColors[i].a != 1.0f) return false; 
        }
        return true; 
    }

    bool setContext(const GMatrix& ctm) override {
        return (ctm*localMatrix).invert(&localInverse); 
    }

    void shadeRow(int x, int y, int count, GPixel *row) override {
        TileFunction tile; 
        if (myTileMode == TileMode::kClamp) {
            tile = &kClampFunction; 
        } else if (myTileMode == TileMode::kRepeat) {
            tile = &kRepeatFunction; 
        } else {
            tile = &kMirrorFunction; 
        }

        if (colorsCount == 1) {
            GColor c = myColors[0]; 
            for (int i = 0; i < count; ++i) {
                row[i] = MyBlendModes::premul(c.r, c.g, c.b, c.a); 
            }
            return; 
        // } else if (colorsCount == 2) {
        //     //GPoint loc = localInverse * GPoint{x + 0.5f, y + 0.5f}; 
        //     float locX = localInverse[0] * (x + 0.5f) + localInverse[1] * (y + 0.5f) + localInverse[2];
        //     float endX = localInverse[0] * (x + count - 1.0f + 0.5f) + localInverse[1] * (y + count - 1.0f + 0.5f) + localInverse[2];
        //     const float dx = localInverse[0];

        //     if (locX >= 0.0f && locX <= 1.0f && endX >= 0.0f && endX <= 1.0f) {
        //         GColor c = myColors[0]*(1.0f-locX) + myColors[1]*locX; 

        //         for (int i = 0; i < count; ++i) {
        //             //c = c.pinToUnit(); 
        //             row[i] = MyBlendModes::premul(c.r, c.g, c.b, c.a); 
        //             c += localInverse[0] * (myColors[1]-myColors[0]); 
        //         }
        //     } else {
        //         float locX = localInverse[0] * (x + 0.5f) + localInverse[1] * (y + 0.5f) + localInverse[2];
        //         for (int i = 0; i < count; ++i) {
        //             //float x = std::min(1.0f, std::max(0.0f, locX)); 
        //             float x = tile(locX, 1); 

        //             int color1Idx = GFloorToInt(x*(colorsCount-1)); 
        //             int color2Idx = std::min(color1Idx + 1, colorsCount-1); 

        //             float w2 = x*(colorsCount-1) - color1Idx; 
        //             float w1 = 1.0f - w2; 

        //             GColor c = myColors[color1Idx]*w1 + myColors[color2Idx]*w2; 

        //             row[i] = MyBlendModes::premul(c.r, c.g, c.b, c.a); 
        //             locX += dx; 
        //         }
        //     }
        //     return; 
        } else {
            const float dx = localInverse[0];
            //GPoint loc = localInverse * GPoint{x + 0.5f, y + 0.5f}; 
            float locX = localInverse[0] * (x + 0.5) + localInverse[1] * (y + 0.5) + localInverse[2];

            for (int i = 0; i < count; ++i) {
                //float x = std::min(1.0f, std::max(0.0f, locX)); 
                float x = tile(locX); 

                int color1Idx = GFloorToInt(x*(colorsCount-1)); 
                int color2Idx = std::min(color1Idx + 1, colorsCount-1); 

                float w2 = x*(colorsCount-1) - color1Idx; 
                float w1 = 1.0f - w2; 

                GColor c = myColors[color1Idx]*w1 + myColors[color2Idx]*w2; 

                row[i] = MyBlendModes::premul(c.r, c.g, c.b, c.a); 
                locX += dx; 
            }
            return; 
        }
    }

private: 
    int colorsCount; 
    GColor* myColors; 
    GMatrix localMatrix; 
    GMatrix localInverse; 
    GShader::TileMode myTileMode; 
}; 

std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GShader::TileMode tileMode) {
    return std::unique_ptr<GShader>(new MyGradientShader(p0, p1, count, colors, tileMode)); 
}