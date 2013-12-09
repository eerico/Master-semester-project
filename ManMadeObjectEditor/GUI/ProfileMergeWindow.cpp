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

    Profile* profileToMerge1 = scene->getProfileToMerge();
    Profile* profileToMerge2 = scene2->getProfileToMerge();

    if(profileToMerge1 != 0 && profileToMerge2 != 0) {
        Profile* newMergedProfile = new Profile(true);
        ProfileDestructorManager::putProfile(newMergedProfile);

        VertexComparator vertexComparator;
        std::sort(newProfile.begin(), newProfile.end(), vertexComparator);

        foreach(Vertex* vertex, newProfile) {
            Vertex* vertexCopy = new Vertex(vertex->getX(), vertex->getY());
            newMergedProfile->addVertexEnd(vertexCopy);
        }

        Vertex* iterator = meshManager->getFloorPlan();
        unsigned int size = meshManager->getFloorPlanSize();
        for(unsigned int i(0); i < size; ++i) {
            Edge* currentEdge = iterator->getEdge2();
            Profile* currentProfile = currentEdge->getProfile();

            if(currentProfile == profileToMerge1 || currentProfile == profileToMerge2) {
                currentEdge->setProfile(newMergedProfile);
            }

            iterator = iterator->getNeighbor2();
        }

        meshManager->emitDrawWithoutDeleteOldProfile();

        /*meshManager->setCurrentProfile(newMergedProfile);
        meshManager->emitNewFloorPlan();
        meshManager->setUpdateOnMesh();*/
    } else if(profileToMerge1 != 0 || profileToMerge2 != 0) {
        if(meshManager->getCurrentProfile() != 0) {
            meshManager->emitDrawWithoutDeleteOldProfile();
        }
    }
    close();
}

void ProfileMergeWindow::cancel(){
    scene->revert();
    scene2->revert();

    Profile* profileToMerge1 = scene->getProfileToMerge();
    Profile* profileToMerge2 = scene2->getProfileToMerge();

    if(meshManager->getCurrentProfile() != 0 && (profileToMerge1 != 0 || profileToMerge2 != 0)) {
        meshManager->emitDrawWithoutDeleteOldProfile();
    }
    close();
}

void ProfileMergeWindow::closeEvent(QCloseEvent *event) {
    emit closeSignal();
    event->accept();
}

void ProfileMergeWindow::updateScenes() {
    scene->updateProfileSelected();
    scene2->updateProfileSelected();
}
