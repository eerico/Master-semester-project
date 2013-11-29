#include "FloorAndProfileViewer.h"

FloorAndProfileViewer::FloorAndProfileViewer()
    : QMainWindow()
{
    // all main data will be computed and accessed through this object
    meshManager = new MeshManager();

    // create the object viewer
    objViewer = new ObjectViewer(meshManager);
    objViewer->setWindowTitle("Object Viewer");
    objViewer->show();

    // create the central widget
    centralWidget = new CentralWidget(this, meshManager);
    this->setCentralWidget(centralWidget);

    createMenuBar();

    // if we close the window, tells the object viewer to close itself
    QObject::connect(this, SIGNAL(closeSignal()), objViewer, SLOT(close()));
    // if the object viewer is closed, then we close this window
    QObject::connect(objViewer, SIGNAL(closeSignal()), this, SLOT(close()));
}

FloorAndProfileViewer::~FloorAndProfileViewer()
{
    delete centralWidget;
    delete openAction;
    delete saveAction;
    delete clearAction;
    delete exitAction;
    delete aboutQtAction;
    delete wireframeAction;
    delete flatAction;
    delete smoothAction;
    delete pointAction;
    delete noViewAction;
    delete meshManager;
    delete objViewer;
}

void FloorAndProfileViewer::closeEvent(QCloseEvent *event) {
    emit closeSignal();
    event->accept();
}

void FloorAndProfileViewer::createMenuBar() {
    // create all menu in the menu bar
    createFileMenu();
    createViewMenu();
    createAboutMenu();
}

void FloorAndProfileViewer::createFileMenu() {
    fileMenu = this->menuBar()->addMenu(tr("&File"));

    openAction = new QAction(tr("&Open"), this);
    openAction->setShortcut(tr("Ctrl+O"));
    fileMenu->addAction(openAction);
    QObject::connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    fileMenu->addAction(saveAction);
    QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(saveXML()));

   //maybe more interesting to name it new....
    clearAction = new QAction(tr("&Clear"), this);
    clearAction->setShortcut(tr("Ctrl+N"));
    fileMenu->addAction(clearAction);
    QObject::connect(clearAction, SIGNAL(triggered()), this, SLOT(clearFile()));


    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcut(tr("Esc"));
    fileMenu->addAction(exitAction);
    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void FloorAndProfileViewer::createAboutMenu() {
    aboutMenu = this->menuBar()->addMenu(tr("&About"));

    aboutQtAction = new QAction(tr("&About Qt"), this);
    aboutMenu->addAction(aboutQtAction);
    QObject::connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQtMessageBox()));
}

void FloorAndProfileViewer::createViewMenu() {
    viewMenu = this->menuBar()->addMenu(tr("&View"));

    wireframeAction = new QAction(tr("&Wireframe"), this);
    viewMenu->addAction(wireframeAction);
    QObject::connect(wireframeAction, SIGNAL(triggered()), objViewer, SLOT(wireframeMode()));

    flatAction = new QAction(tr("&Flat"), this);
    viewMenu->addAction(flatAction);
    QObject::connect(flatAction, SIGNAL(triggered()), objViewer, SLOT(flatMode()));

    smoothAction = new QAction(tr("&Smooth"), this);
    viewMenu->addAction(smoothAction);
    QObject::connect(smoothAction, SIGNAL(triggered()), objViewer, SLOT(smoothMode()));

    pointAction = new QAction(tr("&Point"), this);
    viewMenu->addAction(pointAction);
    QObject::connect(pointAction, SIGNAL(triggered()), objViewer, SLOT(pointMode()));

    noViewAction = new QAction(tr("&No View"), this);
    viewMenu->addAction(noViewAction);
    QObject::connect(noViewAction, SIGNAL(triggered()), objViewer, SLOT(noViewMode()));
}

void FloorAndProfileViewer::aboutQtMessageBox() {
    QMessageBox::aboutQt(this);
}


void FloorAndProfileViewer::openFile() {
    QString file = QFileDialog::getOpenFileName(this, "Open File", QString(), "Mesh Files (*.obj *.off *.stl *.xml)");

    if (file.isEmpty()) {
        return;
    }
    QFileInfo fi(file);
    if(fi.suffix().toLower() =="xml"){
        Utils::readXML(meshManager, file);
    } else {

    // by default, we do not show all the floor plan
    centralWidget->hideAllPlans();
    centralWidget->uncheckShowPlans();

    // tells the mesh manager that a new mesh can be read
    meshManager->loadMesh(file);
    }
}

void FloorAndProfileViewer::saveXML() {
    QString filename = QFileDialog::getSaveFileName(this,
                                            tr("Save Xml"), ".",
                                            tr("Xml files (*.xml)"));
    Utils::CreateXMLFile(meshManager, filename);
}

void FloorAndProfileViewer::clearFile() {
    this->hide();
    QObject::disconnect(objViewer, SIGNAL(closeSignal()), this, SLOT(close()));
    emit closeSignal();
    QObject::disconnect(this, SIGNAL(closeSignal()), objViewer, SLOT(close()));

    MeshManager* meshManager2 = new MeshManager();
    objViewer = new ObjectViewer(meshManager2);
    objViewer->setWindowTitle("Object Viewer");
    objViewer->show();
    CentralWidget* newCentralWidget = new CentralWidget(this, meshManager2);
    this->setCentralWidget(newCentralWidget);

    ProfileDestructorManager::deleteProfiles();

    this->menuBar()->clear();
    delete openAction;
    delete clearAction;
    delete exitAction;
    delete aboutQtAction;
    delete wireframeAction;
    delete flatAction;
    delete smoothAction;
    delete pointAction;
    delete noViewAction;
    createMenuBar();

    delete centralWidget;
    delete meshManager;
    centralWidget = newCentralWidget;
    meshManager = meshManager2;

    this->show();
    QObject::connect(this, SIGNAL(closeSignal()), objViewer, SLOT(close()));
    QObject::connect(objViewer, SIGNAL(closeSignal()), this, SLOT(close()));
}
