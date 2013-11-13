#ifndef BASICQGRAPHICSVIEW_H
#define BASICQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

/**
 * @brief The BasicQGraphicsView class
 * This class represents a view that will contain a scene. This vew
 * allow the user to zoom
 */
class BasicQGraphicsView : public QGraphicsView
{
public:
    BasicQGraphicsView(QGraphicsScene* const parent = NULL);
protected:

    /**
     * @brief wheelEvent
     * This method handle the mouse wheel event. It allow the user
     * to zoom
     * @param event The wheel event
     */
    virtual void wheelEvent(QWheelEvent* event);

    /**
     * @brief mousePressEvent
     * This method allow use to redefine the cursor icon
     * @param event
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief mouseReleaseEvent
     * This method allow use to redefine the cursor icon
     * @param event
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * @brief enterEvent
     * This method allow use to redefine the cursor icon
     * @param event
     */
    void enterEvent(QEvent *event);
};

#endif // BASICQGRAPHICSVIEW_H
