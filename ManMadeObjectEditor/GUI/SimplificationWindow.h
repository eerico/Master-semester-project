#ifndef SIMPLIFICATIONWINDOW_H
#define SIMPLIFICATIONWINDOW_H
#include <QDialog>
#include "BasicQGraphicsView.h"
#include "SimplificationScene.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "../Simplification/Curve.h"
#include "../Simplification/Simplification.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>

class SimplificationWindow: public QDialog
{
    Q_OBJECT
public:
    SimplificationWindow(MeshManager* meshManager);
    ~SimplificationWindow();

public slots:
    void ok();
    void cancel();

signals:
    void closeSignal();

private:
    SimplificationScene* scene;
    BasicQGraphicsView* view;
    QGridLayout* gridLayout;
    QHBoxLayout* hBoxLayout;
    QFormLayout* formLayout;

    QPushButton* okButton;
    QPushButton* cancelButton;

    MeshManager* meshManager;

    void closeEvent(QCloseEvent* event);

    std::vector< Curve* > curveArray;

    QDoubleSpinBox* thresholdBox;
    QLabel* thresholdLabel;
};

#endif // SIMPLIFICATIONWINDOW_H
