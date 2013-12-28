#include "ProfileMergeWindow.h"

ProfileMergeWindow::ProfileMergeWindow(MeshManager *meshManager)
    :QDialog(0, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint )
    , meshManager(meshManager), buttonPressed(false)
{
    //create the two scenes with their views
    scene = new ProfileMergeScene(&newProfile, meshManager, true);
    scene2 = new ProfileMergeScene(&newProfile, meshManager, false);

    view = new BasicQGraphicsView(scene);
    view2 = new BasicQGraphicsView(scene2);

    // create the buttons
    mergeButton = new QPushButton("Merge");
    cancelButton = new QPushButton("Cancel");

    // create the layout
    gridLayout = new QGridLayout;
    hBoxLayout = new QHBoxLayout;
    hBoxLayout2 = new QHBoxLayout;

    // construct the window
    gridLayout->addLayout(hBoxLayout, 1, 0);
    gridLayout->addLayout(hBoxLayout2, 2, 0);

    this->setLayout(gridLayout);

    hBoxLayout->addWidget(view);
    hBoxLayout->addWidget(view2);

    hBoxLayout2->addWidget(mergeButton);
    hBoxLayout2->addWidget(cancelButton);

    // connect the buttons with their actions
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

    // if the two profile are correctly defined
    if(profileToMerge1 != 0 && profileToMerge2 != 0) {
        // create a new profile
        Profile* newMergedProfile = new Profile(true);
        ProfileDestructorManager::putProfile(newMergedProfile);

        // sort the vertices used during the merge by ascending order (height)
        VertexComparator vertexComparator;
        std::sort(newProfile.begin(), newProfile.end(), vertexComparator);

        // add every selected vertex into the new profile
        foreach(Vertex* vertex, newProfile) {
            Vertex* vertexCopy = new Vertex(vertex->getX(), vertex->getY());
            newMergedProfile->addVertexEnd(vertexCopy);
        }

        // for every edge, if its associated profile is one of the merged profile, then replace
        // it by the new profile and update the color identification
        Vertex* iterator = meshManager->getFloorPlan();
        unsigned int size = meshManager->getFloorPlanSize();
        for(unsigned int i(0); i < size; ++i) {
            Edge* currentEdge = iterator->getEdge2();
            Profile* currentProfile = currentEdge->getProfile();

            if(currentProfile == profileToMerge1 || currentProfile == profileToMerge2) {
                currentEdge->setProfile(newMergedProfile);

                QGraphicsLineItem* lineItem = currentEdge->getLineItem();
                QPen linePen = lineItem->pen();
                linePen.setColor(*newMergedProfile->getProfileColorIdentification());
                lineItem->setPen(linePen);
            }

            iterator = iterator->getNeighbor2();
        }

        // update the meshManager and all associated data
        meshManager->setCurrentProfile(newMergedProfile);
        meshManager->emitDrawWithoutDeleteOldProfile();
        meshManager->emitUpdateColorIndicationGUI();
        meshManager->setUpdateOnMesh();
    } else if(profileToMerge1 != 0 || profileToMerge2 != 0) {
        if(meshManager->getCurrentProfile() != 0) {
            // draw again the profile in the profile scene
            meshManager->emitDrawWithoutDeleteOldProfile();
        }
    }
    buttonPressed = true;
    close();
}

void ProfileMergeWindow::cancel(){
    scene->revert();
    scene2->revert();

    Profile* profileToMerge1 = scene->getProfileToMerge();
    Profile* profileToMerge2 = scene2->getProfileToMerge();

    if(meshManager->getCurrentProfile() != 0 && (profileToMerge1 != 0 || profileToMerge2 != 0)) {
        // draw again the profile in the profile scene
        meshManager->emitDrawWithoutDeleteOldProfile();
    }
    buttonPressed = true;
    close();
}

void ProfileMergeWindow::closeEvent(QCloseEvent *event) {
    if(!buttonPressed) {
        scene->revert();
        scene2->revert();

        Profile* profileToMerge1 = scene->getProfileToMerge();
        Profile* profileToMerge2 = scene2->getProfileToMerge();

        if(meshManager->getCurrentProfile() != 0 && (profileToMerge1 != 0 || profileToMerge2 != 0)) {
            // draw again the profile in the profile scene
            meshManager->emitDrawWithoutDeleteOldProfile();
        }
    }

    emit closeSignal();
    event->accept();
}

void ProfileMergeWindow::keyPressEvent(QKeyEvent *e){
    if(e->key() != Qt::Key_Escape){
        QDialog::keyPressEvent(e);
    }
}

void ProfileMergeWindow::updateScenes() {
    // the user has selected a new edge. We must update the two scene with the new
    // information
    scene->updateProfileSelected();
    scene2->updateProfileSelected();
}
