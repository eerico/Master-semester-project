#include "SimplificationWindow.h"

SimplificationWindow::SimplificationWindow(MeshManager* meshManager, bool floorPlanSimplification)
    :QDialog(0, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint )
    , meshManager(meshManager), floorPlanSimplification(floorPlanSimplification), buttonPressed(false)
    , previewSize(0)
{

    if(floorPlanSimplification) {
        scene = new SimplificationFloorPlanScene(meshManager, &curveArray);
    } else {
        scene = new SimplificationProfileScene(meshManager, &curveArray);
    }
    view = new BasicQGraphicsView(scene);

    previewScene = new PreviewScene;
    previewView = new BasicQGraphicsView(previewScene);

    gridLayout = new QGridLayout;
    hBoxLayoutScene = new QHBoxLayout;
    hBoxLayout = new QHBoxLayout;
    formLayout = new QFormLayout;

    this->setLayout(gridLayout);
    gridLayout->addLayout(hBoxLayoutScene, 0, 0);
    hBoxLayoutScene->addWidget(view);
    hBoxLayoutScene->addWidget(previewView);

    /*thresholdBox = new QDoubleSpinBox;
    thresholdBox->setRange(0.0, 2.0);
    thresholdBox->setSingleStep(0.01);
    thresholdBox->setValue(0.01);
    thresholdBox->setDecimals(2);*/

    thresholdSlider = new QSlider(Qt::Horizontal);
    thresholdSlider->setRange(0, 200);
    thresholdSlider->setSingleStep(1);
    thresholdSlider->setValue(0);

    QObject::connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(valueSliderChanged(int)));

    formLayout->addRow("Threshold: ", thresholdSlider);

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
    delete previewScene;
    delete view;
    delete previewView;
    delete gridLayout;
    delete hBoxLayoutScene;
    delete hBoxLayout;
    delete formLayout;

    delete okButton;
    delete cancelButton;

    delete thresholdSlider;
}

void SimplificationWindow::ok(){
    Simplification simplification(&curveArray, meshManager, (float)thresholdSlider->value() / 100.0f);

    if(floorPlanSimplification) {
        if(previewSize >= 3) {
            scene->revertColor();
            simplification.simplifyFloorPlan();
        } else {
            QMessageBox::warning(0, "Warning", "The floor plan must have at least 3 edges !");
            return;
        }
    } else {
        scene->revertColor();
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

void SimplificationWindow::valueSliderChanged(int thresholdValue) {
    if(curveArray.size() == 0) {
        return;
    }

    float threshold = (float)thresholdValue / 100.0f;

    Simplification simplification(&curveArray, meshManager, threshold);

    std::vector<Vertex*>* preview;
    if(floorPlanSimplification) {
        preview = simplification.simplifyFloorPlanPreview();
    } else {
        preview = simplification.simplifyProfilePreview();
    }
    previewSize = preview->size();
    previewScene->loadPreview(preview);
}
