#include "ProfileMergeScene.h"

ProfileMergeScene::ProfileMergeScene(std::vector<Vertex *> *newProfile, MeshManager* meshManager, bool firstScene)
    :QGraphicsScene(), meshManager(meshManager), newProfile(newProfile), firstScene(firstScene), profile(0)
{
    this->setSceneRect(QRectF(0, 0, 400, 600));

    oldLinePen.setWidth(3);
    oldEllipsePen.setWidth(5);
    oldEllipsePen.setColor(Qt::white);
    oldEllipseBrush.setColor(Qt::red);
    oldEllipseBrush.setStyle(Qt::SolidPattern);

    loadProfile();
}

void ProfileMergeScene::revert() {
    // revert color if needed
    if(profile != 0) {
        Vertex* iterator = profile->getProfileVertex();
        while(iterator != 0) {
            QGraphicsEllipseItem* ellipse = iterator->getEllipse();
            this->removeItem(ellipse);
            ellipse->setPen(oldEllipsePen);
            ellipse->setBrush(oldEllipseBrush);

            if(iterator->getNeighbor2() != 0) {
                QGraphicsLineItem* lineItem = iterator->getEdge2()->getLineItem();
                this->removeItem(lineItem);
                lineItem->setPen(oldLinePen);
            }

            iterator = iterator->getNeighbor2();
        }
    }
}

void ProfileMergeScene::loadProfile() {
    Edge* currentEdgeSelected = meshManager->getEdgeSelected();
    Edge* previousEdgeSelected = meshManager->getPreviousEdgeSelected();

    if(firstScene) {
        if(currentEdgeSelected != 0) {
            profile = currentEdgeSelected ->getProfile();
            // we dont want to merge the same profiles
            if(previousEdgeSelected != 0) {
                if(profile == previousEdgeSelected->getProfile()) {
                    return;
                }
            }
        } else {
            return;
        }
    } else {
        if(previousEdgeSelected != 0 && currentEdgeSelected != 0) {
            profile = previousEdgeSelected->getProfile();
            // we dont want to merge the same profiles
            if(profile == currentEdgeSelected->getProfile()){
                return;
            }
        } else {
            return;
        }
    }

    Vertex* iterator = profile->getProfileVertex();
    while(iterator != 0) {
        QGraphicsEllipseItem* ellipse = iterator->getEllipse();
        if(ellipse == 0) {
            float x = iterator->getX();
            float y = iterator->getY();
            QRectF thisSize = this->sceneRect();
            Utils::adjustCoordinates3DToScene(x, y, thisSize.width(), thisSize.height());
            int vertexRadius = 6;
            ellipse = new QGraphicsEllipseItem(x - vertexRadius, y - vertexRadius, vertexRadius * 2.0f, vertexRadius * 2.0f);
            iterator->setEllipse(ellipse);
        }

        QPen vertexPen;
        QBrush vertexBrush;
        vertexPen.setWidth(3);
        vertexPen.setColor(Qt::white);
        vertexBrush.setColor(Qt::black);
        vertexBrush.setStyle(Qt::SolidPattern);

        ellipse->setPen(vertexPen);
        ellipse->setBrush(vertexBrush);

        this->addItem(ellipse);

        iterator->setValid(true);
        iterator = iterator->getNeighbor2();
    }


    iterator = profile->getProfileVertex();
    while(iterator != 0) {
        if(iterator->getNeighbor2() != 0) {
            Edge*  currentEdge = iterator->getEdge2();
            QGraphicsLineItem* lineItem = currentEdge->getLineItem();
            if(lineItem == 0) {
                lineItem = currentEdge->computeLineItem();
            }

            QPen linePen;
            linePen.setWidth(3);
            QColor color = *profile->getProfileColorIdentification();
            linePen.setColor(color);
            lineItem->setPen(linePen);

            this->addItem(lineItem);
        }

        iterator = iterator->getNeighbor2();
    }
}

void ProfileMergeScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) {
    if (mouseEvent->button() ==  Qt::RightButton){
        /*Profile* profile = meshManager->getCurrentProfile();
        Vertex* iterator = profile->getProfileVertex();

        // find on which vertex we have clicked
        while(iterator != 0){
            QGraphicsEllipseItem* ellipse = iterator->getEllipse();
            if (ellipse->isUnderMouse() && iterator->isValid()){
                if(currentCurve->begin == 0) {
                    // we start a new curve
                    currentCurve->begin = iterator;
                    currentCurve->color.setRgb(std::rand()%256, std::rand()%256, std::rand()%256);

                    QPen pen = ellipse->pen();
                    pen.setColor(currentCurve->color);
                    ellipse->setPen(pen);

                    iterator->invalid();
                } else {
                    //check validity of the curve
                    Vertex* iterator2 = currentCurve->begin->getNeighbor2();
                    bool curveValid = true;
                    // check if all vertices between the begining and the end are valid
                    while(iterator2 != iterator && iterator2 != 0) {
                        if(!iterator2->isValid()) {
                            curveValid = false;
                        }
                        iterator2 = iterator2->getNeighbor2();
                    }
                    if(iterator2 == 0) {
                        // we didnt find the end
                        curveValid = false;
                    }

                    if(curveValid) {
                        //we finish the current curve
                        currentCurve->end = iterator;
                        curveArray->push_back(currentCurve);

                        //update the color indication between the begin vertex and the end vertex
                        updateSceneEllipseAndLineColor();

                        // construct the next curve
                        currentCurve = new Curve;
                    }
                }
                break;
            }
            iterator = iterator->getNeighbor2();
        }*/
    }
}

void ProfileMergeScene::updateProfileSelected() {
    loadProfile();
}

