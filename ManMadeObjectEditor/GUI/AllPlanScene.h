#ifndef ALLPLANSCENE_H
#define ALLPLANSCENE_H

#include "../MeshManager.h"
#include "QGraphicsScene"

/**
 * @brief The AllPlanScene class
 * This class is a scene used to show all floors levels found with
 * the algorithm used to construct the floor plan and the profiles
 */
class AllPlanScene : public QGraphicsScene
{

public:
    AllPlanScene(MeshManager* const meshManager);
    ~AllPlanScene();

    /**
     * @brief loadPlan
     * Show the floor plan at the level defined by the level variable
     * in this scene
     * @param level The level to show
     */
    void loadPlan(const int& level);

private:

    // Used to take the floor plan at every level
    MeshManager* meshManager;

    // The radius of the vertices that we draw
    static const int vertexRadius;
};

#endif // ALLPLANSCENE_H
