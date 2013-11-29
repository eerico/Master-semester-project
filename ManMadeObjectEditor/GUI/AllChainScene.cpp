#include "AllChainScene.h"

const int AllChainScene::vertexRadius(2);

AllChainScene::AllChainScene(MeshManager * meshManager)
    : QGraphicsScene(), meshManager(meshManager)
{
    // set the scene size
    this->setSceneRect(QRectF(0, 0, 400, 600));
}

AllChainScene::~AllChainScene()
{
    this->clear();
}

void AllChainScene::loadPlan(int level, bool beforeChainAlgorithm) {
    // delete the previous loaded chains
    this->clear();

    std::vector< std::vector< std::vector< Edge* > > > chainsList;

    if(beforeChainAlgorithm) {
        chainsList = meshManager->getChains();
    } else {
        chainsList = meshManager->getChains2();
    }

    std::vector< std::vector< Edge* > > chains = chainsList[level];

    unsigned int numberChains = chains.size();
    float x(0.0f);
    float y(0.0f);

    for(unsigned int chainIndex(0); chainIndex < numberChains; ++chainIndex) {
        std::vector< Edge* > currentChain = chains[chainIndex];
        QColor chainIdentification;
        chainIdentification.setRgb(std::rand()%256, std::rand()%256, std::rand()%256);
        foreach(Edge* edge, currentChain){
            Vertex* currentVertex = edge->getVertex1();
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
            Vertex* neighbor = edge->getVertex2();

            float xn = neighbor->getX();
            float yn = neighbor->getY();
            Utils::adjustCoordinates3DToScene(xn, yn, thisSize.width(), thisSize.height());

            QGraphicsLineItem* lineItem = new QGraphicsLineItem(x, y, xn, yn);
            pen.setColor(chainIdentification);
            lineItem->setPen(pen);

            // draw the edge
            this->addItem(lineItem);
        }
    }
}
