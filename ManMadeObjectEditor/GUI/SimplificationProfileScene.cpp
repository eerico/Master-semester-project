#include "SimplificationProfileScene.h"

SimplificationProfileScene::SimplificationProfileScene(MeshManager *meshManager, std::vector< Curve* >* curveArray)
    : SimplificationScene(meshManager, curveArray)
{
    this->setSceneRect(QRectF(0, 0, 4000, 6000));
    loadProfile();

    // the first curve
    currentCurve = new Curve;
}

SimplificationProfileScene::~SimplificationProfileScene() {
    // delete all created data
    foreach(Curve* curve, *curveArray) {
        delete curve;
    }
    delete currentCurve;

    Profile* profile = meshManager->getCurrentProfile();
    Vertex* iterator = profile->getProfileVertex();
    while(iterator != 0){
        iterator->setValid(true);
        iterator = iterator->getNeighbor2();
    }
}

void SimplificationProfileScene::revertColor() {
    Profile* profile = meshManager->getCurrentProfile();
    Vertex* iterator = profile->getProfileVertex();
    int i(0);
    // revert every ellipse and line item pen and brush
    while(iterator != 0){
        QGraphicsEllipseItem* ellipse = iterator->getEllipse();

        QPen penEllipse = oldPenEllipse[i];
        QBrush penBrush = oldBrushEllipse[i];

        ellipse->setPen(penEllipse);
        ellipse->setBrush(penBrush);

        if(iterator->getNeighbor2() != 0) {
            Edge* currentEdge = iterator->getEdge2();
            QGraphicsLineItem* lineItem = currentEdge->getLineItem();
            QPen linePen = oldPenLine[i];
            lineItem->setPen(linePen);

        }

        iterator = iterator->getNeighbor2();
        i++;
    }
}

void SimplificationProfileScene::loadProfile() {
    Profile* profile = meshManager->getCurrentProfile();
    Vertex* iterator = profile->getProfileVertex();
    Edge* currentEdge;

    // load every vertices and edges in this scene
    while(iterator != 0) {
        QGraphicsEllipseItem* ellipse = iterator->getEllipse();
        oldPenEllipse.push_back(ellipse->pen());
        oldBrushEllipse.push_back(ellipse->brush());

        QPen vertexPen;
        QBrush vertexBrush;
        vertexPen.setWidth(3);
        vertexPen.setColor(Qt::white);
        vertexBrush.setColor(Qt::black);
        vertexBrush.setStyle(Qt::SolidPattern);

        ellipse->setPen(vertexPen);
        ellipse->setBrush(vertexBrush);

        // add the ellipse in this scene
        this->addItem(ellipse);


        if(iterator->getNeighbor2() != 0) {
            currentEdge = iterator->getEdge2();
            QGraphicsLineItem* lineItem = currentEdge->getLineItem();

            oldPenLine.push_back(lineItem->pen());

            QPen linePen;
            linePen.setWidth(3);
            linePen.setColor(Qt::black);
            lineItem->setPen(linePen);

            // add the line item in this scene
            this->addItem(lineItem);
        }

        iterator->setValid(true);
        iterator = iterator->getNeighbor2();
    }
}

void SimplificationProfileScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) {
    if (mouseEvent->button() ==  Qt::RightButton){
        Profile* profile = meshManager->getCurrentProfile();
        Vertex* iterator = profile->getProfileVertex();

        // find on which vertex we have clicked
        while(iterator != 0){
            QGraphicsEllipseItem* ellipse = iterator->getEllipse();
            if (ellipse->isUnderMouse() && iterator->isValid()){
                if(currentCurve->begin == 0) {
                    // we start a new curve
                    currentCurve->begin = iterator;
                    // we add its color identification
                    currentCurve->color.setRgb(std::rand()%256, std::rand()%256, std::rand()%256);

                    QPen pen = ellipse->pen();
                    pen.setColor(currentCurve->color);
                    ellipse->setPen(pen);

                    iterator->invalid();
                } else {
                    // we finish the current curve

                    //check validity of the curve
                    Vertex* iterator2 = currentCurve->begin->getNeighbor2();
                    bool curveValid = true;
                    // check if all vertices between the begining and the end are valid (not already
                    // in another curve)
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
        }
    }
}

void SimplificationProfileScene::updateSceneEllipseAndLineColor() {
    Vertex* begin = currentCurve->begin;
    Vertex* end = currentCurve->end;

    Edge* edgeToChangeColor;
    QPen linePen = begin->getEdge2()->getLineItem()->pen();
    linePen.setColor(currentCurve->color);

    QPen ellipsePen = begin->getEllipse()->pen();
    ellipsePen.setColor(currentCurve->color);

    currentCurve->size = 1;

    // update the curve color
    while(begin != end) {
        //update edge color
        edgeToChangeColor = begin->getEdge2();
        edgeToChangeColor->getLineItem()->setPen(linePen);
        begin = begin->getNeighbor2();

        // we will not be able to click on this vertex
        begin->invalid();
        currentCurve->size++;

        //update vertex color
        begin->getEllipse()->setPen(ellipsePen);
    }
    end->invalid();
}
