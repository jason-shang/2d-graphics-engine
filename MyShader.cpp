#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GMath.h"
#include "UtilsTileModes.h"

class MyShader: public GShader {
public: 
    MyShader(const GBitmap& bitmap, const GMatrix& localMatrix, GShader::TileMode tileMode) : bitmap(bitmap), localMatrix(localMatrix), myTileMode(tileMode) {}

    bool isOpaque() override {
        return bitmap.isOpaque(); 
    }

    bool setContext(const GMatrix& ctm) override {
        if (!ctm.invert(&localInverse)) return false; 
        localInverse = localMatrix*localInverse; 
        return true; 
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        TileFunction tile; 
        if (myTileMode == TileMode::kClamp) {
            tile = &kClampFunction; 
            width = bitmap.width() - 1; 
            height = bitmap.height() - 1; 
        } else if (myTileMode == TileMode::kRepeat) {
            tile = &kRepeatFunction; 
            width = bitmap.width(); 
            height = bitmap.height(); 
        } else {
            tile = &kMirrorFunction; 
            width = bitmap.width(); 
            height = bitmap.height(); 
        }

        const float dx = localInverse[0];
        const float dy = localInverse[3];
        GPoint loc = localInverse * GPoint{x + 0.5f, y + 0.5f};

        if (localInverse[0] == 0 && localInverse[3] == 0) {
            int x_new = GFloorToInt(tile(loc.x()/width) * width);
            int y_new = GFloorToInt(tile(loc.y()/height) * height); 
            for (int i = 0; i < count; ++i) {
                row[i] = *bitmap.getAddr(x_new, y_new); 
            }
            return; 
        }

        if (localInverse[0] == 0) {
            int x_new = GFloorToInt(tile(loc.x()/width) * width);
            for (int i = 0; i < count; ++i) {
                int y_new = GFloorToInt(tile(loc.y()/height)) * height; 
                row[i] = *bitmap.getAddr(x_new, y_new); 
                loc.fY += dy; 
            }
            return; 
        }

        if (localInverse[3] == 0) {
            int y_new = GFloorToInt(tile(loc.y()/height) * height); 
            for (int i = 0; i < count; ++i) {
                int x_new = GFloorToInt(tile(loc.x()/width) * width); 
                row[i] = *bitmap.getAddr(x_new, y_new); 
                loc.fX += dx; 
            }
            return; 
        }
        
        for (int i = 0; i < count; ++i) { 
            int x_new = GFloorToInt(tile(loc.x()/width) * width); 
            int y_new = GFloorToInt(tile(loc.y()/height) * height); 
            row[i] = *bitmap.getAddr(x_new, y_new);
            loc.fX += dx;
            loc.fY += dy;
        }

        return; 
    }

private: 
    const GBitmap bitmap; 
    GMatrix localMatrix; 
    GMatrix localInverse; 
    GShader::TileMode myTileMode; 
    // used to calculate x & y for getAddr
    int width; 
    int height; 
};

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& bitmap, const GMatrix& localInverse, GShader::TileMode tileMode) {
    return std::unique_ptr<GShader>(new MyShader(bitmap, localInverse, tileMode));
}