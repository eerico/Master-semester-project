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
    void loadPlan(int level, bool beforeChainAlgorithm);

private:
    MeshManager* meshManager;

    static const int vertexRadius;
};

#endif // ALLCHAINSCENE_H
