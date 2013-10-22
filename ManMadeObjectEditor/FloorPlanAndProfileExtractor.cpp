#include "FloorPlanAndProfileExtractor.h"

FloorPlanAndProfileExtractor::FloorPlanAndProfileExtractor()
    :intervalBetweenZValue(0.1f), totalNumPlan(11), smallestZValue(-0.5f), floorPlanIndex(0)
{
    for(int i(0); i < totalNumPlan; ++i){
        std::vector<OMMesh::VertexIter>* planTmp = new std::vector<OMMesh::VertexIter>();
        plan.push_back(planTmp);
    }
}

void FloorPlanAndProfileExtractor::extractAllPlan(OMMesh* inputMesh)
{
    OMMesh::VertexIter v_it, v_end(inputMesh->vertices_end());

    OMMesh::VertexVertexIter vv_it;

    // iterate over the vertices to construct a floor plan using the TmpFloorVertex struct
    // and store for each plan all vertices (iterator) associated with the plan
    // it will be usefull to construct the profiles
    for (v_it=inputMesh->vertices_begin(); v_it!=v_end; ++v_it) {
        OMMesh::Point point = inputMesh->point(v_it);
        float planNumberTmp = (point[2] - smallestZValue) / intervalBetweenZValue;
        int planNumber = floor(planNumberTmp + 0.5f);

        // check if planNumber is really a correct plan number
        // 0.001f because of precision problem
        if ((abs(planNumberTmp - planNumber) < 0.001f) && (planNumber >= floorPlanIndex)) {

            // store the vertices with the associated plan
            std::vector<OMMesh::VertexIter>* planTmp = plan[planNumber];
            planTmp->push_back(v_it);
            point = inputMesh->point(v_it);

            // check is the current vertex is on the floor plan
            if (abs(planNumber - floorPlanIndex) < 0.001f) {
                TmpFloorVertex currentVertex;
                currentVertex.point = point;
                currentVertex.neighbor1Used = false;
                currentVertex.neighbor2Used = false;
                currentVertex.v_it = v_it;

                // find the two neighbor of this floor plan vertex
                for (vv_it=inputMesh->vv_iter(v_it); vv_it; ++vv_it) {
                    OMMesh::Point neighborTmp = inputMesh->point(vv_it);
                    planNumberTmp = (neighborTmp[2] - smallestZValue) / intervalBetweenZValue;
                    planNumber = floor(planNumberTmp + 0.5);
                    if (abs(planNumberTmp - planNumber) < 0.001f ) {
                        if (abs(planNumber - floorPlanIndex) < 0.001f) {
                            if (currentVertex.neighbor1Used == false) {
                                currentVertex.neighbor1 = neighborTmp;
                                currentVertex.neighbor1Used = true;
                            } else {
                                currentVertex.neighbor2 = neighborTmp;
                                currentVertex.neighbor2Used = true;
                            }
                        }
                    }

                }
                // store the current floor plan vertex with his neighbors information,
                // used later to construct our floor plan on which we will work
                tmpFloorPlanVector.push_back(currentVertex);
            }
        }
    }
}

void FloorPlanAndProfileExtractor::floorPlanConstruction(FloorVertex*& floorPlan, unsigned int& floorPlanSize
                                                         ,OMMesh* inputMesh)
{
    // with the previous computed information, we will construct the floor plan
    // on which we will work
    // The idea is to pick a vertex, then find one neighbor and thus defining a neighboring
    // relationship between them and iterate to construct our floor plan like this
    unsigned int size = tmpFloorPlanVector.size();

    // take a vertex
    TmpFloorVertex currentVertex = tmpFloorPlanVector[0];
    OMMesh::Point currentPoint = currentVertex.point;;
    OMMesh::Point currentVertexNeighbor1 = currentVertex.neighbor1;
    OMMesh::Point currentVertexNeighbor2 = currentVertex.neighbor2;

    FloorVertex* firstPoint;
    FloorVertex* previousFloorVertexNeighbor;
    TmpFloorVertex possibleNeighbor;
    bool found(false);

    // find the neighbors of the vertex we have just taken
    for(unsigned int j(0); j < size && !found; ++j) {
        possibleNeighbor = tmpFloorPlanVector[j];
        OMMesh::Point possibleNeighborPoint = possibleNeighbor.point;

        if ((possibleNeighborPoint == currentVertexNeighbor1) || (possibleNeighborPoint == currentVertexNeighbor2)) {
            FloorVertex* currentFloorVertexPoint = new FloorVertex(currentPoint[0], currentPoint[1]);
            floorPlanSize++;

            FloorVertex* currentFloorVertexNeighbor = new FloorVertex(possibleNeighborPoint[0], possibleNeighborPoint[1]);

            currentFloorVertexPoint->setNeighbor2(currentFloorVertexNeighbor);//addNeighbor
            currentFloorVertexNeighbor->setNeighbor1(currentFloorVertexPoint);//addNeighbor

            if (possibleNeighbor.neighbor1 == currentPoint) {
                possibleNeighbor.neighbor1Used = false;
            } else {
                possibleNeighbor.neighbor2Used = false;
            }

            OMMesh::VertexIter v_it = currentVertex.v_it;
            OMMesh::Point point = inputMesh->normal(v_it);
            currentFloorVertexPoint->setNormal(point[0], point[1]);

            previousFloorVertexNeighbor = currentFloorVertexNeighbor;
            firstPoint = currentFloorVertexPoint;
            found = true;
        }
    }

    // our floor plan is a pointer on a vertex of the floor plan
    floorPlan = firstPoint;

    // we will then apply the same logic with the neighbor. It becomes our current vertex
    // and we will find his neighbors and iterate to construct our floor plan
    found = false;
    for (unsigned int i(1); i < size - 1; ++i) {
        currentVertex = possibleNeighbor;
        currentPoint = currentVertex.point;
        currentVertexNeighbor1 = currentVertex.neighbor1;
        currentVertexNeighbor2 = currentVertex.neighbor2;

        found = false;

        for(unsigned int j(0); (j < size) && !found; ++j) {
            possibleNeighbor = tmpFloorPlanVector[j];
            OMMesh::Point possibleNeighborPoint = possibleNeighbor.point;

            if (((currentVertex.neighbor2Used == false) && (currentVertex.neighbor1Used != false) && (possibleNeighborPoint == currentVertexNeighbor1))
                || ((currentVertex.neighbor1Used == false) && (currentVertex.neighbor2Used != false) && (currentVertexNeighbor2 == possibleNeighborPoint)) ) {

                FloorVertex* currentFloorVertexNeighbor = new FloorVertex(possibleNeighborPoint[0], possibleNeighborPoint[1]);
                floorPlanSize++;

                previousFloorVertexNeighbor->setNeighbor2(currentFloorVertexNeighbor);//addNeighbor
                currentFloorVertexNeighbor->setNeighbor1(previousFloorVertexNeighbor);//addNeighbor

                if (possibleNeighbor.neighbor1 == currentPoint) {
                    possibleNeighbor.neighbor1Used = false;
                } else {
                    possibleNeighbor.neighbor2Used = false;
                }

                OMMesh::VertexIter v_it = currentVertex.v_it;
                OMMesh::Point point = inputMesh->normal(v_it);
                previousFloorVertexNeighbor->setNormal(point[0], point[1]);

                previousFloorVertexNeighbor = currentFloorVertexNeighbor;
                found = true;
            }
        }
    }

    OMMesh::VertexIter v_it = possibleNeighbor.v_it;
    OMMesh::Point point = inputMesh->normal(v_it);
    previousFloorVertexNeighbor->setNormal(point[0], point[1]);

    // define the neighboring relationship between the last and first vertex
    floorPlanSize++;
    previousFloorVertexNeighbor->setNeighbor2(firstPoint);//addNeighbor
    firstPoint->setNeighbor1(previousFloorVertexNeighbor);//addNeighbor
}

void FloorPlanAndProfileExtractor::profileConstruction(OMMesh* inputMesh, FloorVertex*& floorPlan,
                                                       Profile*& currentProfile, unsigned int& floorPlanSize)
{
    // now create profiles

    // we have the floorplan, so now we must compute the profiles

    float centerFloorPlanX(0.0f);
    float centerFloorPlanY(0.0f);

    // center computation of the floor plan
    Vertex* vertexIterator = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i){
        centerFloorPlanX += vertexIterator->getX();
        centerFloorPlanY += vertexIterator->getY();
        vertexIterator = vertexIterator->getNeighbor2();
    }

    centerFloorPlanX = centerFloorPlanX / floorPlanSize;
    centerFloorPlanY = centerFloorPlanY / floorPlanSize;

    int totalNumberPlan = plan.size();
    int numberFloorPlanVertex = floorPlanSize;
    Profile* p;

    // will compute a profile for each vertex in the floor plan
    FloorVertex* floorVertexIterator = floorPlan;
    for(int i(0); i < numberFloorPlanVertex; ++i){

        p = new Profile(true);
        ProfileDestructorManager::putProfile(p);

        p->addProfileVertex(Utils::distance(floorVertexIterator->getX(), floorVertexIterator->getY(),
                                            floorVertexIterator->getX(), floorVertexIterator->getY()),
                            floorPlanIndex * intervalBetweenZValue);

        for (int j = floorPlanIndex + 1; j < totalNumberPlan; ++j ) {
            std::vector<OMMesh::VertexIter>* planJ = plan[j];
            int numberVertices = planJ->size();


            // for each vertex of the current plan, we want to find the one that has the most similar
            // angle with the current floor plan vertex (because if it is the same angle, it is on the same profile)
            // To do it,we have to compute the center of the plan in which the current possible profile point
            // belongs to.

            // center computation of the current plan
            float centerCurrentPlanX(0.0f);
            float centerCurrentPlanY(0.0f);
            OMMesh::VertexIter v_it_tmp;
            OMMesh::Point point_tmp;
            unsigned int sizeCurrentPlan = planJ->size();
            for(unsigned int m(0); m < sizeCurrentPlan; ++m){
                v_it_tmp = (*planJ)[m];
                point_tmp = inputMesh->point(v_it_tmp);
                centerCurrentPlanX += point_tmp[0];
                centerCurrentPlanY += point_tmp[1];
            }
            centerCurrentPlanX = centerCurrentPlanX / sizeCurrentPlan;
            centerCurrentPlanY = centerCurrentPlanY / sizeCurrentPlan;


            for (int k(0); k < numberVertices; ++k) {
                OMMesh::VertexIter v_it = (*planJ)[k];
                OMMesh::Point point = inputMesh->point(v_it);

                // get the angle defining the 2D position on the current plan

                if (isSameAngle(point[0], point[1], centerFloorPlanX, centerFloorPlanY,
                                floorVertexIterator->getX(), floorVertexIterator->getY(),
                                centerCurrentPlanX, centerCurrentPlanY)) {
                    p->addProfileVertex(Utils::distance(point[0], point[1], floorVertexIterator->getX(),
                            floorVertexIterator->getY()), point[2] - smallestZValue);
                    break;
                }
            }
        }
        floorVertexIterator->setProfile(p);
        floorVertexIterator= (FloorVertex*)floorVertexIterator->getNeighbor2();
    }
    currentProfile = p;
}

void FloorPlanAndProfileExtractor::extract(OMMesh* inputMesh, FloorVertex*& floorPlan,
                                           Profile*& currentProfile, unsigned int& floorPlanSize)
{
    extractAllPlan(inputMesh);
    floorPlanConstruction(floorPlan, floorPlanSize, inputMesh);
    profileConstruction(inputMesh, floorPlan, currentProfile, floorPlanSize);

    // we do not need all these plans anymore
    for(int i(0); i < totalNumPlan; ++i){
        std::vector<OMMesh::VertexIter>* planTmp = plan[i];
        delete planTmp;
    }
}

bool FloorPlanAndProfileExtractor::isSameAngle(float toX1, float toY1, float fromX1, float fromY1,
                                               float toX2, float toY2, float fromX2, float fromY2)
{
    if ((abs(toX1 - fromX1) < 0.001f  && abs(toY1 - fromY1) < 0.001f) || (abs(toX2 - fromX2) < 0.001f  && abs(toY2 - fromY2) < 0.001f)) {
        return true;
    }

    float v1x = toX1 - fromX1;
    float v1y = toY1 - fromY1;

    float v2x = toX2 - fromX2;
    float v2y = toY2 - fromY2;

    Utils::normalize(v1x, v1y);
    Utils::normalize(v2x, v2y);

    float dotProduct = v1x * v2x + v1y * v2y;

    return abs(1.0f - dotProduct) < 0.00001f;
}
