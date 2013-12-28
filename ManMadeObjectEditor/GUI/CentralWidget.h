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
#include "../GUI/AllChainScene.h"

/**
 * @brief The CentralWidget class
 * This class represents the central widget of the main windows.
 */
class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget(QWidget* parent, MeshManager* meshManager);
    ~CentralWidget();


    /**
     * @brief hideAllPlans
     * Hide the scene that shows the floor plan at a selected level
     * when extracting the floor plan and profile
     */
    void hideAllPlans();

    /**
     * @brief hideAllChains
     * Hide the scene that shows the information about the chains and the activeplan
     * during the 3D reconstruction
     */
    void hideAllChains();
    
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
    void valueSliderChanged(int level);

    /**
     * @brief uncheckShowPlans
     * Uncheck the check box used to control if the user want to see a floor plan
     * at a specific level
     */
    void uncheckShowPlans();

    /**
     * @brief valueSliderChainsChanged
     * Action called when the value of the slider is changed, telling us that
     * the user want to see a specific floor plan and chains
     * at the selected level during the 3D reconstruction. This
     * level is given by the value of the slider
     * @param level
     */
    void valueSliderChainsChanged(int level);

    /**
     * @brief allChains
     * Action made when the user click on the showChains check box.
     * It either hides or shows the floor plan and chains for the 3D
     * reconstruction at a selected level
     */
    void allChains();

    /**
     * @brief uncheckShowChains
     * Uncheck the check box used to control if the user want to see all floor plan
     * and chains during the 3d reconstruction
     */
    void uncheckShowChains();
    
private:

    // Used to obtain different kind of information about the floor plan and the profiles
    MeshManager* meshManager;

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

    // Check box that allow the user to view the chains and active plan
    // during the 3d reconstruction
    QCheckBox* showChains;

    // The scene that show the chains before the chains algorithm
    AllChainScene *allChainScene;

    // The view associated with the allChainScene
    BasicQGraphicsView *allChainView;

    // The scene that show the chains after the chains algorithm
    AllChainScene *allChainAfterAlgorithmScene;

    // The view associated with the allChainAfterAlgorithmScene
    BasicQGraphicsView *allChainAfterAlgorithmView;

    // The scene that show the resulting active plan after the chains algorithm
    AllChainScene *allActivePlanScene;

    // The view associated with the allActivePlanScene
    BasicQGraphicsView *allActivePlanView;

    // A slider used to choose which chains/active plan to show
    QSlider *levelChainSelector;

    /**
     * @brief showAllPlans
     * Show the scene that shows the floor plan at a selected level
     */
    void showAllPlans();

    /**
     * @brief showAllChains
     * Show the scenes that shows all chains before and after the chains algorithm and the resulting active plans
     */
    void showAllChains();
};

#endif // CENTRALWIDGET_H
