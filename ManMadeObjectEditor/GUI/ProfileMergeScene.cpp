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
    foreach(Profile* profile, profileToRevertColor) {
        Vertex* iterator = profile->getProfileVertex();
        while(iterator != 0) {
            QGraphicsEllipseItem* ellipse = iterator->getEllipse();
            ellipse->setPen(oldEllipsePen);
            ellipse->setBrush(oldEllipseBrush);

            if(iterator->getNeighbor2() != 0) {
                QGraphicsLineItem* lineItem = iterator->getEdge2()->getLineItem();
                lineItem->setPen(oldLinePen);
            }

            iterator = iterator->getNeighbor2();
        }
    }
}

void ProfileMergeScene::loadProfile() {
    Edge* currentEdgeSelected = meshManager->getEdgeSelected();
    Edge* previousEdgeSelected = meshManager->getPreviousEdgeSelected();

    if(currentEdgeSelected != 0 && previousEdgeSelected != 0) {
        // we dont want to merge the same profiles
        if(currentEdgeSelected->getProfile() == previousEdgeSelected->getProfile()) {
            return;
        }
    }

    if(firstScene) {
        if(currentEdgeSelected != 0) {
            profile = currentEdgeSelected ->getProfile();
        } else {
            return;
        }
    } else {
        if(previousEdgeSelected != 0 && currentEdgeSelected != 0) {
            profile = previousEdgeSelected->getProfile();
        } else {
            return;
        }
    }

    profileToRevertColor.push_back(profile);

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
    Edge* currentEdgeSelected = meshManager->getEdgeSelected();
    Edge* previousEdgeSelected = meshManager->getPreviousEdgeSelected();

    if(currentEdgeSelected == 0 || previousEdgeSelected == 0) {
        return;
    } else if (currentEdgeSelected->getProfile() == previousEdgeSelected->getProfile()) {
        return;
    }


    if (mouseEvent->button() ==  Qt::RightButton){
        Vertex* iterator = profile->getProfileVertex();
        // find on which vertex we have clicked
        while(iterator != 0){
            QGraphicsEllipseItem* ellipse = iterator->getEllipse();
            if (ellipse->isUnderMouse() && iterator->isValid()){
                //check validity (monotonicity => nothing at the same height)
                bool valid = true;
                foreach(Vertex* vertex, *newProfile) {
                    if(vertex->getY() == iterator->getY()) {
                        valid = false;
                        break;
                    }
                }
                if(iterator->getY() == 0) {
                    valid = false;
                }

                if(valid) {
                    newProfile->push_back(iterator);
                    iterator->invalid();

                    QPen pen = ellipse->pen();
                    pen.setColor(Qt::blue);
                    ellipse->setPen(pen);

                    QBrush brush = ellipse->brush();
                    brush.setColor(Qt::white);
                    ellipse->setBrush(brush);
                }

                break;
            }
            iterator = iterator->getNeighbor2();
        }
    }
}

void ProfileMergeScene::updateProfileSelected() {
    loadProfile();
}

Profile* ProfileMergeScene::getProfileToMerge() {
    return profile;
}
