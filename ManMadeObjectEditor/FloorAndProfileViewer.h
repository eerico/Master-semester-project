#ifndef FLOORANDPROFILEVIEWER_H
#define FLOORANDPROFILEVIEWER_H

#include <QApplication>
#include <QMainWindow>
#include "CentralWidget.h"
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include "ObjectViewer.h"
#include "Mesh.h"
#include "ObjectViewer.h"

class FloorAndProfileViewer : public QMainWindow
{
    Q_OBJECT
public:
    FloorAndProfileViewer();
    ~FloorAndProfileViewer();
    void closeEvent(QCloseEvent* event);
    void createMenuBar();
    void createFileMenu();
    void createViewMenu();
    void createAboutMenu();

public slots:
    void closeViewer();
    void openFile();
    void aboutQtMessageBox();

signals:
    void closeSignal();

private:
    CentralWidget* centralWidget;
    ObjectViewer* objViewer;
    Mesh* mesh;
    QMenu* fileMenu;
    QAction* openAction;
    QAction* exitAction;
    QMenu* aboutMenu;
    QAction* aboutQtAction;
    QMenu* viewMenu;
    QAction* wireframeAction;
    QAction* flatAction;
    QAction* smoothAction;
    QAction* pointAction;
    QAction* pointSampledAction;
};

#endif // FLOORANDPROFILEVIEWER_H
