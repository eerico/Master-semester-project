#include "SimplificationFloorPlanScene.h"

SimplificationFloorPlanScene::SimplificationFloorPlanScene(MeshManager *meshManager, std::vector< Curve* >* curveArray)
    : SimplificationScene(meshManager, curveArray)
{
    this->setSceneRect(QRectF(0, 0, 4000, 6000));
    loadFloorPlan();

    // the first curve
    currentCurve = new Curve;
}

SimplificationFloorPlanScene::~SimplificationFloorPlanScene() {
    // delete all created data
    foreach(Curve* curve, *curveArray) {
        delete curve;
    }
    delete currentCurve;

    unsigned int size = meshManager->getFloorPlanSize();
    Vertex* iterator = meshManager->getFloorPlan();
    for(unsigned int i(0); i < size; ++i) {
        iterator->setValid(true);
        iterator = iterator->getNeighbor2();
    }
}

void SimplificationFloorPlanScene::revertColor() {
    unsigned int size = meshManager->getFloorPlanSize();
    Vertex* iterator = meshManager->getFloorPlan();
    Edge* currentEdge = iterator->getEdge2();

    // revert every ellipse and line item pen and brush
    for(unsigned int i(0); i < size; ++i) {
        QGraphicsEllipseItem* ellipse = iterator->getEllipse();
        QGraphicsLineItem* lineItem = currentEdge->getLineItem();

        QPen penEllipse = oldPenEllipse[i];
        QBrush penBrush = oldBrushEllipse[i];

        QPen linePen = oldPenLine[i];

        ellipse->setPen(penEllipse);
        ellipse->setBrush(penBrush);

        lineItem->setPen(linePen);

        iterator = iterator->getNeighbor2();
        currentEdge = iterator->getEdge2();
    }
}

void SimplificationFloorPlanScene::loadFloorPlan() {
    unsigned int size = meshManager->getFloorPlanSize();
    Vertex* iterator = meshManager->getFloorPlan();
    Edge* currentEdge = iterator->getEdge2();

    // load every vertices and edges in this scene
    for(unsigned int i(0); i < size; ++i) {
        QGraphicsEllipseItem* ellipse = iterator->getEllipse();
        QGraphicsLineItem* lineItem = currentEdge->getLineItem();

        oldPenEllipse.push_back(ellipse->pen());
        oldBrushEllipse.push_back(ellipse->brush());
        oldPenLine.push_back(lineItem->pen());

        QPen vertexPen;
        QBrush vertexBrush;
        vertexPen.setWidth(3);
        vertexPen.setColor(Qt::white);
        vertexBrush.setColor(Qt::black);
        vertexBrush.setStyle(Qt::SolidPattern);

        ellipse->setPen(vertexPen);
        ellipse->setBrush(vertexBrush);

        QPen linePen;
        linePen.setWidth(3);
        linePen.setColor(Qt::black);
        lineItem->setPen(linePen);

        // add the ellipse and the line item in this scene
        this->addItem(ellipse);
        this->addItem(lineItem);

        iterator->setValid(true);

        iterator = iterator->getNeighbor2();
        currentEdge = iterator->getEdge2();
    }
}

void SimplificationFloorPlanScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) {
    if (mouseEvent->button() ==  Qt::RightButton){
        Vertex* iterator = meshManager->getFloorPlan();
        unsigned int floorPlanSize = meshManager->getFloorPlanSize();

        // find on which vertex we have clicked
        for (unsigned int i(0); i < floorPlanSize; ++i) {
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
                    while(iterator2 != iterator) {
                        if(!iterator2->isValid()) {
                            curveValid = false;
                        }
                        iterator2 = iterator2->getNeighbor2();
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

void SimplificationFloorPlanScene::updateSceneEllipseAndLineColor() {
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

