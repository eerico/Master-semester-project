#include "FloorScene.h"
#include <iostream>

const int FloorScene::vertexRadius(6);

FloorScene::FloorScene(MeshManager * meshManager)
: QGraphicsScene(), meshManager(meshManager), currentlyMovingVertex(0), isVertexMoving(false)
{
    // set the scene size
    this->setSceneRect(QRectF(0, 0, 400, 600));
}

FloorScene::~FloorScene()
{
    
}

void FloorScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) {
    if (mouseEvent->button() ==  Qt::RightButton){
        // if user use right click + ctrl
        if(mouseEvent->modifiers() == Qt::ControlModifier){
            addVertex(mouseEvent->lastScenePos().toPoint());
        // if user use right click + shift
        } else if (mouseEvent->modifiers() == Qt::ShiftModifier) {
            removeVertex();
        } else {
            unsigned int floorPlanSize = meshManager->getFloorPlanSize();
            
            // we have have at least one vertex on the floor plan, if we click on it, we can move it
            // or if we do not have clicked on a vertex, but on an edge, we load his profile
            if (floorPlanSize != 0) {
                moveVertexOrLoadProfile();
                // else if we do not have any vertices on the floor plan, we create a floor plan
            } else {
                QPoint mousePos = mouseEvent->lastScenePos().toPoint();
                // no point defined thus we add an initial geometric structure
                basicCircle(&mousePos, 5);
                //basicCross(0.0f, 0.0f);
            }
        }
    }
}

void FloorScene::moveVertexOrLoadProfile() {
    unsigned int floorPlanSize = meshManager->getFloorPlanSize();
    // find which vertex we have selected
    Vertex* currentVertex = meshManager->getFloorPlan();
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        QGraphicsEllipseItem* ellipse = currentVertex->getEllipse();
        if (ellipse->isUnderMouse()) {
            isVertexMoving = true;
            currentlyMovingVertex = currentVertex;
            // tell the meshManager to generate new point/triangle
            meshManager->setLongUpdateOnMesh(true);
            return;
        }
        currentVertex = currentVertex->getNeighbor2();
    }
    
    // or which edge
    currentVertex = meshManager->getFloorPlan();
    Edge* currentEdge = currentVertex->getEdge2();
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        if (currentEdge->getLineItem()->isUnderMouse()) {
            newProfileSelected(currentEdge->getProfile());
            return;
        }
        currentVertex = currentVertex->getNeighbor2();
        currentEdge = currentVertex->getEdge2();
    }
}

void FloorScene::removeVertex() {
    //remove point if clicked on a point
    Vertex* floorplan = meshManager->getFloorPlan();
    unsigned int floorPlanSize = meshManager->getFloorPlanSize();
    
    // we want at least 3 vertices on the floor plan
    if (floorPlanSize <= 3) {
        return;
    }
    
    Vertex* currentVertex = floorplan;
    
    //find if a vertex is under the mouse
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        QGraphicsEllipseItem* ellipse = currentVertex->getEllipse();
        
        if (ellipse->isUnderMouse()) {
            // first set the floorplan pointer in meshManager to be the neighbor of the
            // currently deleted vertex. Because it can be the one pointed in
            // the meshManager class used to iterate over the floorplan
            meshManager->setFloorPlan(currentVertex->getNeighbor2());
            
            // we find one vertex to remove under the mouse, we remove it
            Edge* newEdge = currentVertex->removeVertex();
            Profile* profile = new Profile(false);
            newEdge->setProfile(profile);
            ProfileDestructorManager::putProfile(profile);
            
            Edge* oldEdge1 = currentVertex->getEdge1();
            Edge* oldEdge2 = currentVertex->getEdge2();
            this->removeItem(ellipse);
            this->removeItem(oldEdge1->getLineItem());
            this->removeItem(oldEdge2->getLineItem());
            
            this->addItem(newEdge->computeLineItem());
            
            delete oldEdge1->getLineItem();
            delete oldEdge1;
            delete oldEdge2->getLineItem();
            delete oldEdge2;
            delete ellipse;
            delete currentVertex;
            
            
            meshManager->decrementFloorPlanSize();
            
            // tell the meshManager to generate new point/triangle
            meshManager->setUpdateOnMesh();
            
            break;
        }
        currentVertex = currentVertex->getNeighbor2();
    }
}

void FloorScene::addVertex(QPoint mousePos) {
    //add point if clicked on an edge
    Vertex* floorplan = meshManager->getFloorPlan();
    unsigned int floorPlanSize = meshManager->getFloorPlanSize();
    
    Vertex* previousVertex = floorplan;
    Vertex* nextVertex;
    
    float x(0.0f);
    float y(0.0f);
    
    Edge* currentEdge(0);
    // find on which edge we have clicked
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        if (previousVertex->getEdge2()->getLineItem()->isUnderMouse()){
            nextVertex = previousVertex->getNeighbor2();
            currentEdge = previousVertex->getEdge2();
            x = mousePos.x();
            y = mousePos.y();
            break;
        }
        previousVertex = previousVertex->getNeighbor2();
    }
    
    // if we have clicked on an edge, we add a vertex
    if(currentEdge != 0){
        QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(x, y, thisSize.width(), thisSize.height());
        
        //build the new vertex and add the edges between the new neighbour
        Vertex* newVertex = new Vertex(x, y);
        newVertex->setEllipse(ellipse);
        
        Edge* edge1 = previousVertex->replaceNeighbour(nextVertex, newVertex);
        Edge* edge2 = nextVertex->replaceNeighbour(previousVertex, newVertex);
        
        edge1->setProfile(currentEdge->getProfile());
        edge2->setProfile(currentEdge->getProfile());
        
        //set all neighbour/edges of the new vertex
        newVertex->setNeighbor1(previousVertex); //addNeighbor
        newVertex->setEdge1(edge1);
        newVertex->setNeighbor2(nextVertex); //addNeighbor
        newVertex->setEdge2(edge2);
        
        //finally add the new vertex to the meshManager, show the ellipse and delete the old edge
        meshManager->incrementFloorPlanSize();
        this->addItem(ellipse);
        this->addItem(edge1->computeLineItem());
        this->addItem(edge2->computeLineItem());
        this->removeItem(currentEdge->getLineItem());
        delete currentEdge;
        // tell the meshManager to generate new point/triangle
        meshManager->setUpdateOnMesh();
    }
}

void FloorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    
    if (event->button() ==  Qt::RightButton) {
        if (isVertexMoving) {
            // then the vertex will not move anymore when we move the mouse
            isVertexMoving = false;
            currentlyMovingVertex = 0;
            // tell the meshManager to generate new point/triangle
            meshManager->setLongUpdateOnMesh(false);
        }
    }
}

void FloorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isVertexMoving) {
        QPoint mousePos = event->lastScenePos().toPoint();
        float x = mousePos.x();
        float y = mousePos.y();
        
        //update the ellipse coordinate of the currently moving vertex
        QGraphicsEllipseItem* ellipse = currentlyMovingVertex->getEllipse();
        ellipse->setRect(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
        
        //update the coordinate of the two edges of the currently moving vertex
        Vertex* neighbor1 = currentlyMovingVertex->getNeighbor1();
        Edge* edge1 = currentlyMovingVertex->getEdge1();
        edge1->getLineItem()->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                                      neighbor1->getEllipse()->rect().center().rx(), neighbor1->getEllipse()->rect().center().ry());
        
        Vertex* neighbor2 = currentlyMovingVertex->getNeighbor2();
        Edge* edge2 = currentlyMovingVertex->getEdge2();
        edge2->getLineItem()->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                                      neighbor2->getEllipse()->rect().center().rx(), neighbor2->getEllipse()->rect().center().ry());
        
        //update the coordinate of the currently moving vertex
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(x, y, thisSize.width(), thisSize.height());
        std::cerr << "Vertex moved: " << x << ", " << y << std::endl;
        currentlyMovingVertex->setX(x);
        currentlyMovingVertex->setY(y);
    }
}

void FloorScene::basicCross(float centerX, float centerY) {
    float halfSquareSize= 0.1f;
    float halfRectSize = 0.4f;

    Profile* commonProfile = new Profile(false);
    ProfileDestructorManager::putProfile(commonProfile);

    Vertex* x1 = new Vertex(centerX + halfSquareSize, centerY + halfSquareSize );
    Vertex* x11 = new Vertex(centerX + halfSquareSize  + halfRectSize, centerY + halfSquareSize );
    Vertex* x12 = new Vertex(centerX + halfSquareSize  + halfRectSize, centerY - halfSquareSize );

    Vertex* x2 = new Vertex(centerX + halfSquareSize , centerY - halfSquareSize );
    Vertex* x21 = new Vertex(centerX + halfSquareSize, centerY - halfSquareSize  - halfRectSize );
    Vertex* x22 = new Vertex(centerX - halfSquareSize, centerY - halfSquareSize  - halfRectSize );

    Vertex* x3 = new Vertex(centerX - halfSquareSize, centerY - halfSquareSize );
    Vertex* x31 = new Vertex(centerX - halfSquareSize  - halfRectSize, centerY - halfSquareSize );
    Vertex* x32 = new Vertex(centerX - halfSquareSize  - halfRectSize, centerY + halfSquareSize  );

    Vertex* x4 = new Vertex(centerX - halfSquareSize, centerY + halfSquareSize );
    Vertex* x41 = new Vertex(centerX - halfSquareSize, centerY + halfSquareSize  + halfRectSize );
    Vertex* x42 = new Vertex(centerX + halfSquareSize, centerY + halfSquareSize  + halfRectSize );

    Vertex* array[12] = {x1, x11, x12, x2, x21, x22, x3, x31, x32, x4, x41, x42};

    for(unsigned int i(0); i < 12; ++i) {
        Vertex* currentVertex = array[i];
        Vertex* nextVertex;
        if (i < 11) {
            nextVertex = array[i+1];
        } else {
            nextVertex = array[0];
        }

        Edge* edge = new Edge(currentVertex, nextVertex, commonProfile);
        currentVertex->setEdge2(edge);
        nextVertex->setEdge1(edge);
        currentVertex->setNeighbor2(nextVertex);
        nextVertex->setNeighbor1(currentVertex);

        meshManager->incrementFloorPlanSize();
    }

    meshManager->setFloorPlan(x1);
    newProfileSelected(commonProfile);
    loadFloorPlan();
}

void FloorScene::basicCircle(QPoint * mousePos, int numSample) {
    
    /*Profile* commonProfile = new Profile(false);
    ProfileDestructorManager::putProfile(commonProfile);
    
    QRectF thisSize = this->sceneRect();
    int radius(thisSize.width()/4);
    float x(0.0f);
    float y(0.0f);
    float alpha(0.0f);
    
    Vertex* firstVertex;
    Vertex* previousVertex;
    Vertex* currentVertex;
    
    // define a floor plan by taking samples on a circle equation
    for(int i(0); i < numSample; i++)
    {
        alpha = 2.0f * M_PI * (float)(i) / (float)(numSample);
        x = mousePos->x() + radius * cos(alpha);
        y = mousePos->y() + radius * sin(alpha);
        
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(x, y, thisSize.width(), thisSize.height());
        
        if (i == 0) {
            firstVertex = new Vertex(x, y);
            meshManager->setFloorPlan(firstVertex);
            meshManager->incrementFloorPlanSize();
            previousVertex = firstVertex;
        } else {
            currentVertex = new Vertex(x, y);
            meshManager->incrementFloorPlanSize();
            previousVertex->setNeighbor2(currentVertex);
            currentVertex->setNeighbor1(previousVertex);
            
            Edge* edge = new Edge(previousVertex, currentVertex, commonProfile);
            previousVertex->setEdge2(edge);
            currentVertex->setEdge1(edge);
            
            previousVertex = currentVertex;
        }
    }
    
    currentVertex->setNeighbor2(firstVertex);
    firstVertex->setNeighbor1(currentVertex);
    
    Edge* edge = new Edge(currentVertex, firstVertex, commonProfile);
    currentVertex->setEdge2(edge);
    firstVertex->setEdge1(edge);
    
    newProfileSelected(commonProfile);
    loadFloorPlan();*/



    ///////////////////////
    // test

    QRectF thisSize = this->sceneRect();


    meshManager->incrementFloorPlanSize();
    meshManager->incrementFloorPlanSize();
    meshManager->incrementFloorPlanSize();
    meshManager->incrementFloorPlanSize();
    meshManager->incrementFloorPlanSize();


    Vertex* a = new Vertex(-1.0, 1.0);
    Vertex* b = new Vertex(1.0, 1.0);//new Vertex(0.815, 1.025);
    Vertex* c = new Vertex(1.0, -1.0);
    Vertex* d = new Vertex(-1.0, -1.0);
    Vertex* e = new Vertex(-0.11, -0.175);//new Vertex(0.0, 0.0);



    meshManager->setFloorPlan(a);


    a->setNeighbor1(e);
    a->setNeighbor2(b);
    b->setNeighbor1(a);
    b->setNeighbor2(c);
    c->setNeighbor1(b);
    c->setNeighbor2(d);
    d->setNeighbor1(c);
    d->setNeighbor2(e);
    e->setNeighbor1(d);

    e->setNeighbor2(a);

    Profile* p = new Profile(false);
    Edge* ab = new Edge(a, b, p);
    Edge* bc = new Edge(b, c, p);
    Edge* cd = new Edge(c, d, p);
    Edge* de = new Edge(d, e, p);

    Edge* ea = new Edge(e, a, p);

    a->setEdge1(ea);
    a->setEdge2(ab);
    b->setEdge1(ab);
    b->setEdge2(bc);
    c->setEdge1(bc);
    c->setEdge2(cd);
    d->setEdge1(cd);
    d->setEdge2(de);
    e->setEdge1(de);
    e->setEdge2(ea);



    newProfileSelected(p);
    loadFloorPlan();
}

void FloorScene::newProfileSelected(Profile * p) {
    meshManager->setCurrentProfile(p);
    emit newProfileSelected();
}

void FloorScene::loadFloorPlan() {
    // remove the old floor plan
    this->clear();
    
    // get the new floor plan
    Vertex* floorplan = meshManager->getFloorPlan();
    unsigned int floorPlanSize = meshManager->getFloorPlanSize();
    float x(0.0f);
    float y(0.0f);
    Vertex* currentVertex = floorplan;
    
    // go through the vertices of the floor plan, create and draw their ellipse
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        x = currentVertex->getX();
        y = currentVertex->getY();
        
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinates3DToScene(x, y, thisSize.width(), thisSize.height());
        if(currentVertex->getEllipse() == 0){
            QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
            currentVertex->setEllipse(ellipse);
            this->addItem(currentVertex->getEllipse());
        } else {
            std::cerr << "something strange happen, try to reload an already loaded floorplan maybe is ok and i shouldn't do anything";
        }
        
        currentVertex = currentVertex->getNeighbor2();
    }
    
    // go through the vertices of the floor plan, create and draw their edges item
    currentVertex = floorplan;
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        if (currentVertex->getEdge2()->getLineItem() == 0) {
            this->addItem(currentVertex->getEdge2()->computeLineItem());
        }
        currentVertex = currentVertex->getNeighbor2();
    }
    
    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}
