/*
 *  Copyright 2023 Jason Shang
 */

#include "include/GPoint.h"
#include "include/GPaint.h"
#include "include/GRect.h"
#include "include/GMath.h"
#include "include/GBitmap.h"
#include "include/GBlendMode.h"
#include "UtilsBlit.h"

#ifndef UtilsConvexPolygon_DEFINED
#define UtilsConvexPolygon_DEFINED

struct Edge {
    int top; 
    int bottom; 
    float m; 
    float currX; 
    int wind; 
}; 

bool noDrawing(const GPaint& paint) {
    GShader* shader = paint.getShader(); 
    GBlendMode blend = paint.getBlendMode(); 
    if (blend == GBlendMode::kDst) return true; 

    if (shader != nullptr) {
        if (shader->isOpaque()) {
            if (blend == GBlendMode::kDstIn) return true; 
        }
    } else {
        int srcA = GRoundToInt(255.0*paint.getColor().a); 
        GBlendMode blend = paint.getBlendMode(); 
        if (blend == GBlendMode::kDst) return true; 
        if (
            srcA == 0 && (
                blend == GBlendMode::kSrcOver ||
                blend == GBlendMode::kDstOver ||
                blend == GBlendMode::kDstOut ||
                blend == GBlendMode::kSrcATop ||
                blend == GBlendMode::kXor
            ) || 
            srcA == 1 && blend == GBlendMode::kDstIn
        ) return true; 
    }
    return false; 
}

static inline bool edgeIsOutOfRange(const Edge &e, int y) {
    assert(y < e.bottom && y >= e.top); 
    return y == e.bottom - 1; 
}

BlitFunction setBlendMode(const GPaint& paint, int srcA){
    GBlendMode blend = paint.getBlendMode(); 
    GShader* shader = paint.getShader(); 

    switch(blend){
        case GBlendMode::kClear: return &blitKClear;
        case GBlendMode::kSrc: return &blitKSrc;
        case GBlendMode::kDst: return &blitKDst;
        case GBlendMode::kSrcOver:
            if (shader != nullptr) { 
                if(shader->isOpaque()){ return &blitKSrc; }
                return &blitKSrcOver; 
            }
            if (srcA == 0) return &blitKDst;
            if (srcA == 255) return &blitKSrc;
            return &blitKSrcOver;
        case GBlendMode::kDstOver:
            if (shader != nullptr) return &blitKDstOver; 
            if (srcA == 0) return &blitKDst;
            return &blitKDstOver;
        case GBlendMode::kSrcIn:
            if (shader != nullptr) return &blitKSrcIn; 
            if (srcA == 0) return &blitKClear;
            return &blitKSrcIn;
        case GBlendMode::kDstIn:
            if (shader != nullptr) return &blitKDstIn;
            if (srcA == 0) return &blitKClear;
            return &blitKDstIn;
        case GBlendMode::kSrcOut:
            if (shader != nullptr) return &blitKSrcOut; 
            if (srcA == 0) return &blitKClear;
            return &blitKSrcOut;
        case GBlendMode::kDstOut:
            if (shader != nullptr){ 
                if(shader->isOpaque()) return &blitKClear; 
                return &blitKDstOut; 
            }
            if (srcA == 0) return &blitKDst;
            if (srcA == 255) return &blitKClear;
            return &blitKDstOut;
        case GBlendMode::kSrcATop:
            if (shader != nullptr) return &blitKSrcATop; 
            if (srcA == 0) return &blitKDst;
            return &blitKSrcATop;
        case GBlendMode::kDstATop:
            if (shader != nullptr) return &blitKDstATop; 
            if(srcA == 0) return &blitKClear;
            return &blitKDstATop;
        case GBlendMode::kXor:
            if (shader != nullptr) return &blitKXor; 
            if (srcA == 0) return &blitKDst;
            return &blitKXor;
        default:
            return &blitKClear;
    }
}

// BlendProc setBlendMode(const GPaint& paint, int srcA) {
//     GBlendMode blend = paint.getBlendMode(); 
//     GShader* shader = paint.getShader(); 

//     if (shader != nullptr) {
//         if (shader->isOpaque()) {
//             if (blend == GBlendMode::kDstOut) return kClear; 
//             if (blend == GBlendMode::kXor) return kSrcOut; 
//             if (blend == GBlendMode::kSrcOver) return kSrc; 
//             if (blend == GBlendMode::kSrcATop) return kSrcIn;
//         }
//     } else {
//         if (
//             srcA == 0 && (
//                 blend == GBlendMode::kDstIn ||
//                 blend == GBlendMode::kSrc ||
//                 blend == GBlendMode::kSrcIn ||
//                 blend == GBlendMode::kDstIn ||
//                 blend == GBlendMode::kSrcOut ||
//                 blend == GBlendMode::kDstATop
//             ) ||
//             blend == GBlendMode::kDstOut && srcA == 255
//         ) return kClear; 
//         if (
//             blend == GBlendMode::kDstATop && srcA == 0 ||
//             blend == GBlendMode::kXor && srcA == 255
//         ) return kSrcOut; 
//         if (blend == GBlendMode::kSrcOver && srcA == 255) return kSrc; 
//         if (blend == GBlendMode::kSrcATop && srcA == 255) return kSrcIn;
//     }

//     switch(blend) {
//         case GBlendMode::kClear:     //!<     0
//             return kClear;
//         case GBlendMode::kSrc:    //!<blend = kSrc; 
//             return kSrc; 
//         case GBlendMode::kDst:      //!<     D
//             return kDst; 
//         case GBlendMode::kSrcOver: //!<     S + (1 - Sa)*D
//             return kSrcOver; 
//         case GBlendMode::kDstOver: //!<     D + (1 - Da)*S
//             return kDstOver; 
//         case GBlendMode::kSrcIn:    //!<     Da * S
//             return kSrcIn; 
//         case GBlendMode::kDstIn:    //!<     Sa * D
//             return kDstIn; 
//         case GBlendMode::kSrcOut:   //!<     (1 - Da)*S
//             return kSrcOut; 
//         case GBlendMode::kDstOut:   //!<     (1 - Sa)*D
//             return kDstOut; 
//         case GBlendMode::kSrcATop:  //!<     Da*S + (1 - Sa)*D
//             return kSrcATop; 
//         case GBlendMode::kDstATop:  //!<     Sa*D + (1 - Da)*S
//             return kDstATop; 
//         case GBlendMode::kXor:   //!<     (1 - Sa)*D + (1 - Da)*S
//             return kXor; 
//     }; 

//     return nullptr; 
// }

void makeEdge(GPoint p0, GPoint p1, std::vector<Edge>& edges, int wind) {
    Edge edge; 
    if (p0.y() > p1.y()) {
        // wind *= -1; 
        std::swap(p0, p1); 
    }
    edge.top = GRoundToInt(p0.y()); 
    edge.bottom = GRoundToInt(p1.y()); 
    if (edge.top == edge.bottom) return; 
    edge.m = (p0.x()-p1.x())/(p0.y()-p1.y()); 
    edge.wind = wind; 
    float b = p0.x()-(edge.m * p0.y()); 
    edge.currX = edge.m * (edge.top+0.5) + b; 
    edges.push_back(edge); 
}

// template<typename BlendProc> void blitSingleColor(int x0, int x1, int y, GBitmap fDevice, BlendProc blend, GPixel src) {
//     for (int x = x0; x < x1; ++x) {
//         assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
//         GPixel* pixel = fDevice.getAddr(x, y);
//         *pixel = blend(src, *pixel);
//     }
// }

// template<typename BlendProc> void blitShader(int x0, int x1, int y, GBitmap fDevice, BlendProc blend, GShader* shader) {
//     int count = x1 - x0; 
//     assert(count >= 0); 
//     GPixel src[count]; 
//     shader->shadeRow(x0, y, count, src); 
//     for (int x = x0; x < x1; ++x) {
//         assert(x >= 0 && x <= fDevice.width() && y >= 0 && y <= fDevice.height());
//         GPixel* pixel = fDevice.getAddr(x, y);
//         *pixel = blend(src[x-x0], *pixel);
//     }
// }

void castRays(std::vector<Edge>& edges, GBitmap fDevice, const GPaint& paint) {
    if (edges.size() < 2) return; 

    GColor color = paint.getColor(); 
    GPixel srcPixel = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
    BlitFunction blit = setBlendMode(paint, GPixel_GetA(srcPixel)); 

    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.top < b.top; 
    }); 
    int top = edges.front().top, bottom = edges.back().bottom;

    int e0ptr = 0, e1ptr = 1; 
    for (int y = top; y < bottom; ++y) {
        assert(edges.size() >= 2); 
        Edge& e0 = edges[e0ptr]; 
        Edge& e1 = edges[e1ptr]; 
        int x0 = GRoundToInt(e0.currX); 
        int x1 = GRoundToInt(e1.currX); 
        e0.currX += e0.m; 
        e1.currX += e1.m; 
        if (x0 > x1) std::swap(x0, x1); 
        blit(x0, x1, y, fDevice, paint); 
        // if (paint.getShader() != nullptr) {
        //     blitShader(x0, x1, y, fDevice, blend, paint.getShader()); 
        // } else {
        //     blitSingleColor(x0, x1, y, fDevice, blend, srcPixel); 
        // }
        if (edgeIsOutOfRange(e1, y)) {
            if (e0ptr < e1ptr) e1ptr++; 
            else e1ptr = e0ptr+1; 
        }
        if (edgeIsOutOfRange(e0, y)) {
            if (e0ptr < e1ptr) e0ptr=e1ptr+1; 
            else e0ptr++; 
        }
    }
}

// checks if the edge is to be considered for the current y value
bool edgeIsActive(Edge edge, int y) {
    assert(y < edge.bottom); 
    return y >= edge.top; 
}

void castRaysComplex(std::vector<Edge> edges, GBitmap fDevice, const GPaint& paint) {
    if (edges.size() < 2) return; 

    GColor color = paint.getColor(); 
    GPixel srcPixel = MyBlendModes::premul(color.r, color.g, color.b, color.a); 
    BlitFunction blit = setBlendMode(paint, GPixel_GetA(srcPixel)); 

    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.bottom < b.bottom; 
    }); 
    int bottom = edges.back().bottom; 

    // sort by top, break tie using currX
    // optimizations: sort in descending order?
    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        if (a.top != b.top) {
            return a.top < b.top; 
        } else {
            return a.currX < b.currX; 
        }
    }); 
    int top = edges.front().top; 

    int y = top; 
    int left = 0; 
    int right = 0; 
    int edgeIdx = 0; 

    std::vector<Edge> activeEdges; 
    activeEdges.reserve(edges.size()); 
    while (y < bottom) {
        assert(edges.size() >= 2); 
        int w = 0; 

        while (edgeIdx < edges.size() && edgeIsActive(edges[edgeIdx], y)) {
            activeEdges.push_back(edges[edgeIdx]); 
            edgeIdx++; 
        }

        std::sort(activeEdges.begin(), activeEdges.end(), [](const Edge& a, const Edge& b) {
            return a.currX < b.currX; 
        });

        for (int i = 0; i < activeEdges.size(); ++i) {
            Edge& currEdge = activeEdges[i]; 
            int x = GRoundToInt(currEdge.currX); 
            currEdge.currX += currEdge.m; 
            if (w == 0) left = x; 
            w += currEdge.wind; 
            if (w == 0) {
                right = x; 
                blit(left, right, y, fDevice, paint); 
            }
            if (edgeIsOutOfRange(currEdge, y)) {
                activeEdges.erase(activeEdges.begin()+i);
                i--; 
            }
        }
        assert(w == 0); 
        y++; 
    }
}

void clip(GPoint p0, GPoint p1, GRect rect, std::vector<Edge>& edges) {
    int wind = 1; 
    // vertical clip
    if (p0.y() > p1.y()) {
        wind *= -1; 
        std::swap(p0, p1); 
    }

    if (p1.y() <= rect.top() || p0.y() >= rect.bottom() || p0.y() == p1.y()) { // entirely above or below
        return; 
    }

    if (p0.y() < rect.top()) { // edge crosses top, need to crop
        if (p1.x() != p0.x()) {
            p0.fX -= ((p0.x()-p1.x()) * (rect.top()-p0.y())/(p1.y()-p0.y())); 
        }
        p0.fY = rect.top(); 
    }

    if (p1.y() > rect.bottom()) { // edge crosses bottom, need to crop
        if (p1.x() != p0.x()) {
            p1.fX -= ((p1.x()-p0.x())*(p1.y()-rect.bottom())/(p1.y()-p0.y())); 
        }
        p1.fY = rect.bottom(); 
    }

    // horizontal clip
    if (p0.x() > p1.x()) {
        //wind *= -1; 
        std::swap(p0, p1); 
    }
    
    if (p1.x() <= rect.left()) {
        p0.fX = rect.left(); 
        p1.fX = rect.left(); 
        makeEdge(p0, p1, edges, wind); 
        return; 
    }

    if (p0.x() >= rect.right()) { // collapse completely onto right side
        p0.fX = rect.right(); 
        p1.fX = rect.right(); 
        makeEdge(p0, p1, edges, wind); 
        return; 
    }

    if (p0.x() < rect.left()) {
        GPoint tmp; 
        tmp.fX = rect.left(); 
        tmp.fY = p0.y(); 
        p0.fY += ((p1.y()-p0.y())*(rect.left()-p0.x())/(p1.x()-p0.x())); 
        p0.fX = rect.left(); 

        makeEdge(p0, tmp, edges, wind); 
    }

    if (p1.x() > rect.right()) {
        GPoint tmp; 
        tmp.fX = rect.right(); 
        tmp.fY = p1.y(); 
        p1.fY += ((p0.y()-p1.y())*(p1.x()-rect.right())/(p1.x()-p0.x())); 
        p1.fX = rect.right(); 

        makeEdge(p1, tmp, edges, wind); 
    }

    makeEdge(p0, p1, edges, wind); 
}

#endif