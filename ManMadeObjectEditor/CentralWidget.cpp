#include "CentralWidget.h"

CentralWidget::CentralWidget(Mesh* mesh) :
    QWidget(), mesh(mesh), allPlanView(0), allPlanScene(0), levelSelector(0)
{
    layout = new QGridLayout();
    
    floorScene = new FloorScene(mesh);
    floorView = new BasicQGraphicsView(floorScene);
    
    layout->addWidget(floorView, 0, 0);
    
    profileScene = new ProfileScene(mesh);
    profileView = new BasicQGraphicsView(profileScene);
    layout->addWidget(profileView, 0, 1);

    showPlans = new QCheckBox("Show all plans");
    layout->addWidget(showPlans, 1, 0);
    
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(mesh, SIGNAL(newFloorPlan()), floorScene, SLOT(loadFloorPlan()));
    QObject::connect(mesh, SIGNAL(newFloorPlan()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(mesh, SIGNAL(newFloorPlan()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(showPlans, SIGNAL(clicked()), this, SLOT(allPlans()));

    this->setLayout(layout);
    this->setMinimumSize(QSize(300, 170));
}

void CentralWidget::changeProfileColorIndication()
{
    this->setAutoFillBackground(true);
    QPalette palette(this->palette());
    palette.setColor(QPalette::Window, *mesh->getCurrentProfile()->getProfileColorIdentification());
    this->setPalette(palette);
    
}

void CentralWidget::valueSliderChanged(int level)
{
    allPlanScene->loadPlan(level);
}

void CentralWidget::allPlans()
{
    if(showPlans->isChecked()) {
        showAllPlans();
    } else {
        hideAllPlans();
    }
}

void CentralWidget::showAllPlans()
{
    if (mesh->getPlans().size() == 0) {
        return;
    }

    allPlanScene = new AllPlanScene(mesh);
    allPlanView = new BasicQGraphicsView(allPlanScene);
    layout->addWidget(allPlanView, 0, 2);
    levelSelector = new QSlider(Qt::Horizontal);

    levelSelector->setMinimum(0);
    levelSelector->setMaximum(mesh->getPlans().size() - 1);
    layout->addWidget(levelSelector, 1, 2);

    allPlanScene->loadPlan(levelSelector->value());

    QObject::connect(levelSelector, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChanged(int)));

}

void CentralWidget::hideAllPlans()
{
    if(allPlanScene != 0)  {
        delete allPlanScene;
        allPlanScene = 0;
    }

    if(allPlanView != 0) {
        delete allPlanView;
        allPlanView = 0;
    }

    if(levelSelector != 0) {
        QObject::disconnect(levelSelector, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChanged(int)));

        delete levelSelector;
        levelSelector = 0;
    }
}
void CentralWidget::uncheckShowPlans()
{
    showPlans->setChecked(false);
}

CentralWidget::~CentralWidget()
{
    delete layout;
    delete floorScene;
    delete floorView;
    delete profileScene;
    delete profileView;
    delete showPlans;

    if(allPlanScene != 0)  {
        delete allPlanScene;
    }

    if(allPlanView != 0) {
        delete allPlanView;
    }

    if(levelSelector != 0) {
        delete levelSelector;
        levelSelector = 0;
    }
}
