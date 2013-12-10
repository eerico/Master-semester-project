#include "SimplificationWindow.h"

SimplificationWindow::SimplificationWindow(MeshManager* meshManager, bool floorPlanSimplification)
    :QDialog(0, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint )
    , meshManager(meshManager), floorPlanSimplification(floorPlanSimplification), buttonPressed(false)
{

    if(floorPlanSimplification) {
        scene = new SimplificationFloorPlanScene(meshManager, &curveArray);
    } else {
        scene = new SimplificationProfileScene(meshManager, &curveArray);
    }
    view = new BasicQGraphicsView(scene);

    gridLayout = new QGridLayout;
    hBoxLayout = new QHBoxLayout;
    formLayout = new QFormLayout;

    this->setLayout(gridLayout);
    gridLayout->addWidget(view, 0, 0);

    thresholdBox = new QDoubleSpinBox;
    thresholdBox->setRange(0.0, 2.0);
    thresholdBox->setSingleStep(0.01);
    thresholdBox->setValue(0.01);
    thresholdBox->setDecimals(2);

    formLayout->addRow("Threshold: ", thresholdBox);

    gridLayout->addLayout(formLayout, 1, 0);
    gridLayout->addLayout(hBoxLayout, 2, 0);

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
    delete formLayout;

    delete okButton;
    delete cancelButton;

    delete thresholdBox;
    delete thresholdLabel;
}

void SimplificationWindow::ok(){
    scene->revertColor();

    Simplification simplification(&curveArray, meshManager, thresholdBox->value());

    if(floorPlanSimplification) {
        simplification.simplifyFloorPlan();
    } else {
        simplification.simplifyProfile();
    }
    buttonPressed = true;
    close();
}

void SimplificationWindow::cancel(){

    scene->revertColor();

    if(floorPlanSimplification) {
        meshManager->emitNewFloorPlan();
    } else {
        meshManager->emitDrawWithoutDeleteOldProfile();
    }
    buttonPressed = true;
    close();
}

void SimplificationWindow::closeEvent(QCloseEvent *event) {
    if(!buttonPressed){
        scene->revertColor();

        if(floorPlanSimplification) {
            meshManager->emitNewFloorPlan();
        } else {
            meshManager->emitDrawWithoutDeleteOldProfile();
        }
    }

    emit closeSignal();
    event->accept();
}

void SimplificationWindow::keyPressEvent(QKeyEvent *e){
    if(e->key() != Qt::Key_Escape){
        QDialog::keyPressEvent(e);
    }
}
