#include "FloorScene.h"
#include <iostream>

const int FloorScene::vertexRadius(6);

FloorScene::FloorScene(Mesh* mesh)
    : QGraphicsScene(), ctrl_pressed(false), shift_pressed(false), mesh(mesh)
    , currentlyMovingVertex(0), isVertexMoving(false)
{
    this->setSceneRect(QRectF(0, 0, 400, 600));
}

FloorScene::~FloorScene()
{

}

void FloorScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->button() ==  Qt::RightButton){
        if(ctrl_pressed){
            addVertex(mouseEvent->lastScenePos().toPoint());
        } else if (shift_pressed) {
            removeVertex();
        } else {
            unsigned int floorPlanSize = mesh->getFloorPlanSize();

            // we have have at least one vertex on the floor plan, if we click on it, we can move it
            // or if we do not have clicked on a vertex, but on an edge, we load his profile
            if (floorPlanSize != 0) {
                moveVertexOrLoadProfile();
            // else if we do not have any vertices on the floor plan, we create a floor plan
            } else {
                QPoint mousePos = mouseEvent->lastScenePos().toPoint();
                // no point defined thus we add an initial geometric structure
                basicCircle(&mousePos, 10);
            }
        }
    }
}

void FloorScene::moveVertexOrLoadProfile()
{
    unsigned int floorPlanSize = mesh->getFloorPlanSize();
    // find which vertex we have selected    
    Vertex* currentVertex = mesh->getFloorPlan();
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        QGraphicsEllipseItem* ellipse = currentVertex->getEllipse();
        if (ellipse->isUnderMouse()) {
            isVertexMoving = true;
            currentlyMovingVertex = currentVertex;
            // tell the mesh to generate new point/triangle
            mesh->setLongUpdateOnMesh(true);
            return;
        }
        currentVertex = currentVertex->getNeighbor2();
    }

    // or which edge
    currentVertex = mesh->getFloorPlan();
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

void FloorScene::removeVertex()
{
    //remove point if clicked on a point
    Vertex* floorplan = mesh->getFloorPlan();
    unsigned int floorPlanSize = mesh->getFloorPlanSize();

    if (floorPlanSize <= 3) {
        return;
    }

    Vertex* currentVertex = floorplan;

    //find if a vertex is under the mouse
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        QGraphicsEllipseItem* ellipse = currentVertex->getEllipse();

        if (ellipse->isUnderMouse()) {
            // first set the floorplan pointer in mesh to be the neighbor of the
            // currently deleted vertex. Because it can be the one pointed in
            // the mesh class used to iterate over the floorplan
            mesh->setFloorPlan(currentVertex->getNeighbor2());

            // we find one vertex to remove under the mouse, we remove it
            Edge* newEdge = currentVertex->removeVertex();
            newEdge->setProfile(new Profile(false));
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


            mesh->decrementFloorPlanSize();

            // tell the mesh to generate new point/triangle
            mesh->setUpdateOnMesh();

            break;
        }
        currentVertex = currentVertex->getNeighbor2();
    }
}

void FloorScene::addVertex(QPoint mousePos)
{
    //add point if clicked on an edge
    Vertex* floorplan = mesh->getFloorPlan();
    unsigned int floorPlanSize = mesh->getFloorPlanSize();

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

        //finally add the new vertex to the mesh, show the ellipse and delete the old edge
        mesh->incrementFloorPlanSize();
        this->addItem(ellipse);
        this->addItem(edge1->computeLineItem());
        this->addItem(edge2->computeLineItem());
        this->removeItem(currentEdge->getLineItem());
        delete currentEdge;
        // tell the mesh to generate new point/triangle
        mesh->setUpdateOnMesh();
    }
}

void FloorScene::keyPressEvent(QKeyEvent* keyEvent)
{
    switch(keyEvent->key())
    {
        case Qt::Key_Control:
            ctrl_pressed = true;
            break;
        case Qt::Key_Shift:
            shift_pressed = true;
            break;
    }
}

void FloorScene::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Control:
            ctrl_pressed = false;
            break;
        case Qt::Key_Shift:
            shift_pressed = false;
            break;
    }
}

void FloorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    if (event->button() ==  Qt::RightButton) {
        if (isVertexMoving) {
            isVertexMoving = false;
            currentlyMovingVertex = 0;
            // tell the mesh to generate new point/triangle
            mesh->setLongUpdateOnMesh(false);
        }
    }
}

void FloorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
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
        currentlyMovingVertex->setX(x);
        currentlyMovingVertex->setY(y);
    }
}

void FloorScene::basicCircle(QPoint *mousePos, int numSample)
{

    Profile* commonProfile = new Profile(false);
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
            mesh->setFloorPlan(firstVertex);
            mesh->incrementFloorPlanSize();
            previousVertex = firstVertex;
        } else {
            currentVertex = new Vertex(x, y);
            mesh->incrementFloorPlanSize();
            previousVertex->setNeighbor2(currentVertex);//addNeighbor
            currentVertex->setNeighbor1(previousVertex);//addNeighbor

            Edge* edge = new Edge(previousVertex, currentVertex, commonProfile);
            previousVertex->setEdge2(edge);
            currentVertex->setEdge1(edge);

            previousVertex = currentVertex;
        }
    }

    currentVertex->setNeighbor2(firstVertex);//addNeighbor
    firstVertex->setNeighbor1(currentVertex);//addNeighbor

    Edge* edge = new Edge(currentVertex, firstVertex, commonProfile);
    currentVertex->setEdge2(edge);
    firstVertex->setEdge1(edge);

    newProfileSelected(commonProfile);
    loadFloorPlan();

    // tell the mesh to generate new point/triangle
    mesh->setUpdateOnMesh();
}

void FloorScene::newProfileSelected(Profile* p)
{
    mesh->setCurrentProfile(p);
    emit newProfileSelected();
}

void FloorScene::loadFloorPlan() {
    this->clear();

    Vertex* floorplan = mesh->getFloorPlan();
    unsigned int floorPlanSize = mesh->getFloorPlanSize();
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

    // tell the mesh to generate new point/triangle
    mesh->setUpdateOnMesh();
}


