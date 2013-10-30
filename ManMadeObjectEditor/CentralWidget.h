#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QGraphicsScene>
#include <QLabel>
#include <QPalette>
#include <QFont>
#include <QCheckBox>
#include <QSlider>
#include "ProfileScene.h"
#include "FloorScene.h"
#include "Mesh.h"
#include "BasicQGraphicsView.h"
#include "AllPlanScene.h"

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget(QWidget* parent, Mesh* mesh);
    ~CentralWidget();
    
    QGridLayout *layout;
    FloorScene *floorScene;
    BasicQGraphicsView *floorView;
    ProfileScene *profileScene;
    BasicQGraphicsView *profileView;
    QCheckBox* showPlans;
    BasicQGraphicsView *allPlanView;
    AllPlanScene *allPlanScene;
    QSlider *levelSelector;
    
    public slots:
    void changeProfileColorIndication();
    void allPlans();
    void valueSliderChanged(int level);
    void hideAllPlans();
    void uncheckShowPlans();
    
private:
    Mesh* mesh;
    void showAllPlans();
};

#endif // CENTRALWIDGET_H
