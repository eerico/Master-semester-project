#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QGraphicsScene>
#include <QLabel>
#include <QPalette>
#include <QFont>
#include <QCheckBox>
#include <QSlider>
#include "../GUI/ProfileScene.h"
#include "../GUI/FloorScene.h"
#include "../MeshManager.h"
#include "../GUI/BasicQGraphicsView.h"
#include "../GUI/AllPlanScene.h"

/**
 * @brief The CentralWidget class
 * This class represents the central widget of the main windows.
 */
class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget(QWidget* const parent, MeshManager* const meshManager);
    ~CentralWidget();
    
    // The layout used to place our element inside this widget
    QGridLayout *layout;

    // The scene that show the floor plan
    FloorScene *floorScene;

    // The view associated with the floor scene
    BasicQGraphicsView *floorView;

    // The scene that show the current selected profile
    ProfileScene *profileScene;

    // The view associated with the profile scene
    BasicQGraphicsView *profileView;

    // Check box that allow the user to view the floor scene found
    // at every level
    QCheckBox* showPlans;

    // The scene that show the floor plan at the selected level
    AllPlanScene *allPlanScene;

    // The view associated with the all plan scene
    BasicQGraphicsView *allPlanView;

    // A slider used to choose which level the user want to see
    QSlider *levelSelector;

    /**
     * @brief hideAllPlans
     * Hide the scene that shows the floor plan at a selected level
     */
    void hideAllPlans();
    
public slots:
    /**
     * @brief changeProfileColorIndication
     * Change the background color to be the color of the selected profile
     */
    void changeProfileColorIndication();

    /**
     * @brief allPlans
     * Action made when the user click on the showplans check box.
     * It either hides or shows the floor plan at a selected level
     */
    void allPlans();

    /**
     * @brief valueSliderChanged
     * Action called when the value of the slider is changed, telling us that
     * the user want to see a specific floor plan at the selected level. This
     * level is given by the value of the slider
     * @param level The level selected by the slider
     */
    void valueSliderChanged(const int& level);

    /**
     * @brief uncheckShowPlans
     * Uncheck the check box used to control if the user want to see a floor plan
     * at a specific level
     */
    void uncheckShowPlans();
    
private:

    // Used to obtain different kind of information about the floor plan and the profiles
    MeshManager* meshManager;

    /**
     * @brief showAllPlans
     * Show the scene that shows the floor plan at a selected level
     */
    void showAllPlans();
};

#endif // CENTRALWIDGET_H
