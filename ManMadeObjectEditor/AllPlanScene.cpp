#include "AllPlanScene.h"

const int AllPlanScene::vertexRadius(2);

AllPlanScene::AllPlanScene(Mesh* mesh)
: QGraphicsScene(), mesh(mesh)
{
    this->setSceneRect(QRectF(0, 0, 400, 600));
}

AllPlanScene::~AllPlanScene()
{
    this->clear();
}

void AllPlanScene::loadPlan(int level) {
    this->clear();

    std::vector< std::vector< Vertex* > > plans = mesh->getPlans();


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
        pen.setWidth(2);
        ellipse->setPen(pen);
        this->addItem(ellipse);

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
        this->addItem(lineItem);
    }
}


