#include "include/GBitmap.h"
#include "include/GShader.h"
#include "include/GPaint.h"
#include "MyBlendModes.h"

#ifndef UtilsBlit_DEFINED
#define UtilsBlit_DEFINED

typedef void (*BlitFunction) (int, int, int, GBitmap, GPaint);

static void blitKClear(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kClear(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kClear(src, *pixel);
        }
    }
}

static void blitKSrc(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrc(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrc(src, *pixel);
        }
    }
}

static void blitKDst(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDst(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDst(src, *pixel);
        }
    }
}

static void blitKSrcOver(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcOver(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcOver(src, *pixel);
        }
    }
}

static void blitKDstOver(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstOver(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstOver(src, *pixel);
        }
    }
}

static void blitKSrcIn(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcIn(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcIn(src, *pixel);
        }
    }
}

static void blitKDstIn(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstIn(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstIn(src, *pixel);
        }
    }
}

static void blitKSrcOut(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcOut(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcOut(src, *pixel);
        }
    }
}

static void blitKDstOut(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstOut(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstOut(src, *pixel);
        }
    }
}

static void blitKSrcATop(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcATop(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kSrcATop(src, *pixel);
        }
    }
}

static void blitKDstATop(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstATop(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kDstATop(src, *pixel);
        }
    }
}

static void blitKXor(int x0, int x1, int y, GBitmap fDevice, GPaint paint) {
    GShader* shader = paint.getShader(); 
    if (shader != nullptr) {
        int count = x1 - x0; 
        assert(count >= 0); 
        GPixel src[count]; 
        shader->shadeRow(x0, y, count, src); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kXor(src[x-x0], *pixel);
        }
    } else {
        GColor color = paint.getColor(); 
        GPixel src = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
        for (int x = x0; x < x1; ++x) {
            assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
            GPixel* pixel = fDevice.getAddr(x, y);
            *pixel = MyBlendModes::kXor(src, *pixel);
        }
    }
}

#endif