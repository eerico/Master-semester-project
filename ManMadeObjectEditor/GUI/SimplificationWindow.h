#ifndef SIMPLIFICATIONWINDOW_H
#define SIMPLIFICATIONWINDOW_H

#include <QDialog>
#include "BasicQGraphicsView.h"
#include "SimplificationScene.h"
#include "SimplificationFloorPlanScene.h"
#include "SimplificationProfileScene.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "../Simplification/Curve.h"
#include "../Simplification/Simplification.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QSlider>
#include "PreviewScene.h"

/**
 * @brief The SimplificationWindow class
 * This class is the window used when the user want to apply
 * the simplification algorithm
 */
class SimplificationWindow: public QDialog
{
    Q_OBJECT
public:
    SimplificationWindow(MeshManager* meshManager, bool floorPlanSimplification);
    ~SimplificationWindow();

public slots:
    /**
     * @brief ok
     * Apply the simplification algorithm on the profile or
     * on the floor plan
     */
    void ok();

    /**
     * @brief cancel
     * Cancel the simplification
     */
    void cancel();

    /**
     * @brief valueSliderChanged
     * Method called when the value of the slider changes.
     * It will load a new preview based on the new  value of
     * the slider.
     * @param thresholdValue
     */
    void valueSliderChanged(int thresholdValue);

signals:
    /**
     * @brief closeSignal
     * Signal emitted when the window is closed
     */
    void closeSignal();

private:
    // The scene that will load the floor plan or the profile to simplify
    SimplificationScene* scene;
    // The scene that will load the peview
    PreviewScene* previewScene;
    // the two associated view
    BasicQGraphicsView* view;
    BasicQGraphicsView* previewView;

    // All layout
    QGridLayout* gridLayout;
    QHBoxLayout* hBoxLayoutScene;
    QHBoxLayout* hBoxLayout;
    QFormLayout* formLayout;

    // the two buttons
    QPushButton* okButton;
    QPushButton* cancelButton;

    MeshManager* meshManager;

    // The array containing all curve
    std::vector< Curve* > curveArray;

    // A slider used to defined the threshold used in
    // the simplification window
    QSlider* thresholdSlider;

    // Used to know if this simplification window is used
    // to simplify the floor plan or the profile
    bool floorPlanSimplification;

    // Used to know if the user has pressed a button
    bool buttonPressed;

    // Used to know the final size of the profile/floor plan
    // after the simplification preview
    unsigned int previewSize;

    void closeEvent(QCloseEvent* event);

    void keyPressEvent(QKeyEvent *e);
};

#endif // SIMPLIFICATIONWINDOW_H
