#ifndef ALLPLANSCENE_H
#define ALLPLANSCENE_H

#include "Mesh.h"
#include "QGraphicsScene"

class AllPlanScene : public QGraphicsScene
{

public:
    AllPlanScene(Mesh* mesh);
    ~AllPlanScene();
    void loadPlan(int level);

private:
    Mesh* mesh;
    static const int vertexRadius;
};

#endif // ALLPLANSCENE_H
