#ifndef ALLCHAINSCENE_H
#define ALLCHAINSCENE_H

#include "../MeshManager.h"
#include "QGraphicsScene"
#include <QColor>

/**
 * @brief The AllChainScene class
 * This class is a scene used to show the result of the chains
 * algorithm and the reslting active plan
 */
class AllChainScene : public QGraphicsScene
{
public:
    AllChainScene(MeshManager* meshManager);
    ~AllChainScene();


    /**
     * @brief The Show enum
     * Choose what to show in this scene
     */
    enum Show{
        Chain1, // The edges involved in the
                // chain algorithm before running.
                // The color defined the different chains
        Chain2, // The edge after the chains algorithm
        ActivePlan // The updated active plan after the
                    // two chain algorithm
    };

    /**
     * @brief loadPlan
     * Show the edges at the level defined by the level parameter
     * @param level The level to show
     * @param show Either chain1, chain2 or ActivePlan
     */
    void loadPlan(int level, Show show);

private:
    //Used to obtain the information about the chains and the activeplan
    MeshManager* meshManager;

    // The radius of the vertices that we draw
    static const int vertexRadius;

    /**
     * @brief drawChain
     * Draw the chains at the level defined by level
     * @param level
     * @param show
     */
    void drawChain(int level, Show show);

    /**
     * @brief drawActivePlan
     * Draw the activeplan at the level defined by level
     * @param level
     * @param show
     */
    void drawActivePlan(int level, Show show);
};

#endif // ALLCHAINSCENE_H
