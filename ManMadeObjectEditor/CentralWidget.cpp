#include "CentralWidget.h"

CentralWidget::CentralWidget(QWidget* parent, MeshManager* meshManager) :
    QWidget(parent), meshManager(meshManager), allPlanView(0), allPlanScene(0), levelSelector(0)
{
    layout = new QGridLayout();
    
    floorScene = new FloorScene(meshManager);
    floorView = new BasicQGraphicsView(floorScene);
    
    layout->addWidget(floorView, 0, 0);
    
    profileScene = new ProfileScene(meshManager);
    profileView = new BasicQGraphicsView(profileScene);
    layout->addWidget(profileView, 0, 1);

    showPlans = new QCheckBox("Show all plans");
    layout->addWidget(showPlans, 1, 0);

    allPlanScene = new AllPlanScene(meshManager);
    allPlanView = new BasicQGraphicsView(allPlanScene);
    layout->addWidget(allPlanView, 0, 2);

    allPlanView->hide();

    QObject::connect(floorScene, SIGNAL(newProfileSelected()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), floorScene, SLOT(loadFloorPlan()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(showPlans, SIGNAL(clicked()), this, SLOT(allPlans()));

    this->setLayout(layout);
    this->setMinimumSize(QSize(300, 170));

}

void CentralWidget::changeProfileColorIndication()
{
    this->setAutoFillBackground(true);
    QPalette palette(this->palette());
    palette.setColor(QPalette::Window, *meshManager->getCurrentProfile()->getProfileColorIdentification());
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
    this->adjustSize();
    this->parentWidget()->adjustSize();
    this->repaint();
}

void CentralWidget::showAllPlans()
{
    if (meshManager->getPlans().size() == 0) {
        return;
    }

    levelSelector = new QSlider(Qt::Horizontal);

    levelSelector->setMinimum(0);
    levelSelector->setMaximum(meshManager->getPlans().size() - 1);
    layout->addWidget(levelSelector, 1, 2);

    allPlanView->show();
    levelSelector->show();

    allPlanScene->loadPlan(levelSelector->value());

    QObject::connect(levelSelector, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChanged(int)));
}

void CentralWidget::hideAllPlans()
{
    allPlanView->hide();

    if(levelSelector != 0) {
        levelSelector->hide();
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
