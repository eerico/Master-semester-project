#include "FloorScene.h"
#include <iostream>

const int FloorScene::vertexRadius(3);

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
            // and we draw his profile
            if (floorPlanSize != 0) {
                moveVertex();
            // else if we do not have any vertices on the floor plan, we create a floor plan
            } else {
                QPoint mousePos = mouseEvent->lastScenePos().toPoint();
                // no point defined thus we add an initial geometric structure
                basicCircle(&mousePos, 10);
            }
        }
    }
}

void FloorScene::moveVertex()
{
    unsigned int floorPlanSize = mesh->getFloorPlanSize();
    // find which vertex we have selected
    Vertex* currentVertex = mesh->getFloorPlan();
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        QGraphicsEllipseItem* ellipse = currentVertex->getEllipse();
        if (ellipse->isUnderMouse()) {
            isVertexMoving = true;
            currentlyMovingVertex = (FloorVertex*)currentVertex;
            newProfileSelected(((FloorVertex*)currentVertex)->getProfile());

            // tell the mesh to generate new point/triangle
            mesh->setLongUpdateOnMesh(true);
            break;
        }
        currentVertex = currentVertex->getNeighbor2();
    }
}

void FloorScene::removeVertex()
{
    //remove point if clicked on a point
    FloorVertex* floorplan = mesh->getFloorPlan();
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
            mesh->setFloorPlan((FloorVertex*)currentVertex->getNeighbor2());

            // we find one vertex to remove under the mouse, we remove it
            QGraphicsLineItem* newEdge = currentVertex->removeVertex();
            QGraphicsLineItem* oldEdge1 = currentVertex->getEdge1();
            QGraphicsLineItem* oldEdge2 = currentVertex->getEdge2();
            this->removeItem(ellipse);
            this->removeItem(oldEdge1);
            this->removeItem(oldEdge2);

            //update normals of the two neighbor
            FloorVertex* neighbor1 = (FloorVertex*)currentVertex->getNeighbor1();
            FloorVertex* neighbor2 = (FloorVertex*)currentVertex->getNeighbor2();
            neighbor1->computeNormal();
            neighbor2->computeNormal();

            this->addItem(newEdge);

            delete oldEdge1;
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
    FloorVertex* floorplan = mesh->getFloorPlan();
    unsigned int floorPlanSize = mesh->getFloorPlanSize();

    FloorVertex* previousVertex = floorplan;
    FloorVertex* nextVertex;

    float x(0.0f);
    float y(0.0f);

    QGraphicsLineItem * currentEdge(0);
    // find on which edge we have clicked
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        if (previousVertex->getEdge2()->isUnderMouse()){
           nextVertex = (FloorVertex*)previousVertex->getNeighbor2();
           currentEdge = previousVertex->getEdge2();
           x = mousePos.x();
           y = mousePos.y();
           break;
        }
        previousVertex = (FloorVertex*)previousVertex->getNeighbor2();
    }

    // if we have clicked on an edge, we add a vertex
    if(currentEdge != 0){
        QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(x, y, thisSize.width(), thisSize.height());

        //build the new vertex and add the edges between the new neighbour
        FloorVertex* newVertex = new FloorVertex(x,y, previousVertex->getProfile());//comme choisir quel profil mettre ca c'est tres interessant...
        newVertex->setEllipse(ellipse);


        QGraphicsLineItem* edge1 = previousVertex->replaceNeighbour(nextVertex, newVertex);
        QGraphicsLineItem* edge2 = nextVertex->replaceNeighbour(previousVertex, newVertex);


        //set all neighbour/edges of the new vertex
        newVertex->setNeighbor1(previousVertex); //addNeighbor
        newVertex->setEdge1(edge1);
        newVertex->setNeighbor2(nextVertex); //addNeighbor
        newVertex->setEdge2(edge2);

        //set the normal of the new vertex.
        newVertex->computeNormal();
        //update normal of the neighbors
        previousVertex->computeNormal();
        nextVertex->computeNormal();

        //finally add the new vertex to the mesh, show the ellipse and delete the old edge
        mesh->incrementFloorPlanSize();
        this->addItem(ellipse);
        this->addItem(edge1);
        this->addItem(edge2);
        this->removeItem(currentEdge);
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
        FloorVertex* neighbor1 = (FloorVertex*)currentlyMovingVertex->getNeighbor1();
        QGraphicsLineItem* edge1 = currentlyMovingVertex->getEdge1();
        edge1->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                       neighbor1->getEllipse()->rect().center().rx(), neighbor1->getEllipse()->rect().center().ry());

        FloorVertex* neighbor2 = (FloorVertex*)currentlyMovingVertex->getNeighbor2();
        QGraphicsLineItem* edge2 = currentlyMovingVertex->getEdge2();
        edge2->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                       neighbor2->getEllipse()->rect().center().rx(), neighbor2->getEllipse()->rect().center().ry());

        //update the coordinate of the currently moving vertex
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(x, y, thisSize.width(), thisSize.height());
        currentlyMovingVertex->setX(x);
        currentlyMovingVertex->setY(y);

        //update the normals
        currentlyMovingVertex->computeNormal();
        neighbor1->computeNormal();
        neighbor2->computeNormal();
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

    FloorVertex* firstVertex;
    FloorVertex* previousVertex;
    FloorVertex* currentVertex;

    // define a floor plan by taking samples on a circle equation
    for(int i(0); i < numSample; i++)
    {
        alpha = 2.0f * M_PI * (float)(i) / (float)(numSample);
        x = mousePos->x() + radius * cos(alpha);
        y = mousePos->y() + radius * sin(alpha);

        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(x, y, thisSize.width(), thisSize.height());

        if (i == 0) {
            firstVertex = new FloorVertex(x, y, commonProfile);
            mesh->setFloorPlan(firstVertex);
            mesh->incrementFloorPlanSize();
            previousVertex = firstVertex;
        } else {
            currentVertex = new FloorVertex(x, y, commonProfile);
            mesh->incrementFloorPlanSize();
            previousVertex->setNeighbor2(currentVertex);//addNeighbor
            currentVertex->setNeighbor1(previousVertex);//addNeighbor

            previousVertex = currentVertex;
        }
    }
    currentVertex->setNeighbor2(firstVertex);//addNeighbor
    firstVertex->setNeighbor1(currentVertex);//addNeighbor
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

    FloorVertex* floorplan = mesh->getFloorPlan();
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
            currentVertex->setEllipse(new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f));
            this->addItem(currentVertex->getEllipse());
        } else {
            std::cerr << "something strange happen, try to reload an already loaded floorplan maybe is ok and i shouldn't do anything";
        }

        currentVertex = currentVertex->getNeighbor2();
    }

    // go through the vertices of the floor plan, create and draw their edges
    currentVertex = floorplan;
    for (unsigned int i(0); i < floorPlanSize; ++i) {
        if(currentVertex->addEdge1()){
            this->addItem(currentVertex->getEdge1());
        }
        if(currentVertex->addEdge2()){
           this->addItem(currentVertex->getEdge2());
        }
        currentVertex = currentVertex->getNeighbor2();
    }

    // tell the mesh to generate new point/triangle
    mesh->setUpdateOnMesh();
}


