#include "ProfileScene.h"
//#include <iostream>

// Tres similaire a floorScene pour le moment mais je garde 2 classe au cas ou faut tout changer..
const int ProfileScene::vertexRadius(3);

// il va prendre de mesh le profile courant a modifier
ProfileScene::ProfileScene(Mesh* mesh)
    : QGraphicsScene(), mesh(mesh), isProfileSelected(false), currentProfile(0)
    , currentlyMovingVertex(0), isVertexMoving(false)
{
    this->setSceneRect(QRectF(0, 0, 400, 600));
}

ProfileScene::~ProfileScene()
{

}

void ProfileScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

    if (!isProfileSelected) {
        return;
    }

    if (mouseEvent->button() ==  Qt::RightButton){
        if(ctrl_pressed){
            addVertex(mouseEvent->lastScenePos().toPoint());
        } else if (shift_pressed) {
            //remove point if clicked on a point
            removeVertex();
        } else {
            moveVertex();
        }
    }
}

void ProfileScene::addVertex(QPoint mousePos)
{
    //if clicked on an edge, add point on this edge
    Edge* currentEdge(0);
    bool foundEdge(false);
    Profile* profile = currentProfile;
    Vertex* currentVertex = profile->getProfileVertex();

    while(currentVertex->getNeighbor2() != 0) {
        currentEdge = currentVertex->getEdge2();
        if (currentEdge->getLineItem()->isUnderMouse()) {
            foundEdge = true;
            break;
        }
        currentVertex = currentVertex->getNeighbor2();
    }

    float w = mousePos.x();
    float z = mousePos.y();


    if (foundEdge) {
        QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(w - vertexRadius, z - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(w, z, thisSize.width(), thisSize.height());

        //build the new vertex and add the edges between the new neighbour
        Vertex* newVertex = new Vertex(w, z);
        newVertex->setEllipse(ellipse);

        Vertex* nextVertex = currentVertex->getNeighbor2();
        Vertex* previousVertex = currentVertex;

        Edge* edge1 = previousVertex->replaceNeighbour(nextVertex, newVertex);
        Edge* edge2 = nextVertex->replaceNeighbour(previousVertex, newVertex);


        //set all neighbour/edges of the new vertex
        newVertex->setNeighbor1(previousVertex);//addNeighbor
        newVertex->setEdge1(edge1);
        newVertex->setNeighbor2(nextVertex);//addNeighbor
        newVertex->setEdge2(edge2);


        //finally show the ellipse and delete the old edge
        this->addItem(ellipse);
        this->addItem(edge1->computeLineItem());
        this->addItem(edge2->computeLineItem());
        this->removeItem(currentEdge->getLineItem());
        delete currentEdge;

        // tell the mesh to generate new point/triangle
        mesh->setUpdateOnMesh();
    } else {
        // we don't have clicked on an edge, we will put the point at the current position

        Vertex * newVertex = new Vertex(0,0);

        newVertex->setEllipse(new QGraphicsEllipseItem(w - vertexRadius, z - vertexRadius, vertexRadius * 2.0f, vertexRadius*2.0f));
        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(w, z, thisSize.width(), thisSize.height());
        newVertex->setX(w);
        newVertex->setY(z);

        this->currentProfile->addVertexEnd(newVertex);
        this->addItem(newVertex->getEllipse());
        this->addItem(newVertex->getEdge1()->computeLineItem());

        // tell the mesh to generate new point/triangle
        mesh->setUpdateOnMesh();
    }
}

//remove point if clicked on a point
void ProfileScene::removeVertex()
{
    Vertex* currentVertex = currentProfile->getProfileVertex();

    bool foundVertex(false);
    QGraphicsEllipseItem* ellipse;
    while(currentVertex != 0) {
        ellipse = currentVertex->getEllipse();
        if (ellipse->isUnderMouse()) {
            foundVertex = true;
            break;
        }
        currentVertex = currentVertex->getNeighbor2();
    }

    // cannot remove if we do not find the vertex or if it is the first one
    if (foundVertex && currentVertex->getNeighbor1() != 0) {
        Edge* newEdge = currentVertex->removeVertex();
        this->removeItem(ellipse);
        Edge* oldEdge1 = currentVertex->getEdge1();
        Edge* oldEdge2 = currentVertex->getEdge2();

        if (currentVertex->getNeighbor1() != 0) {
            this->removeItem(oldEdge1->getLineItem());
            delete oldEdge1->getLineItem();
            delete oldEdge1;
        }
        if (currentVertex->getNeighbor2() != 0) {
            this->removeItem(oldEdge2->getLineItem());
            delete oldEdge2->getLineItem();
            delete oldEdge2;
        }
        if (newEdge != 0) {
            this->addItem(newEdge->computeLineItem());
        }
        delete ellipse;
        delete currentVertex;

        // tell the mesh to generate new point/triangle
        mesh->setUpdateOnMesh();
    }
}

void ProfileScene::moveVertex()
{
    // we have have at least one vertex on the profile plan, if we click on it, we can move it
    if (isProfileSelected){
        Vertex* currentVertex = currentProfile->getProfileVertex();
        // find which vertex we have selected
        while(currentVertex != 0) {
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
    }
}

void ProfileScene::keyPressEvent(QKeyEvent* keyEvent)
{
    if (!isProfileSelected) {
        return;
    }

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

void ProfileScene::keyReleaseEvent(QKeyEvent *event)
{
    if (!isProfileSelected) {
        return;
    }

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

void ProfileScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isProfileSelected) {
        return;
    }

    if (event->button() ==  Qt::RightButton) {
        if (isVertexMoving) {
            isVertexMoving = false;
            currentlyMovingVertex = 0;
            mesh->setLongUpdateOnMesh(false);
        }
    }
}

void ProfileScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isProfileSelected) {
        return;
    }

    if (isVertexMoving) {
        QPoint mousePos = event->lastScenePos().toPoint();
        float x = mousePos.x();
        float y = mousePos.y();


        QGraphicsEllipseItem* ellipse = currentlyMovingVertex->getEllipse();
        ellipse->setRect(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);

        Vertex* neighbor1 = currentlyMovingVertex->getNeighbor1();
        if (neighbor1 != 0){
            Edge* edge1 = currentlyMovingVertex->getEdge1();
            edge1->getLineItem()->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                       neighbor1->getEllipse()->rect().center().rx(), neighbor1->getEllipse()->rect().center().ry());
        }

        Vertex* neighbor2 = currentlyMovingVertex->getNeighbor2();
        if (neighbor2 != 0) {
            Edge* edge2 = currentlyMovingVertex->getEdge2();
            edge2->getLineItem()->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                       neighbor2->getEllipse()->rect().center().rx(), neighbor2->getEllipse()->rect().center().ry());
        }

        QRectF thisSize = this->sceneRect();
        Utils::adjustCoordinatesSceneTo3D(x, y, thisSize.width(), thisSize.height());
        currentlyMovingVertex->setX(x);
        currentlyMovingVertex->setY(y);
    }
}

// en faire un slot pour que la floorScene puisse l appeler
void ProfileScene::newProfileSelected()
{
    isProfileSelected = true;
    loadProfile();
}

void ProfileScene::loadProfile()
{
    if (currentProfile != 0) {

        Vertex* currentVertex = currentProfile->getProfileVertex();

        while(currentVertex != 0){
            this->removeItem(currentVertex->getEllipse());
            if (currentVertex->getNeighbor2() != 0) {
                this->removeItem(currentVertex->getEdge2()->getLineItem());
            }

            currentVertex = currentVertex->getNeighbor2();
        }
    }


    currentProfile = mesh->getCurrentProfile();

    Vertex* currentVertex = currentProfile->getProfileVertex();

    float x(0.0f);
    float y(0.0f);
    while(currentVertex != 0)
    {
        if(currentVertex->getEllipse() == 0){
            x = currentVertex->getX();
            y = currentVertex->getY();
            QRectF thisSize = this->sceneRect();
            Utils::adjustCoordinates3DToScene(x, y, thisSize.width(), thisSize.height());

            currentVertex->setEllipse(new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f));
            this->addItem(currentVertex->getEllipse());

        }else {
            this->addItem(currentVertex->getEllipse());
            if (currentVertex->getNeighbor2() != 0) {
                this->addItem(currentVertex->getEdge2()->getLineItem());
            }
        }
        currentVertex = currentVertex->getNeighbor2();
    }

    currentVertex = currentProfile->getProfileVertex();

    // go through the vertices of the floor plan, create and draw their edges item
    while(currentVertex->getNeighbor2() != 0){
        if (currentVertex->getEdge2()->getLineItem() == 0) {
            this->addItem(currentVertex->getEdge2()->computeLineItem());
        }
        currentVertex = currentVertex->getNeighbor2();
    }

    // now we can delete the old profiles from the previous mesh
    // take the old profiles
    ProfileDestructorManager::swap();
    // destroy them
    ProfileDestructorManager::deleteProfiles();
    // take again the current profiles
    ProfileDestructorManager::swap();
}

