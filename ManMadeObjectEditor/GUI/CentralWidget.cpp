#include "CentralWidget.h"

CentralWidget::CentralWidget(QWidget* parent, MeshManager* meshManager) :
    QWidget(parent), meshManager(meshManager), allPlanView(0), allPlanScene(0), levelSelector(0)
  , allChainScene(0), allChainView(0), levelChainSelector(0),  allActivePlanScene(0)
  , allActivePlanView(0)
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

    showChains = new QCheckBox("Show all chains and active plans");
    layout->addWidget(showChains, 2, 0);

    allChainScene = new AllChainScene(meshManager);
    allChainView = new BasicQGraphicsView(allChainScene);
    layout->addWidget(allChainView, 0, 3);

    allChainAfterAlgorithmScene = new AllChainScene(meshManager);
    allChainAfterAlgorithmView = new BasicQGraphicsView(allChainAfterAlgorithmScene);
    layout->addWidget(allChainAfterAlgorithmView, 0, 4);

    allActivePlanScene = new AllChainScene(meshManager);
    allActivePlanView = new BasicQGraphicsView(allActivePlanScene);
    layout->addWidget(allActivePlanView, 0, 5);

    // by default, hide this view
    allChainAfterAlgorithmView->hide();
    allChainView->hide();
    allActivePlanView->hide();

    // connect all signal and slot
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), floorScene, SLOT(loadFloorPlan()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(floorScene, SIGNAL(newProfileSelected()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(meshManager, SIGNAL(newFloorPlan()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(showPlans, SIGNAL(clicked()), this, SLOT(allPlans()));
    QObject::connect(showChains, SIGNAL(clicked()), this, SLOT(allChains()));
    QObject::connect(meshManager, SIGNAL(newProfileCreatedForSelectedEdge()), floorScene,
                     SLOT(newProfileCreatedForSelectedEdge()));
    QObject::connect(meshManager, SIGNAL(newProfileCreatedForSelectedEdge()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(meshManager, SIGNAL(newProfileCreatedForSelectedEdge()), this, SLOT(changeProfileColorIndication()));
    QObject::connect(meshManager, SIGNAL(drawWithoutDeleteOldProfile()), profileScene, SLOT(drawWithoutDeleteOldProfile()));
    QObject::connect(meshManager, SIGNAL(newProfileSelected()), profileScene, SLOT(newProfileSelected()));
    QObject::connect(meshManager, SIGNAL(updateColorIndicationGUI()), this, SLOT(changeProfileColorIndication()));

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

    if(allChainScene != 0)  {
        delete allChainScene;
    }

    if(allChainView != 0) {
        delete allChainView;
    }

    if(allChainAfterAlgorithmScene != 0)  {
        delete allChainAfterAlgorithmScene;
    }

    if(allChainAfterAlgorithmView != 0) {
        delete allChainAfterAlgorithmView;
    }

    if(allActivePlanScene != 0) {
        delete allActivePlanScene;
    }

    if(allActivePlanView != 0) {
        delete allActivePlanView;
    }

    if(levelChainSelector != 0) {
        delete levelChainSelector;
        levelChainSelector = 0;
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

    // automatically adjust the size and repaint the widget when the user shows or hides the all plan view
    //this->adjustSize();

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

void CentralWidget::valueSliderChainsChanged(int level) {    
    allChainScene->loadPlan(level, AllChainScene::Chain1);
    allChainAfterAlgorithmScene->loadPlan(level, AllChainScene::Chain2);
    allActivePlanScene->loadPlan(level, AllChainScene::ActivePlan);
}

void CentralWidget::allChains() {
    if(showChains->isChecked()) {
        showAllChains();
    } else {
        hideAllChains();
    }

    // automatically adjust the size and repaint the widget when the user shows or hides the views
    //this->adjustSize();
    this->parentWidget()->adjustSize();
    this->repaint();
}

void CentralWidget::showAllChains() {
    // if there is nothing to show, dont try to show something
    if (meshManager->getChains().size() == 0) {
        return;
    }

    // create the slider used to select which chains/active plan level the user want to see
    levelChainSelector = new QSlider(Qt::Horizontal);

    levelChainSelector->setMinimum(0);
    levelChainSelector->setMaximum(std::max(meshManager->getChains().size() - 1, meshManager->getActivePlanDebug().size() - 1));
    layout->addWidget(levelChainSelector, 1, 3);

    // show the views and the slider
    allChainView->show();
    allChainAfterAlgorithmView->show();
    allActivePlanView->show();
    levelChainSelector->show();

    // load the selected informations
    allChainScene->loadPlan(levelChainSelector->value(), AllChainScene::Chain1);
    allChainAfterAlgorithmScene->loadPlan(levelChainSelector->value(), AllChainScene::Chain2);
    allActivePlanScene->loadPlan(levelChainSelector->value(), AllChainScene::ActivePlan);

    // connect the signal emitted by the slider when its value changed to the slot that will load the
    // chains/active plan at a given level defined by the value of the slider
    QObject::connect(levelChainSelector, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChainsChanged(int)));
}

void CentralWidget::hideAllChains() {
    // hide the views
    allChainView->hide();
    allChainAfterAlgorithmView->hide();
    allActivePlanView->hide();

    // the slider is no more usefull
    if(levelChainSelector != 0) {
        levelChainSelector->hide();
        QObject::disconnect(levelChainSelector, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChainsChanged(int)));

        delete levelChainSelector;
        levelChainSelector = 0;
    }
}

void CentralWidget::uncheckShowChains() {
    showChains->setChecked(false);
}
