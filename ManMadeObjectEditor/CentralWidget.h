#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QGraphicsScene>
#include "ProfileScene.h"
#include "FloorScene.h"
#include "Mesh.h"
#include "BasicQGraphicsView.h"

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget(Mesh* mesh);
    ~CentralWidget();

    QGridLayout *layout;
    FloorScene *floorScene;
    BasicQGraphicsView *floorView;
    ProfileScene *profileScene;
    BasicQGraphicsView *profileView;
};

#endif // CENTRALWIDGET_H
