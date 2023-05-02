#include "include/GPath.h"

void GPath::addRect(const GRect& rect, Direction dir) {
    moveTo(rect.left(), rect.top()); 
    if (dir == kCW_Direction) {
        lineTo( {rect.right(), rect.top()} );
        lineTo( {rect.right(), rect.bottom()} ); 
        lineTo( {rect.left(), rect.bottom()} ); 
    } else {
        lineTo( {rect.left(), rect.bottom()} ); 
        lineTo( {rect.right(), rect.bottom()} ); 
        lineTo( {rect.right(), rect.top()} );
    }
}

void GPath::addPolygon(const GPoint pts[], int count) {
    // assert(sizeof(*pts) == count*sizeof(GPoint)); 
    if (count == 0) return; 
    moveTo(pts[0].x(), pts[0].y()); 
    for (int i = 1; i < count; i++) {
        lineTo(pts[i]); 
    }
}

GRect GPath::bounds() const {
    if (fPts.empty()) return GRect::LTRB(0.0, 0.0, 0.0, 0.0); 

    float maxX, maxY = FLT_MIN; 
    float minX, minY = FLT_MAX; 
    for (int i = 0; i < fPts.size(); i++) {
        GPoint cur = fPts[i]; 
        if (cur.x() < minX) minX = cur.x(); 
        if (cur.x() > maxX) maxX = cur.x(); 
        if (cur.y() < minY) minY = cur.y(); 
        if (cur.y() > maxY) maxY = cur.y(); 
    }

    return GRect::LTRB(minX, minY, maxX, maxY); 
}

void GPath::transform(const GMatrix& matrix) {
    if (fPts.empty()) return; 
    matrix.mapPoints(&fPts[0], &fPts[0], fPts.size()); 
}

void GPath::addCircle(GPoint center, float radius, Direction dir) {
    // unit circle
    float tanVal = tanf(3.1415926/8.0); 
    float sqrtVal = sqrt(2)/2.0; 
    GPoint unitCirclePts[16] = {
        {1, 0}, {1, tanVal}, 
        {sqrtVal, sqrtVal}, {tanVal, 1}, 
        {0, 1}, {-tanVal, 1}, 
        {-sqrtVal, sqrtVal}, {-1, tanVal}, 
        {-1, 0}, {-1, -tanVal}, 
        {-sqrtVal, -sqrtVal}, {-tanVal, -1}, 
        {0, -1}, {tanVal, -1},
        {sqrtVal, -sqrtVal}, {1, -tanVal} 
    }; 

    // transform unit circle into current circle
    GPoint points[16]; 
    GMatrix m = GMatrix::Translate(center.x(), center.y()) * GMatrix::Scale(radius, radius);
    m.mapPoints(points, unitCirclePts, 16); 

    moveTo(points[0]); 
    if (dir == kCW_Direction) {
        for (int i = 1; i < 15; i += 2) {
            quadTo(points[i].x(), points[i].y(), points[i+1].x(), points[i+1].y()); 
        }
        quadTo(points[15].x(), points[15].y(), points[0].x(), points[0].y()); 
    } else {
        for (int i = 15; i > 1; i -= 2) {
            quadTo(points[i].x(), points[i].y(), points[i-1].x(), points[i-1].y()); 
        }
        quadTo(points[1].x(), points[1].y(), points[0].x(), points[0].y()); 
    }
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    dst[0] = src[0];
    dst[1] = (1-t)*src[0] + t*src[1];
    dst[3] = (1-t)*src[1] + t*src[2];
    dst[2] = (1-t)*dst[1] + t*dst[3];
    dst[4] = src[2];
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    dst[0] = src[0];
    dst[1] = (1 - t) * src[0] + t * src[1];
    dst[5] = (1 - t) * src[2] + t * src[3];
    dst[6] = src[3];

    GPoint b = (1 - t) * src[1] + t * src[2];
    dst[2] = (1 - t) * dst[1] + t * b;
    dst[4] = (1 - t) * b + t * dst[5];
    dst[3] = (1 - t) * dst[2] + t * dst[4];
}
