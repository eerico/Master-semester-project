#include "ProfileMergeWindow.h"

ProfileMergeWindow::ProfileMergeWindow(MeshManager *meshManager)
    :QDialog(0, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint )
    , meshManager(meshManager)
{
    scene = new ProfileMergeScene(&newProfile, meshManager, true);
    scene2 = new ProfileMergeScene(&newProfile, meshManager, false);

    view = new BasicQGraphicsView(scene);
    view2 = new BasicQGraphicsView(scene2);

    mergeButton = new QPushButton("Merge");
    cancelButton = new QPushButton("Cancel");

    gridLayout = new QGridLayout;
    hBoxLayout = new QHBoxLayout;
    hBoxLayout2 = new QHBoxLayout;

    gridLayout->addLayout(hBoxLayout, 1, 0);
    gridLayout->addLayout(hBoxLayout2, 2, 0);

    this->setLayout(gridLayout);

    hBoxLayout->addWidget(view);
    hBoxLayout->addWidget(view2);

    hBoxLayout2->addWidget(mergeButton);
    hBoxLayout2->addWidget(cancelButton);

    connect(mergeButton, SIGNAL(clicked()), this, SLOT(merge()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    this->setWindowTitle("Merge Profile");

    QObject::connect(meshManager, SIGNAL(newEdgeSelected()), this, SLOT(updateScenes()));

}


ProfileMergeWindow::~ProfileMergeWindow(){
    delete scene;
    delete scene2;
    delete view;
    delete view2;
    delete gridLayout;
    delete hBoxLayout;
    delete hBoxLayout2;

    delete mergeButton;
    delete cancelButton;
}

void ProfileMergeWindow::merge(){
    scene->revert();
    scene2->revert();

    // TODO
    if(meshManager->getCurrentProfile() != 0) {
        meshManager->emitDrawWithoutDeleteOldProfile();
    }
    close();
}

void ProfileMergeWindow::cancel(){
    scene->revert();
    scene2->revert();

    // TODO
    if(meshManager->getCurrentProfile() != 0) {
        meshManager->emitDrawWithoutDeleteOldProfile();
    }
    close();
}

void ProfileMergeWindow::closeEvent(QCloseEvent *event) {
    emit closeSignal();
    event->accept();
}

void ProfileMergeWindow::updateScenes() {
    scene->revert();
    scene2->revert();

    scene->updateProfileSelected();
    scene2->updateProfileSelected();
}
