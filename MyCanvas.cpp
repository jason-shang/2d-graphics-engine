/*
 *  Copyright 2023 Jason Shang
 */

#include "include/GCanvas.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GShader.h"
#include "include/GPath.h"
#include "UtilsConvexPolygon.h"
#include <stack>
#include <cmath>

GShader* GCreateTriTexShader(GShader *shader, GPoint points[3], GPoint texs[3]);
GShader* GCreateComposeShader(GShader* s0, GShader* s1); 
GShader* GCreateTriColorShader(GPoint points[3], GColor colors[3]); 

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device), stack() {
        GMatrix identityMatrix; 
        stack.push(identityMatrix); 
    }

    void drawPaint(const GPaint& paint) override {
        GMatrix ctm = stack.top(); 
        GShader* shader = paint.getShader(); 
        if (shader != nullptr) {
            if (!shader->setContext(ctm)) return; 
        }

        GPoint rectPoints[4] = {
            GPoint{0, 0}, 
            GPoint{float(fDevice.width()), 0}, 
            GPoint{float(fDevice.width()), float(fDevice.height())},
            GPoint{0, float(fDevice.height())},
        }; 

        drawConvexPolygon(rectPoints, 4, paint); 

        // // package given GColor into GPixel
        // GColor color = paint.getColor(); 
        // GPixel pixelColor = MyBlendModes::premul(color.r, color.g, color.b, color.a); 

        // // assign above GPixel to all pixels in bitmap
        // for (int i = 0; i < fDevice.height(); ++i) {
        //     for (int j = 0; j < fDevice.width(); ++j) {
        //         GPixel* pixel = fDevice.getAddr(j, i); 
        //         *pixel = pixelColor; 
        //     }
        // }
    }

    void drawRect(const GRect& rect, const GPaint& paint) override {
        if (noDrawing(paint)) return; 

        GPoint rectPoints[4] = {
            GPoint{rect.left(), rect.top()}, 
            GPoint{rect.right(), rect.top()},
            GPoint{rect.right(), rect.bottom()}, 
            GPoint{rect.left(), rect.bottom()} 
        }; 

        drawConvexPolygon(rectPoints, 4, paint); 
    }

    void drawConvexPolygon(const GPoint originalPoints[], int count, const GPaint& paint) override {
        if (noDrawing(paint)) return; 

        // transform elements in points[] into appropriate coordinate space
        GPoint points[count]; 
        GMatrix ctm = stack.top(); 
        ctm.mapPoints(points, originalPoints, count); 

        GShader* shader = paint.getShader(); 
        if (shader != nullptr) {
            if (!shader->setContext(ctm)) return; 
        }

        // Clip edges of the polygon, store clipped edges
        std::vector<Edge> edges;
        edges.reserve(count*2);
        GRect rect = { 0, 0, fDevice.width(), fDevice.height() }; 
        for (int i = 0; i < count-1; ++i) {
            clip(points[i], points[i+1], rect, edges); 
        }
        clip(points[count-1], points[0], rect, edges); 

        // Cast Rays
        castRays(edges, fDevice, paint); 
    }

    void save() override {
        stack.push(stack.top()); 
    }

    void restore() override {
        if (!stack.empty()) {
            stack.pop(); 
        }
    }

    void concat(const GMatrix& matrix) override {
        if (!stack.empty()) {
            GMatrix top = stack.top() * matrix; 
            stack.top() = top; 
        }
    }

    void drawPath(const GPath& path, const GPaint& paint) override {
        if (noDrawing(paint)) return; 

        GMatrix ctm = stack.top(); 

        GShader* shader = paint.getShader(); 
        if (shader != nullptr) {
            if (!shader->setContext(ctm)) return; 
        }

        // cast rays through the edges, keep track of winds, and blit
        GPoint pts[GPath::kMaxNextPoints];
        GPath::Edger iter(path);
        GPath::Verb v;
        std::vector<Edge> edges; // store clipped edges

        GRect rect = { 0, 0, fDevice.width(), fDevice.height() }; 

        while ((v = iter.next(pts)) != GPath::kDone) {
            // transform points in path into appropriate coordinate space
            ctm.mapPoints(pts, pts, GPath::kMaxNextPoints); 
            switch (v) {
                case GPath::kLine: {
                    clip(pts[0], pts[1], rect, edges);
                    break;
                } case GPath::kQuad: {
                    // set up (coefficients) for Horner's rule
                    GPoint a = pts[0] - 2 * pts[1] + pts[2];
                    GPoint b = 2 * ((-1) * pts[0] + pts[1]);
                    GPoint c = pts[0];

                    GPoint error = 0.25 * a;
                    int numSegments = GCeilToInt(sqrt(error.length()*4.0f)); // multiply by 4 bc we're using tolerance = 0.25
                    float dt = 1.0f / numSegments;

                    GPoint p0 = pts[0];
                    GPoint p1;
                    for (float t = dt; t < 1.0f; t += dt) {
                        p1 = (a * t + b) * t + c;
                        clip(p0, p1, rect, edges);
                        p0 = p1;
                    }
                    p1 = pts[2]; 
                    clip(p0, p1, rect, edges);
                    break;
                } case GPath::kCubic: {
                    GPoint e0 = pts[0] + 2*pts[1] + pts[2];
                    GPoint e1 = pts[1] + 2*pts[2] + pts[3];
                    GPoint error = {std::max(abs(e0.x()), abs(e1.x())), std::max(abs(e0.y()), abs(e1.y()))};
                    int numSegments = GCeilToInt(sqrt(3.0f * error.length()));
                    float dt = 1.0f / numSegments;

                    // set up for Horner's rule
                    GPoint a = (-1) * pts[0] + 3 * pts[1] + (-3) * pts[2] + pts[3];
                    GPoint b = 3 * pts[0] + (-6) * pts[1] + 3 * pts[2];
                    GPoint c = 3 * (pts[1] + (-1) * pts[0]);
                    GPoint d = pts[0];

                    GPoint p0 = pts[0];
                    GPoint p1;
                    for (float t = dt; t < 1.0f; t += dt) {
                        p1 = ((a * t + b) * t + c) * t + d;
                        clip(p0, p1, rect, edges);
                        p0 = p1;
                    }
                    p1 = pts[3]; 
                    clip(p0, p1, rect, edges);
                    break;
                } default: {
                    break;
                }
            }
        }

        castRaysComplex(edges, fDevice, paint); 
    }

    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint) override {
        if (colors == nullptr && texs == nullptr) { return; }

        int n = 0; 
        GPoint myVerts[3]; 
        GPoint myTexs[3]; 
        GColor myColors[3]; 
        
        if (colors != nullptr && texs != nullptr) {
            for (int i = 0; i < count; ++i) {
                myVerts[0] = verts[indices[n]]; 
                myVerts[1] = verts[indices[n+1]]; 
                myVerts[2] = verts[indices[n+2]]; 

                myColors[0] = colors[indices[n]]; 
                myColors[1] = colors[indices[n+1]]; 
                myColors[2] = colors[indices[n+2]]; 

                myTexs[0] = texs[indices[n]]; 
                myTexs[1] = texs[indices[n+1]]; 
                myTexs[2] = texs[indices[n+2]]; 

                GShader* triColorShader = GCreateTriColorShader(myVerts, myColors); 
                GShader* triTexShader = GCreateTriTexShader(paint.getShader(), myVerts, myTexs); 
                GShader* combinedShader = GCreateComposeShader(triColorShader, triTexShader); 

                drawConvexPolygon(myVerts, 3, GPaint(combinedShader)); 

                n += 3; 
            }
        } else if (colors != nullptr) {
            for (int i = 0; i < count; ++i) {
                myVerts[0] = verts[indices[n]]; 
                myVerts[1] = verts[indices[n+1]]; 
                myVerts[2] = verts[indices[n+2]]; 

                myColors[0] = colors[indices[n]]; 
                myColors[1] = colors[indices[n+1]]; 
                myColors[2] = colors[indices[n+2]]; 

                GShader* triColorShader = GCreateTriColorShader(myVerts, myColors); 
                drawConvexPolygon(myVerts, 3, GPaint(triColorShader)); 

                n += 3; 
            }
        } else if (texs != nullptr) {
            for (int i = 0; i < count; ++i) {
                myVerts[0] = verts[indices[n]]; 
                myVerts[1] = verts[indices[n+1]]; 
                myVerts[2] = verts[indices[n+2]]; 

                myTexs[0] = texs[indices[n]]; 
                myTexs[1] = texs[indices[n+1]]; 
                myTexs[2] = texs[indices[n+2]]; 

                GShader* triTexShader = GCreateTriTexShader(paint.getShader(), myVerts, myTexs); 
                drawConvexPolygon(myVerts, 3, GPaint(triTexShader)); 

                n += 3; 
            }
        } else {
            return; 
        }
    }

    GPoint calculateMeshPoint(GPoint A,GPoint B,GPoint C,GPoint D,float u,float v){
        return (1-u)*(1-v)*A + (1-v)*u*B + u*v*C + (1-u)*v*D; 
    }

    GColor calculateNewColor(GColor A, GColor B, GColor C, GColor D, float u, float v){
        return (1-u)*(1-v)*A + (1-v)*u*B + u*v*C + (1-u)*v*D; 
    }

    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint& paint) override {
        if (colors == nullptr && texs == nullptr) { return; }

        GPoint A = verts[0];
        GPoint B = verts[1];
        GPoint C = verts[2];
        GPoint D = verts[3];

        // ALTERNATIVE IMPLEMENTATION
        // to specify drawing order of verts in order to produce the triangles (see diagram in GCanvas.h)
        // int indices[6] = {0, 1, 3, 1, 2, 3}; 
        // int n = level + 2; // number of points to draw in each dimension
        // int numTriangles = 2 * (level+1) * (level+1); // total number of triangles to draw
        // GPoint meshPoints[n*n]; // store all the mesh points
        // GColor newColors[n*n]; 
        // GPoint newTexs[n*n]; 

        // float du = 1.0f / (float)(level + 1);
        // float dv = 1.0f / (float)(level + 1);
        // float u = 0;
        // float v = 0;

        // if (colors != nullptr && texs != nullptr) {
        //     float coefA, coefB, coefC, coefD; 
        //     for (int i = 0; i < n; ++i) {
        //         for (int j = 0; j < n; ++j) {
        //             coefA = (1-u)*(1-v); 
        //             coefB = u*(1-v); 
        //             coefC = u*v; 
        //             coefD = (1-u)*v; 
        //             meshPoints[i*n + j] = coefA*A + coefB*B + coefC*C + coefD*D; 
        //             newColors[i*n + j] = coefA*colors[0] + coefB*colors[1] + coefC*colors[2] + coefD*colors[3]; 
        //             newTexs[i*n + j] = coefA*texs[0] + coefB*texs[1] + coefC*texs[2] + coefD*texs[3]; 

        //             v += dv; 
        //         }
        //         u += du; 
        //     }

        //     drawMesh(meshPoints, newColors, newTexs, numTriangles, indices); 
        // }

        GPoint newVerts[4];
        int indices[6] = {0,1,3,1,2,3}; // specify drawing order of verts in order to produce the triangles (see diagram in GCanvas.h)

        for (int u = 0; u <= level; ++u) {
            for (int v = 0; v <= level; ++v) {
                float u1 = (float)u/(float)(level+1);
                float u2 = (float)(u+1)/(float)(level+1);

                float v1 = (float)v/(float)(level+1);
                float v2 = (float)(v+1)/(float)(level+1);

                newVerts[0]=calculateMeshPoint(A, B, C, D, u1, v1);
                newVerts[1]=calculateMeshPoint(A, B, C, D, u2, v1);
                newVerts[2]=calculateMeshPoint(A, B, C, D, u2, v2);
                newVerts[3]=calculateMeshPoint(A, B, C, D, u1, v2);

                GColor new_colors[4];
                GColor *cp = nullptr;
                GPoint new_texs[4];
                GPoint *pc = nullptr;

                if (colors != nullptr) {
                    new_colors[0] = calculateNewColor(colors[0], colors[1],colors[2],colors[3], u1, v1);
                    new_colors[1] = calculateNewColor(colors[0], colors[1],colors[2],colors[3], u2, v1);
                    new_colors[2] = calculateNewColor(colors[0], colors[1],colors[2],colors[3], u2, v2);
                    new_colors[3] = calculateNewColor(colors[0], colors[1],colors[2],colors[3], u1, v2);

                    cp = new_colors;
                }

                if (texs != nullptr) {
                    new_texs[0] = calculateMeshPoint(texs[0], texs[1], texs[2], texs[3], u1, v1);
                    new_texs[1] = calculateMeshPoint(texs[0], texs[1], texs[2], texs[3], u2, v1);
                    new_texs[2] = calculateMeshPoint(texs[0], texs[1], texs[2], texs[3], u2, v2);
                    new_texs[3] = calculateMeshPoint(texs[0], texs[1], texs[2], texs[3], u1, v2);

                    pc = new_texs;
                }

                drawMesh(newVerts, cp, pc, 2, indices, paint);
            }
        }

        return; 
    }

private:
    // Note: we store a copy of the bitmap
    const GBitmap fDevice;
    std::stack<GMatrix> stack;
};

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    // canvas->clear({1,1,1,1});

    // GPaint paint;
    // paint.setBlendMode(GBlendMode::kSrc);
    // GPoint pts[3]; 
    // pts[0] = {128, 50}; 
    // pts[1] = {91, 100}; 
    // pts[2] = {165, 100}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(0.1568, 0.1529, 0.1490, 1)); 
    
    // pts[0] = {128, 50}; 
    // pts[1] = {91, 100}; 
    // pts[2] = {75, 50}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(0.40, 0.40, 0.37, 1)); 

    // pts[0] = {128, 50}; 
    // pts[1] = {181, 50}; 
    // pts[2] = {165, 100}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(0.3803, 0.3843, 0.3569, 1)); 
    
    // pts[0] = {40, 90}; 
    // pts[1] = {91, 100}; 
    // pts[2] = {75, 50}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(0.1568, 0.1529, 0.1490, 1)); 

    // pts[0] = {181, 50}; 
    // pts[1] = {165, 100}; 
    // pts[2] = {216, 90}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(0.8745, 0.8431, 0.8274, 1)); 
    
    // pts[0] = {128, 210}; 
    // pts[1] = {40, 90}; 
    // pts[2] = {91, 100}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(0.87, 0.74, 0.6823, 1)); 
    
    // pts[0] = {91, 100}; 
    // pts[1] = {165, 100}; 
    // pts[2] = {128, 210}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(0.6157, 0.6196, 0.5961, 1)); 

    // pts[0] = {165, 100}; 
    // pts[1] = {216, 90}; 
    // pts[2] = {128, 210}; 
    // canvas->drawConvexPolygon(pts, 3, paint.setRGBA(1, 0.1, 0.1, 0.1)); 

    return "Diamond"; 
}