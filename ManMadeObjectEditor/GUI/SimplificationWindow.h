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

    QPushButton* okButton;
    QPushButton* cancelButton;

    MeshManager* meshManager;

    void closeEvent(QCloseEvent* event);

    std::vector< Curve* > curveArray;
};

#endif // SIMPLIFICATIONWINDOW_H
