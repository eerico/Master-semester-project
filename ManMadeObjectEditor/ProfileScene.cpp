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

    /*{//TEST PRINT FLOORPLAN
        Profile* profile = currentProfile;
        Vertex* v = profile->getProfileVertex();
        std::cout << "PROFILE: format: :: neighbor1 <- edge1 -> ||curent|| <- neighbor2 -> edge2 :: " << std::endl;
        while(v != 0) {
            std::cout << ":: " << v->getNeighbor1() << " <- " << v->getEdge1() << " -> ||" << v << "|| <- " << v->getEdge2() << " -> " << v->getNeighbor2() << " ::";
            v = v->getNeighbor2();
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }*/


<<<<<<< HEAD
    if (mouseEvent->button() ==  Qt::LeftButton){
=======
    if (mouseEvent->button() ==  Qt::RightButton){
>>>>>>> Erico
        if(ctrl_pressed){
            //add point

            //if clicked on an edge, add point on this edge
            QGraphicsLineItem* currentEdge = 0;
            bool foundEdge(false);
            Profile* profile = currentProfile;
            Vertex* currentVertex = profile->getProfileVertex();

            while(currentVertex->getNeighbor2() != 0) {
                currentEdge = currentVertex->getEdge2();
                if (currentEdge->isUnderMouse()) {
                    foundEdge = true;
                    break;
                }
                currentVertex = currentVertex->getNeighbor2();
            }

            QPoint mousePos = mouseEvent->lastScenePos().toPoint();
            float w = mousePos.x();
            float z = mousePos.y();


            if (foundEdge) {
                QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(w - vertexRadius, z - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
                adjustCoordinatesSceneTo3D(w, z);

                //build the new vertex and add the edges between the new neighbour
                Vertex* newVertex = new Vertex(w, z);
                newVertex->setEllipse(ellipse);

                Vertex* nextVertex = currentVertex->getNeighbor2();
                Vertex* previousVertex = currentVertex;
                QGraphicsLineItem* edge1 = previousVertex->replaceNeighbour(nextVertex, newVertex);
                QGraphicsLineItem* edge2 = nextVertex->replaceNeighbour(previousVertex, newVertex);


                //set all neighbour/edges of the new vertex
                newVertex->setNeighbor1(previousVertex);//addNeighbor
                newVertex->setEdge1(edge1);
                newVertex->setNeighbor2(nextVertex);//addNeighbor
                newVertex->setEdge2(edge2);


                //finally show the ellipse and delete the old edge
                this->addItem(ellipse);
                this->addItem(edge1);
                this->addItem(edge2);
                this->removeItem(currentEdge);
                delete currentEdge;
            } else {
                // we don't have clicked on an edge, we will put the point at the current position

                Vertex * newVertex = new Vertex(0,0);

                newVertex->setEllipse(new QGraphicsEllipseItem(w - vertexRadius, z - vertexRadius, vertexRadius * 2.0f, vertexRadius*2.0f));
                adjustCoordinatesSceneTo3D(w, z);
                newVertex->setX(w);
                newVertex->setY(z);

                this->currentProfile->addVertexEnd(newVertex);
                this->addItem(newVertex->getEllipse());
                this->addItem(newVertex->getEdge1());
            }

        } else if (shift_pressed) {
            //remove point if clicked on a point

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
                QGraphicsLineItem* newEdge = currentVertex->removeVertex();
                this->removeItem(ellipse);
                QGraphicsLineItem* oldEdge1 = currentVertex->getEdge1();
                QGraphicsLineItem* oldEdge2 = currentVertex->getEdge2();

                if (currentVertex->getNeighbor1() != 0) {
                    this->removeItem(oldEdge1);
                    delete oldEdge1;
                }
                if (currentVertex->getNeighbor2() != 0) {
                    this->removeItem(oldEdge2);
                    delete oldEdge2;
                }
                if (newEdge != 0) {
                    this->addItem(newEdge);
                }
                delete ellipse;
                delete currentVertex;
            }
        } else {
            // we have have at least one vertex on the profile plan, if we click on it, we can move it
            if (isProfileSelected){
                Vertex* currentVertex = currentProfile->getProfileVertex();
                // find which vertex we have selected
                while(currentVertex != 0) {
                    QGraphicsEllipseItem* ellipse = currentVertex->getEllipse();
                    if (ellipse->isUnderMouse()) {
                        isVertexMoving = true;
                        currentlyMovingVertex = currentVertex;
                        break;
                    }
                    currentVertex = currentVertex->getNeighbor2();
                }
            }
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

    if (event->button() ==  Qt::LeftButton) {
        if (isVertexMoving) {
            isVertexMoving = false;
            currentlyMovingVertex = 0;
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
            QGraphicsLineItem* edge1 = currentlyMovingVertex->getEdge1();
            edge1->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                       neighbor1->getEllipse()->rect().center().rx(), neighbor1->getEllipse()->rect().center().ry());
        }

        Vertex* neighbor2 = currentlyMovingVertex->getNeighbor2();
        if (neighbor2 != 0) {
            QGraphicsLineItem* edge2 = currentlyMovingVertex->getEdge2();
            edge2->setLine(ellipse->rect().center().rx(), ellipse->rect().center().ry(),
                       neighbor2->getEllipse()->rect().center().rx(), neighbor2->getEllipse()->rect().center().ry());
        }

        adjustCoordinatesSceneTo3D(x, y);
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

    // clean profile before drawing he new one
    // maybe there is a better way to do it..
    this->clear(); // if I understand correctly, this method already delete the ellipse in our scene, we have to create new one
    if (currentProfile != 0) {

        Vertex* currentVertex = currentProfile->getProfileVertex();

        while(currentVertex != 0){
            // no need to delete edges and ellipse
            // the clear method should already have done this operation
            currentVertex->setEdge1(0);
            currentVertex->setEdge2(0);
            currentVertex->setEllipse(0);

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
                adjustCoordinates3DToScene(x, y);

                currentVertex->setEllipse(new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f));
                this->addItem(currentVertex->getEllipse());

            }else {
                std::cerr << "something strange happen, try to reload an already loaded profile maybe is ok and i shouldn't do anything";
            }
            currentVertex = currentVertex->getNeighbor2();
        }


        currentVertex = currentProfile->getProfileVertex();

        while(currentVertex->getNeighbor2() != 0)
        {
            if(currentVertex->addEdge2()){
               currentVertex->getNeighbor2()->setEdge1(currentVertex->getEdge2());
               this->addItem(currentVertex->getEdge2());
            }
            currentVertex = currentVertex->getNeighbor2();
        }

}

void ProfileScene::adjustCoordinates3DToScene(float& w, float& z)
{
    // from 3D to Scene coord
    QRectF thisSize = this->sceneRect();
    float width = thisSize.width();
    float height = thisSize.height();

    /*w += 1.0f;
    z += 1.0f;
    w = w * width / 2.0f;
    z = height - z * width / 2.0f /*+ height * 0.25f*/;

    float width_2 = width/ 2.0f;
    float height_2 = height / 2.0f;

    w = w * width_2;
    z = z * width_2;

    z = height_2 - z;
    w = w + width_2;
}

void ProfileScene::adjustCoordinatesSceneTo3D(float& w, float& z)
{
    // from scene coord to 3D
    QRectF thisSize = this->sceneRect();
    float width = thisSize.width();
    float height = thisSize.height();
    float width_2 = width/ 2.0f;
    float height_2 = height / 2.0f;

    z = height_2  - z;
    w = -width_2 + w;

    w = w/width_2;
    z = z/width_2;
}

