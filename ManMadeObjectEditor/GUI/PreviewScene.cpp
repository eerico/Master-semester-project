#include "PreviewScene.h"

const int PreviewScene::vertexRadius(2);

PreviewScene::PreviewScene()
{
    this->setSceneRect(QRectF(0, 0, 400, 600));
}

void PreviewScene::loadPreview(std::vector<Vertex*>* vertexToShow) {
    // delete the previous loaded plan
    this->clear();

    if(vertexToShow == 0) {
        return;
    }

    // go through the vertices of the plan and draw their ellipse and edge
    for (unsigned int i(0); i < vertexToShow->size(); ++i) {
        Vertex* currentVertex = (*vertexToShow)[i];
        float x = currentVertex->getX();
        float y = currentVertex->getY();

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

    for (unsigned int i(0); i < vertexToShow->size(); ++i) {
        Vertex* v = (*vertexToShow)[i];

        delete v->getEdge2();
        delete v;
    }
    delete vertexToShow;
}
