#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "ProfileScene.h"
#include "FloorScene.h"
#include "Mesh.h"

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget(Mesh* mesh);
    ~CentralWidget();

    QGridLayout *layout;
    FloorScene *floorScene;
    QGraphicsView *floorView;
    ProfileScene *profileScene;
    QGraphicsView *profileView;
};

#endif // CENTRALWIDGET_H
