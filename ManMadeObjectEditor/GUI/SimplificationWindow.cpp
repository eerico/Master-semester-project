#include "SimplificationWindow.h"

SimplificationWindow::SimplificationWindow(MeshManager* meshManager)
    :QDialog(0, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint )
    , meshManager(meshManager)
{
    scene = new SimplificationScene(meshManager, &curveArray);
    view = new BasicQGraphicsView(scene);

    gridLayout = new QGridLayout;
    hBoxLayout = new QHBoxLayout;
    this->setLayout(gridLayout);
    gridLayout->addWidget(view, 0, 0);
    gridLayout->addLayout(hBoxLayout, 1, 0);

    okButton = new QPushButton("Ok");
    cancelButton = new QPushButton("Cancel");

    hBoxLayout->addWidget(okButton);
    hBoxLayout->addWidget(cancelButton);

    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    this->setWindowTitle("FloorPlan Simplification");
}

SimplificationWindow::~SimplificationWindow(){
    delete scene;
    delete view;
    delete gridLayout;
    delete hBoxLayout;

    delete okButton;
    delete cancelButton;
}

void SimplificationWindow::ok(){
    // Do/call the simplification algorithm
    // on met l algo dans mesh manager et il va faire comme si un nouveau
    // floorplan a été load. Bien plus simple.
    // On va modifier le floorplan a traver la view et ensuite la reload

    scene->revertColor();

    std::cerr << "array size: " << curveArray.size() << std::endl;
    Simplification simplification(&curveArray, meshManager);
    simplification.simplify();


    meshManager->emitNewFloorPlan();
    close();
}

void SimplificationWindow::cancel(){

    scene->revertColor();
    meshManager->emitNewFloorPlan();
    close();
}

void SimplificationWindow::closeEvent(QCloseEvent *event) {
    emit closeSignal();
    event->accept();
}
