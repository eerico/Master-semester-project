#include "FloorPlanAndProfileExtractor.h"

FloorPlanAndProfileExtractor::FloorPlanAndProfileExtractor()
    :dz(0.1f), totalNumPlan(11), baseZ(-0.5f), floorPlanIndex(0)
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
        float planNumTmp = (point[2] - baseZ) / dz;
        int planNum = floor(planNumTmp + 0.5f);

        // check if planNum is really a correct plan number
        // 0.001f because of precision problem
        if ((abs(planNumTmp - planNum) < 0.001f) && (planNum >= floorPlanIndex)) {

            // store the vertices with the associated plan
            std::vector<OMMesh::VertexIter>* planTmp = plan[planNum];
            planTmp->push_back(v_it);
            point = inputMesh->point(v_it);

            // check is the current vertex is on the floor plan
            if (abs(planNum - floorPlanIndex) < 0.001f) {
                TmpFloorVertex currentVertex;
                currentVertex.point = point;
                currentVertex.neighbor1Used = false;
                currentVertex.neighbor2Used = false;
                currentVertex.v_it = v_it;

                // find the two neighbor of this floor plan vertex
                for (vv_it=inputMesh->vv_iter(v_it); vv_it; ++vv_it) {
                    OMMesh::Point neighborTmp = inputMesh->point(vv_it);
                    planNumTmp = (neighborTmp[2] - baseZ) / dz;
                    planNum = floor(planNumTmp + 0.5);
                    if (abs(planNumTmp - planNum) < 0.001f ) {
                        if (abs(planNum - floorPlanIndex) < 0.001f) {
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

    float c1x(0.0f);
    float c1y(0.0f);

    // center computation of the floor plan
    Vertex* vMC1 = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i){
        c1x += vMC1->getX();
        c1y += vMC1->getY();
        vMC1 = vMC1->getNeighbor2();
    }

    c1x = c1x / floorPlanSize;
    c1y = c1y / floorPlanSize;

    int totalNumberPlan = plan.size();
    int numberFloorPlanVertex = floorPlanSize;
    Profile* p;

    // will compute a profile for each vertex in the floor plan
    FloorVertex* v = floorPlan;
    for(int i(0); i < numberFloorPlanVertex; ++i){

        p = new Profile(true);
        ProfileDestructorManager::putProfile(p);

        p->addProfileVertex(distance(v->getX(), v->getY(), v->getX(), v->getY()), floorPlanIndex * dz);

        for (int j = floorPlanIndex + 1; j < totalNumberPlan; ++j ) {
            std::vector<OMMesh::VertexIter>* planJ = plan[j];
            int numVertices = planJ->size();


            // for each vertex of the current plan, we want to find the one that has the most similar
            // angle with the current floor plan vertex (because if it is the same angle, it is on the same profile)
            // To do it,we have to compute the center of the plan in which the current possible profile point
            // belongs to.
            // center computation of the current plan
            float c2x(0.0f);
            float c2y(0.0f);
            OMMesh::VertexIter v_it_tmp;
            OMMesh::Point point_tmp;
            unsigned int sizeTmp = planJ->size();
            for(unsigned int m(0); m < sizeTmp; ++m){
                v_it_tmp = (*planJ)[m];
                point_tmp = inputMesh->point(v_it_tmp);
                c2x += point_tmp[0];
                c2y += point_tmp[1];
            }
            c2x = c2x / sizeTmp;
            c2y = c2y / sizeTmp;


            for (int k(0); k < numVertices; ++k) {
                OMMesh::VertexIter v_it = (*planJ)[k];
                OMMesh::Point point = inputMesh->point(v_it);

                // get the angle defining the 2D position on the current plan

                if (isSameAngle(point[0], point[1], c1x, c1y, v->getX(), v->getY(), c2x, c2y)) {
                    p->addProfileVertex(distance(point[0], point[1], v->getX(), v->getY()), point[2] - baseZ);
                    break;
                }
            }
        }
        v->setProfile(p);
        v = (FloorVertex*)v->getNeighbor2();
    }
    currentProfile = p;
}

void FloorPlanAndProfileExtractor::extract(OMMesh* inputMesh, FloorVertex*& floorPlan,
                                           Profile*& currentProfile, unsigned int& floorPlanSize)
{
    extractAllPlan(inputMesh);
    floorPlanConstruction(floorPlan, floorPlanSize, inputMesh);
    profileConstruction(inputMesh, floorPlan, currentProfile, floorPlanSize);

    for(int i(0); i < totalNumPlan; ++i){
        std::vector<OMMesh::VertexIter>* planTmp = plan[i];
        delete planTmp;
    }
}


float FloorPlanAndProfileExtractor::distance(float x1, float y1, float x2, float y2)
{
    float diffX = x1 - x2;
    float diffY = y1 - y2;
    return std::sqrt(diffX * diffX + diffY * diffY);
}

bool FloorPlanAndProfileExtractor::isSameAngle(float x1, float y1, float cx1, float cy1,
                                               float x2, float y2, float cx2, float cy2)
{
    if ((abs(x1 - cx1) < 0.001f  && abs(y1 - cy1) < 0.001f) || (abs(x2 - cx2) < 0.001f  && abs(y2 - cy2) < 0.001f)) {
        return true;
    }

    float norm1 = distance(x1, y1, cx1, cy1);
    float norm2 = distance(x2, y2, cx2, cy2);

    float v1x = x1 - cx1;
    float v1y = y1 - cy1;

    v1x = v1x / norm1;
    v1y= v1y / norm1;

    float v2x = x2 - cx2;
    float v2y = y2 - cy2;

    v2x = v2x / norm2;
    v2y = v2y / norm2;

    float dotProduct = v1x * v2x + v1y * v2y;

    return abs(1.0f - dotProduct) < 0.00001f;
}
