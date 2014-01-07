#include "ProfileMergeScene.h"

ProfileMergeScene::ProfileMergeScene(std::vector<Vertex *> *newProfile, MeshManager* meshManager, bool firstScene)
    :QGraphicsScene(), meshManager(meshManager), newProfile(newProfile), firstScene(firstScene), profile(0)
{
    this->setSceneRect(QRectF(0, 0, 4000, 6000));

    oldLinePen.setWidth(3);
    oldEllipsePen.setWidth(5);
    oldEllipsePen.setColor(Qt::white);
    oldEllipseBrush.setColor(Qt::red);
    oldEllipseBrush.setStyle(Qt::SolidPattern);

    loadProfile();
}

void ProfileMergeScene::revert() {
    // for each profile, revert its color
    foreach(Profile* profile, profileToRevertColor) {
        Vertex* iterator = profile->getProfileVertex();
        while(iterator != 0) {
            //revert the ellipse color/brush
            QGraphicsEllipseItem* ellipse = iterator->getEllipse();
            ellipse->setPen(oldEllipsePen);
            ellipse->setBrush(oldEllipseBrush);

            if(iterator->getNeighbor2() != 0) {
                QGraphicsLineItem* lineItem = iterator->getEdge2()->getLineItem();
                // revert the line item pen
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

    //find which profile we must load
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

    QList<QGraphicsItem*> items = this->items();
    foreach(QGraphicsItem* item, items) {
        this->removeItem(item);
    }

    // this profile will have its color/brush modified
    profileToRevertColor.push_back(profile);

    // load the profile vertices
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

        // load the vertex
        this->addItem(ellipse);

        iterator->setValid(true);
        iterator = iterator->getNeighbor2();
    }

    // load the profile edges
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

                // if the selected vertex is correct
                if(valid) {
                    //the currently selected vertex will be part of the merged profile
                    newProfile->push_back(iterator);
                    // this vertex cannot be selected again
                    iterator->invalid();

                    // show the selected vertices
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
