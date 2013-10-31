#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#include "MeshManager.h"
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
    ObjectViewer(MeshManager* const meshManager);
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
    MeshManager* meshManager;
    static const qglviewer::Vec defaultColor;
    static const float distanceBetweenSampleForRendering;
    qglviewer::Vec color;
    ViewMode vMode;
    void drawTriangles();
    void drawPoints();
};


#endif // OBJECTVIEWER_H
