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
};

class ObjectViewer : public QGLViewer
{
    Q_OBJECT
public:
    ObjectViewer(Mesh* const mesh);
    void init();
    void draw();
    void closeEvent(QCloseEvent *event);

public slots:
    void closeViewer();
    void wireframeMode();
    void flatMode();
    void smoothMode();
    void pointMode();

signals:
    void closeSignal();

private:
    Mesh* mesh;
    static const qglviewer::Vec defaultColor;
    static const float distanceBetweenSampleForRendering;
    qglviewer::Vec color;
    ViewMode vMode;
    void drawTriangles();
    void drawPoints();
};


#endif // OBJECTVIEWER_H
