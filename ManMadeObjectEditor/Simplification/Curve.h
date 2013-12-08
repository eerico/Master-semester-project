#ifndef CURVE_H
#define CURVE_H

struct Curve{
    Curve(): begin(0), end(0), size(0) {}
    Vertex* begin;
    Vertex* end;
    int size;
    QColor color;
};

#endif // CURVE_H
