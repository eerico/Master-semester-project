#ifndef CURVE_H
#define CURVE_H

/**
 * @brief The Curve struct
 * This structure represent the portion of the floor plan
 * or the profile that will be simplified
 */
struct Curve{
    Curve(): begin(0), end(0), size(0) {}
    // the first vertex of the curve
    Vertex* begin;
    // the last vertex of the curve
    Vertex* end;
    // the size of the curve
    int size;
    // the color identification of the curve
    QColor color;
};

#endif // CURVE_H
