#ifndef ALLPLANSCENE_H
#define ALLPLANSCENE_H

#include "MeshManager.h"
#include "QGraphicsScene"

class AllPlanScene : public QGraphicsScene
{

public:
    AllPlanScene(MeshManager* meshManager);
    ~AllPlanScene();
    void loadPlan(int level);

private:
    MeshManager* meshManager;
    static const int vertexRadius;
};

#endif // ALLPLANSCENE_H
