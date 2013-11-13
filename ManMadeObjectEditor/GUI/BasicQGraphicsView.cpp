#include "BasicQGraphicsView.h"

BasicQGraphicsView::BasicQGraphicsView(QGraphicsScene* const parent): QGraphicsView(parent)
{
    //Use ScrollHand Drag Mode to enable Panning
    setDragMode(ScrollHandDrag);
}

void BasicQGraphicsView::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zooming out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
}

void BasicQGraphicsView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void BasicQGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void BasicQGraphicsView::enterEvent(QEvent *event) {
    QGraphicsView::enterEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}
