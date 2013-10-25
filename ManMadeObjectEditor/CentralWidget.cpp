#include "CentralWidget.h"

CentralWidget::CentralWidget(Mesh* mesh) :
    QWidget(), mesh(mesh)
{
    layout = new QGridLayout();

    floorScene = new FloorScene(mesh);
    floorView = new BasicQGraphicsView(floorScene);

    layout->addWidget(floorView, 0, 0);

    profileScene = new ProfileScene(mesh);
    profileView = new BasicQGraphicsView(profileScene);
    layout->addWidget(profileView, 0, 1);

    profileSelectedLabel = new QLabel("Profile Selected");
    profileSelectedLabel->setAlignment(Qt::AlignCenter);
    profileSelectedLabel->setAutoFillBackground(true);
    QFont labelFont;
    labelFont.setBold(true);
    profileSelectedLabel->setFont(labelFont);
    layout->addWidget(profileSelectedLabel, 1, 1);



    QObject::connect(floorScene, SIGNAL(newProfileSelected()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(mesh, SIGNAL(newFloorPlan()), floorScene, SLOT(loadFloorPlan()));
    QObject::connect(mesh, SIGNAL(newFloorPlan()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), this, SLOT(changeProfileColorIndication()));

    this->setLayout(layout);
    this->setMinimumSize(QSize(300, 170));
}

void CentralWidget::changeProfileColorIndication()
{
    QPalette palette = profileSelectedLabel->palette();
    palette.setColor(QPalette::Window, *mesh->getCurrentProfile()->getProfileColorIdentification());
    profileSelectedLabel->setPalette(palette);
}

CentralWidget::~CentralWidget()
{
    delete layout;
    delete floorScene;
    delete floorView;
    delete profileScene;
    delete profileView;
    delete profileSelectedLabel;
}
