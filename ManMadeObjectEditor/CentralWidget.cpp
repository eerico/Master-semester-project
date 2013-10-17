#include "CentralWidget.h"

CentralWidget::CentralWidget(Mesh* mesh) :
    QWidget()
{
    layout = new QGridLayout();

    floorScene = new FloorScene(mesh);
<<<<<<< HEAD
    floorView = new QGraphicsView(floorScene);
    layout->addWidget(floorView, 0, 0);

    profileScene = new ProfileScene(mesh);
    profileView = new QGraphicsView(profileScene);
=======
    floorView = new BasicQGraphicsView(floorScene);

    layout->addWidget(floorView, 0, 0);

    profileScene = new ProfileScene(mesh);
    profileView = new BasicQGraphicsView(profileScene);
>>>>>>> Erico
    layout->addWidget(profileView, 0, 1);

    QObject::connect(floorScene, SIGNAL(newProfileSelected()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(mesh, SIGNAL(newFloorPlan()), floorScene, SLOT(loadFloorPlan()));
    QObject::connect(mesh, SIGNAL(newFloorPlan()), profileScene, SLOT(newProfileSelected()));

    this->setLayout(layout);
    this->setMinimumSize(QSize(300, 170));
}

CentralWidget::~CentralWidget()
{
    delete layout;
    delete floorScene;
    delete floorView;
    delete profileScene;
    delete profileView;
}
