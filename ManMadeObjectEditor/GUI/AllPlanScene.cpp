#include "AllPlanScene.h"

const int AllPlanScene::vertexRadius(2);

AllPlanScene::AllPlanScene(MeshManager * meshManager)
    : QGraphicsScene(), meshManager(meshManager)
{
    // set the scene size
    this->setSceneRect(QRectF(0, 0, 400, 600));
}

AllPlanScene::~AllPlanScene()
{
    this->clear();
}

void AllPlanScene::loadPlan(int level) {
    // delete the previous loaded plan
    this->clear();

    // get the floor plans at every level
    std::vector< std::vector< Vertex* > > plans = meshManager->getPlans();

    // take the plan at the level that we want to draaw
    std::vector< Vertex* > plan = plans[level];

    unsigned int planSize = plan.size();
    float x(0.0f);
    float y(0.0f);

    // go through the vertices of the plan and draw their ellipse and edge
    for (unsigned int i(0); i < planSize; ++i) {
        Vertex* currentVertex = plan[i];
        x = currentVertex->getX();
        y = currentVertex->getY();

        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinates3DToScene(x, y, thisSize.width(), thisSize.height());
        QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
        QPen pen;
        pen.setWidth(vertexRadius);
        ellipse->setPen(pen);

        // draw the current vertex
        this->addItem(ellipse);

        // if the current vertex has a neighbor then we can draw the edge between them
        Vertex* neighbor = currentVertex->getNeighbor2();
        if (neighbor == 0) {
            continue;
        }

        float xn = neighbor->getX();
        float yn = neighbor->getY();
        Utils::adjustCoordinates3DToScene(xn, yn, thisSize.width(), thisSize.height());

        QGraphicsLineItem* lineItem = new QGraphicsLineItem(x, y, xn, yn);
        pen.setColor(QColor(0.0f));
        lineItem->setPen(pen);

        // draw the edge
        this->addItem(lineItem);
    }
}


