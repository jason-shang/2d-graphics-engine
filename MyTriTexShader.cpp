#include "include/GShader.h"
#include "include/GMatrix.h"
#include "MyBlendModes.h"

class MyTriTexShader: public GShader {
public: 
    MyTriTexShader(GShader* shader, GPoint points[3], GPoint texs[3]) {
        wrappedShader = shader; 
        
        // compute the basis vectors of the points array
        GPoint u = points[1] - points[0]; 
        GPoint v = points[2] - points[0]; 
        pointsMatrix = GMatrix(
            u.x(), v.x(), points[0].x(), 
            u.y(), v.y(), points[0].y()
        ); 

        // compute the basis vectors of the texs array of points
        GPoint a = texs[1] - texs[0]; 
        GPoint b = texs[2] - texs[0]; 
        texsMatrix = GMatrix(
            a.x(), b.x(), texs[0].x(), 
            a.y(), b.y(), texs[0].y()
        ); 
    }

    bool isOpaque() override {
        return wrappedShader->isOpaque(); 
    }

    bool setContext(const GMatrix& ctm) override {
        texsMatrix.invert(&texInverse); 
        GMatrix localMatrix = pointsMatrix * texInverse; 
        return wrappedShader->setContext(ctm * localMatrix); 
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        wrappedShader->shadeRow(x, y, count, row); 
    }

private: 
    GMatrix pointsMatrix; 
    GMatrix texsMatrix; 
    GMatrix texInverse; 
    GShader* wrappedShader; 
}; 

GShader* GCreateTriTexShader(GShader* shader, GPoint points[3], GPoint texs[3]) {
    return (new MyTriTexShader(shader, points, texs)); 
}