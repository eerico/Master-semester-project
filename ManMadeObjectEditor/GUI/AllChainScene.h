#ifndef ALLCHAINSCENE_H
#define ALLCHAINSCENE_H

#include "../MeshManager.h"
#include "QGraphicsScene"
#include <QColor>

class AllChainScene : public QGraphicsScene
{
public:
    AllChainScene(MeshManager* meshManager);
    ~AllChainScene();


    enum Show{
        Chain1,
        Chain2,
        ActivePlan
    };

    void loadPlan(int level, Show show);

private:
    MeshManager* meshManager;

    static const int vertexRadius;

    void drawChain(int level, Show show);
    void drawActivePlan(int level, Show show);
};

#endif // ALLCHAINSCENE_H
