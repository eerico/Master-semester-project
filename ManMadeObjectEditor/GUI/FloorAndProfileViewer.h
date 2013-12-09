#ifndef FLOORANDPROFILEVIEWER_H
#define FLOORANDPROFILEVIEWER_H

#include <QApplication>
#include <QMainWindow>
#include "../GUI/CentralWidget.h"
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include "../GUI/ObjectViewer.h"
#include "../MeshManager.h"
#include <QFileInfo>
#include "SimplificationWindow.h"
#include "ProfileMergeWindow.h"

/**
 * @brief The FloorAndProfileViewer class
 * This class is our main window that contain the central widget and that create the object viewer
 */
class FloorAndProfileViewer : public QMainWindow
{
    Q_OBJECT
public:
    FloorAndProfileViewer();
    ~FloorAndProfileViewer();

    /**
     * @brief closeEvent
     * Used to catch the close event and advertise the object viewer to close itself
     * @param event
     */
    void closeEvent(QCloseEvent* event);

    /**
     * @brief createMenuBar
     * Create the menu bar
     */
    void createMenuBar();

    /**
     * @brief createFileMenu
     * Create the file menu
     */
    void createFileMenu();

    /**
     * @brief createViewMenu
     * Create the view menu
     */
    void createViewMenu();

    /**
     * @brief createAboutMenu
     * Create the about menu
     */
    void createAboutMenu();

    void createEditMenu();

public slots:
    /**
     * @brief openFile
     * Open file action that allow the user to read an external .obj
     */
    void openFile();

    /**
     * @brief clearFile
     * Action made when the user use the clear option. It will reset the program to its
     * inital state
     */
    void clearFile();

    /**
     * @brief aboutQtMessageBox
     * Shows the about Qt message box
     */
    void aboutQtMessageBox();

    void createSimplificationFloorPlanWindow();

    void createSimplificationProfileWindow();

    void closeSimplificationWindow();

    void createProfileMergeWindow();

    void closeProfileMergeWindow();

    void  saveXML();

signals:
    /**
     * @brief closeSignal
     * The signal used to advertise the object viewer that it must close
     */
    void closeSignal();

private:
    CentralWidget* centralWidget;
    ObjectViewer* objViewer;
    MeshManager* meshManager;

    // the different menu and action
    QMenu* fileMenu;
    QAction* openAction;
    QAction* saveAction;
    QAction* clearAction;
    QAction* exitAction;

    QMenu* aboutMenu;
    QAction* aboutQtAction;

    // all view actions that allow the user to switch between different
    // 3D representation
    QMenu* viewMenu;

    // tells the object viewer to show the wireframe
    QAction* wireframeAction;

    // tells the object viewer to show a flat representation
    QAction* flatAction;

    // tells the object viewer to show a smooth representation
    QAction* smoothAction;

    // tells the object viewer to show only the vertices
    QAction* pointAction;

    // tells the object viewer to do not show any 3D representation
    QAction* noViewAction;

    QMenu* editMenu;
    QAction* addNewProfileAction;
    QAction* floorPlanSimplificationAction;
    QAction* profileSimplificationAction;
    QAction* profileMergeAction;

    SimplificationWindow* simplificationWindow;
    ProfileMergeWindow* profileMergeWindow;
};

#endif // FLOORANDPROFILEVIEWER_H
