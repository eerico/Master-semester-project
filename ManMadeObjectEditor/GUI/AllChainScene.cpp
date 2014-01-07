#include "AllChainScene.h"

const int AllChainScene::vertexRadius(2);

AllChainScene::AllChainScene(MeshManager * meshManager)
    : QGraphicsScene(), meshManager(meshManager)
{
    // set the scene size
    this->setSceneRect(QRectF(0, 0, 4000, 6000));
}

AllChainScene::~AllChainScene()
{
    this->clear();
}

void AllChainScene::loadPlan(int level, Show show) {
    // delete the previous loaded chains
    this->clear();

    // draw either the chains or the activeplan
    if(show == Chain1 || show == Chain2) {
        drawChain(level, show);
    } else if (show == ActivePlan){
        drawActivePlan(level, show);
    }
}

void AllChainScene::drawChain(int level, Show show) {
    // delete the previous loaded chains
    this->clear();

    //vector(level: vector(chain at the level: vector(edge) ) )
    std::vector< std::vector< std::vector< Edge* > > > chainsList;

    //extract the chain before or after the algorithm (chain1 or chain2)
    if(show == Chain1) {
        chainsList = meshManager->getChains();
    } else if (show == Chain2){
        chainsList = meshManager->getChains2();
    }

    // do not draw after the last level
    if(level >= chainsList.size()) {
        return;
    }

    // extract the chains at the level
    std::vector< std::vector< Edge* > > chains = chainsList[level];

    unsigned int numberChains = chains.size();
    float x(0.0f);
    float y(0.0f);

    Vertex* neighbor;

    // for every chain
    for(unsigned int chainIndex(0); chainIndex < numberChains; ++chainIndex) {      
        std::vector< Edge* > currentChain = chains[chainIndex];

        // assign a color identification to this chain
        QColor chainIdentification;
        chainIdentification.setRgb(std::rand()%256, std::rand()%256, std::rand()%256);

        //for each edges of this chain
        foreach(Edge* edge, currentChain){

            Vertex* currentVertex = edge->getVertex1();
            x = currentVertex->getX();
            y = currentVertex->getY();

            QRectF thisSize = this->sceneRect();
            Utils::adjustCoordinates3DToScene(x, y, thisSize.width(), thisSize.height());
            QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
            QPen pen;
            pen.setColor(Qt::black);
            pen.setWidth(vertexRadius);
            ellipse->setPen(pen);

            // draw the current vertex
            this->addItem(ellipse);

            // the current vertex has a neighbor, we can draw the edge between them
            neighbor = edge->getVertex2();

            float xn = neighbor->getX();
            float yn = neighbor->getY();
            Utils::adjustCoordinates3DToScene(xn, yn, thisSize.width(), thisSize.height());

            QGraphicsLineItem* lineItem = new QGraphicsLineItem(x, y, xn, yn);
            pen.setColor(chainIdentification);
            lineItem->setPen(pen);

            // draw the edge
            this->addItem(lineItem);

            QGraphicsEllipseItem* ellipseNeighbor = new QGraphicsEllipseItem(xn - vertexRadius, yn - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
            pen.setColor(Qt::black);
            pen.setWidth(vertexRadius);
            ellipseNeighbor->setPen(pen);

            // draw the vertex
            this->addItem(ellipseNeighbor);
        }
    }
}

void AllChainScene::drawActivePlan(int level, Show show) {
    // delete the previous loaded chains
    this->clear();

    //vector(level: vector(edge) )
    std::vector< std::vector< Edge* > > allActivePlan;

    //extract the active plan after the chains algorithm
    if(show == ActivePlan) {
        allActivePlan = meshManager->getActivePlanDebug();
    } else {
        return;
    }

    // do not draw after the last level
    if(level >= allActivePlan.size()) {
        return;
    }

    float x(0.0f);
    float y(0.0f);

    Vertex* neighbor;

    //extract the active plan at the level
    std::vector< Edge* > currentActivePlan = allActivePlan[level];

    // for every edge
    foreach(Edge* edge, currentActivePlan){
        Vertex* currentVertex = edge->getVertex1();
        x = currentVertex->getX();
        y = currentVertex->getY();

        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinates3DToScene(x, y, thisSize.width(), thisSize.height());
        QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(vertexRadius);
        ellipse->setPen(pen);

        // draw the current vertex
        this->addItem(ellipse);

        // the current vertex has a neighbor, we can draw the edge between them
        neighbor = edge->getVertex2();

        float xn = neighbor->getX();
        float yn = neighbor->getY();
        Utils::adjustCoordinates3DToScene(xn, yn, thisSize.width(), thisSize.height());

        QGraphicsLineItem* lineItem = new QGraphicsLineItem(x, y, xn, yn);
        pen.setColor(Qt::red);
        lineItem->setPen(pen);

        // draw the edge
        this->addItem(lineItem);

        QGraphicsEllipseItem* ellipseNeighbor = new QGraphicsEllipseItem(xn - vertexRadius, yn - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
        pen.setColor(Qt::black);
        pen.setWidth(vertexRadius);
        ellipseNeighbor->setPen(pen);

        // draw the vertex
        this->addItem(ellipseNeighbor);
    }

}
