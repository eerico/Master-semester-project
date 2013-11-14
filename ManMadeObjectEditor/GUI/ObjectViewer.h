#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#include "../MeshManager.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QColor>

/**
 * @brief The ViewMode enum
 * The different 3D view mode
 */
enum ViewMode {
    wireframe,
    flat,
    smooth,
    point,
    noView // tells that we do not want to show anything
};

/**
 * @brief The ObjectViewer class
 * This class is the window that show the 3D reconstruction.
 */
class ObjectViewer : public QGLViewer
{
    Q_OBJECT
public:
    ObjectViewer(MeshManager* meshManager);

    /**
     * @brief init
     * Initialize the 3D environment
     */
    void init();

    /**
     * @brief draw
     * Set the parameters for the rendering based on the view selected
     * and draw either the points or the triangles or nothing
     */
    void draw();

    /**
     * @brief closeEvent
     * Used to catch the close event and advertise the floor and profile viewer to close itself
     * @param event
     */
    void closeEvent(QCloseEvent *event);

public slots:

    /**
     * @brief wireframeMode
     * Used to advertise this viewer to use the wireframe mode
     */
    void wireframeMode();

    /**
     * @brief flatMode
     * Used to advertise this viewer to use the flat mode
     */
    void flatMode();

    /**
     * @brief smoothMode
     * Used to advertise this viewer to use the smooth mode
     */
    void smoothMode();

    /**
     * @brief pointMode
     * Used to advertise this viewer to use the point mode
     */
    void pointMode();

    /**
     * @brief noViewMode
     * Used to advertise this viewer to use the no view mode
     */
    void noViewMode();

signals:

    /**
     * @brief closeSignal
     * The signal used to advertise the floor and profile viewer that it must close
     */
    void closeSignal();

private:

    /**
     * @brief drawTriangles
     * Draw the triangles retrieved from the mesh manager
     */
    void drawTriangles();

    /**
     * @brief drawPoints
     * Draw the points retrieved from the mesh manager
     */
    void drawPoints();

    // Used to obtain different kind of information about the floor plan and the profiles
    MeshManager* meshManager;

    // the default color of the triangles
    static const qglviewer::Vec defaultColor;

    // the color used when we draw
    qglviewer::Vec color;

    // the selected view mode
    ViewMode vMode;
};


#endif // OBJECTVIEWER_H
