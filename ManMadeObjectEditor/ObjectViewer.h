#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#include "Mesh.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QColor>

enum ViewMode {
    wireframe,
    flat,
    smooth,
    point,
    pointSampled
};

class ObjectViewer : public QGLViewer
{
    Q_OBJECT
public:
    ObjectViewer(Mesh* const mesh);
    void init();
    void draw();
    //void animate();
    void closeEvent(QCloseEvent *event);

public slots:
    void closeViewer();
    void wireframeMode();
    void flatMode();
    void smoothMode();
    void pointMode();
    void pointSampledMode();

signals:
    void closeSignal();

private:
    Mesh* mesh;
    static const qglviewer::Vec defaultColor;
    static const float distanceBetweenSampleForRendering;
    qglviewer::Vec color;
    ViewMode vMode;
    void drawTriangles();
    void drawPoints(bool sampledPoint);
};


#endif // OBJECTVIEWER_H
