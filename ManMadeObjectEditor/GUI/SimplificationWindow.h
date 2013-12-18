#ifndef SIMPLIFICATIONWINDOW_H
#define SIMPLIFICATIONWINDOW_H

#include <QDialog>
#include "BasicQGraphicsView.h"
#include "SimplificationScene.h"
#include "SimplificationFloorPlanScene.h"
#include "SimplificationProfileScene.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "../Simplification/Curve.h"
#include "../Simplification/Simplification.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QSlider>
#include "PreviewScene.h"

class SimplificationWindow: public QDialog
{
    Q_OBJECT
public:
    SimplificationWindow(MeshManager* meshManager, bool floorPlanSimplification);
    ~SimplificationWindow();

public slots:
    void ok();
    void cancel();
    void valueSliderChanged(int thresholdValue);

signals:
    void closeSignal();

private:
    SimplificationScene* scene;
    PreviewScene* previewScene;
    BasicQGraphicsView* view;
    BasicQGraphicsView* previewView;
    QGridLayout* gridLayout;
    QHBoxLayout* hBoxLayoutScene;
    QHBoxLayout* hBoxLayout;
    QFormLayout* formLayout;

    QPushButton* okButton;
    QPushButton* cancelButton;

    MeshManager* meshManager;

    void closeEvent(QCloseEvent* event);

    std::vector< Curve* > curveArray;

    //QDoubleSpinBox* thresholdBox;

    QSlider* thresholdSlider;

    bool floorPlanSimplification;

    bool buttonPressed;

    void keyPressEvent(QKeyEvent *e);
};

#endif // SIMPLIFICATIONWINDOW_H
