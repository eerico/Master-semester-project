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

void FloorPlanAndProfileExtractor::floorPlanConstruction(FloorVertex*& floorPlan, unsigned int& floorPlanSize)
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

                previousFloorVertexNeighbor = currentFloorVertexNeighbor;
                found = true;
            }
        }
    }

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

    // mass center -> profile oriented toward the mass center
    float cx(0.0f);
    float cy(0.0f);

    Vertex* vMC = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i){
        cx += vMC->getX();
        cy += vMC->getY();
        vMC = vMC->getNeighbor2();
    }

    cx = cx / floorPlanSize;
    cy = cy / floorPlanSize;

    int totalNumberPlan = plan.size();
    int numberFloorPlanVertex = floorPlanSize;
    Profile* p;

    // will compute a profile for each vertex in the floor plan
    FloorVertex* v = floorPlan;
    for(int i(0); i < numberFloorPlanVertex; ++i){

        float phi1;
        float r1;

        // get the angle defining the 2D position on the floor vertex
        inversePolar(v->getX(), v->getY(), r1, phi1);
        p = new Profile(true);
        p->addProfileVertex(distance(v->getX(), v->getY(), cx, cy), floorPlanIndex * dz);

        for (int j = floorPlanIndex + 1; j < totalNumberPlan; ++j ) {
            std::vector<OMMesh::VertexIter>* planJ = plan[j];
            int numVertices = planJ->size();

            // for each vertex of the current plan, we want to find the one that has the most similar
            // angle with the current floor plan vertex (because if it is the same angle, it is on the same profile)
            for (int k(0); k < numVertices; ++k) {
                OMMesh::VertexIter v_it = (*planJ)[k];
                OMMesh::Point point = inputMesh->point(v_it);

                float r2;
                float phi2;
                // get the angle defining the 2D position on the current plan
                inversePolar(point[0], point[1], r2, phi2);

                if (abs(phi2 - phi1) < 0.001f || phi2 == 0.0f) {// je ne sait pas si tu veut mettre +- 0.001 comme d'abitude
                   p->addProfileVertex(distance(point[0], point[1], cx, cy), point[2] - baseZ);
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
    floorPlanConstruction(floorPlan, floorPlanSize);
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
    // -1.0f car: quand on calcul avec le profil pour generer 3D, en 0 s est droit et 1 s est pencher (fermer)
    // donc si distance 1.0f alors le cone est ouvert donc -1.0f
    // SA revient a dire que w est la distance entre le bord et le point x, y
    return 1.0f - sqrt(diffX * diffX + diffY * diffY);
}

void FloorPlanAndProfileExtractor::inversePolar(float x, float y, float& r, float& phi)
{
    //http://en.wikipedia.org/wiki/Polar_coordinate_system
    r = sqrt(x*x + y*y);
    phi = atan2f(y, x);

}
