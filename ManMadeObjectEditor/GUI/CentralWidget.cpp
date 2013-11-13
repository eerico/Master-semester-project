#include "CentralWidget.h"

CentralWidget::CentralWidget(QWidget* parent, MeshManager* meshManager) :
    QWidget(parent), meshManager(meshManager), allPlanView(0), allPlanScene(0), levelSelector(0)
{
    layout = new QGridLayout();
    
    // create all scenes,views, checkbox and add it to the layout
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

    // by default, hide this view
    allPlanView->hide();

    // connect all signal and slot
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), floorScene, SLOT(loadFloorPlan()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(showPlans, SIGNAL(clicked()), this, SLOT(allPlans()));

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

void CentralWidget::changeProfileColorIndication() {
    // change the background color, thus the user can easily identify with which profile he is currently working on
    this->setAutoFillBackground(true);
    QPalette palette(this->palette());
    palette.setColor(QPalette::Window, *meshManager->getCurrentProfile()->getProfileColorIdentification());
    this->setPalette(palette);
}

void CentralWidget::valueSliderChanged(int level) {
    allPlanScene->loadPlan(level);
}

void CentralWidget::allPlans() {
    if(showPlans->isChecked()) {
        showAllPlans();
    } else {
        hideAllPlans();
    }

    // automatically ajdust the size and repaint the widget when the user shows or hides the all plan view
    this->adjustSize();
    this->parentWidget()->adjustSize();
    this->repaint();
}

void CentralWidget::showAllPlans() {
    // if there is nothing to show, dont try to show something
    if (meshManager->getPlans().size() == 0) {
        return;
    }

    // create the slider used to select which floor plan level the user want to see
    levelSelector = new QSlider(Qt::Horizontal);

    levelSelector->setMinimum(0);
    levelSelector->setMaximum(meshManager->getPlans().size() - 1);
    layout->addWidget(levelSelector, 1, 2);

    // show the view and the slider
    allPlanView->show();
    levelSelector->show();

    // load the selected plan
    allPlanScene->loadPlan(levelSelector->value());

    // connect the signal emitted by the slider when its value changed to the slot that will load the
    // floor plan at a given level defined by the value of the slider
    QObject::connect(levelSelector, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChanged(int)));
}

void CentralWidget::hideAllPlans() {
    // hide the all plan view
    allPlanView->hide();

    // the slider is no more usefull
    if(levelSelector != 0) {
        levelSelector->hide();
        QObject::disconnect(levelSelector, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChanged(int)));

        delete levelSelector;
        levelSelector = 0;
    }
}

void CentralWidget::uncheckShowPlans() {
    showPlans->setChecked(false);
}
