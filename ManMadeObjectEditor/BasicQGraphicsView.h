#ifndef BASICQGRAPHICSVIEW_H
#define BASICQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

class BasicQGraphicsView : public QGraphicsView
{
public:
    BasicQGraphicsView(QGraphicsScene* parent = NULL);
protected:
    virtual void wheelEvent(QWheelEvent* event);

};

#endif // BASICQGRAPHICSVIEW_H
