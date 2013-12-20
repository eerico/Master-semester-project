#include "ActivePlan.h"


ActivePlan::ActivePlan(Vertex *floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *> *triangles, bool parallelTest)
    : triangles(triangles)
{
    Vertex* currentVertex = floorPlan;
    activePlan = new std::vector< Edge* >;

    Vertex* firstCloneVertex;
    Vertex* previousVertex;
    Edge* previousEdge;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = currentVertex->getEdge2();

        Edge* cloneEdge = new Edge(currentEdge->getVertex1(), currentEdge->getVertex2(), currentEdge->getProfile());
        Vertex* cloneVertex1 = new Vertex(currentVertex->getX(), currentVertex->getY(), currentVertex->getZ());

        cloneVertex1->setEdge2(cloneEdge);
        cloneEdge->setVertex1(cloneVertex1);
        Plan* plan = currentEdge->getDirectionPlan();
        if(plan != 0) {
            plan->setVertex(cloneVertex1);
        }
        cloneEdge->setDirectionPlan(plan);

        if(i > 0) {
            cloneVertex1->setEdge1(previousEdge);
            cloneVertex1->setNeighbor1(previousVertex);
            previousVertex->setNeighbor2(cloneVertex1);
            previousEdge->setVertex2(cloneVertex1);
        } else {
            firstCloneVertex = cloneVertex1;
        }

        previousVertex = cloneVertex1;
        previousEdge = cloneEdge;

        activePlan->push_back(cloneEdge);
        currentVertex = currentVertex->getNeighbor2();

        GeneralDestructorManager::putObject(cloneEdge);
        GeneralDestructorManager::putObject(cloneVertex1);
    }

    firstCloneVertex->setEdge1(previousEdge);
    firstCloneVertex->setNeighbor1(previousVertex);
    previousVertex->setNeighbor2(firstCloneVertex);
    previousEdge->setVertex2(firstCloneVertex);


    // check if two edge are parallel
    // if there are, we remove the connection between them

    if(parallelTest) {
        for(unsigned int i(0); i < floorPlanSize ; ++i) {
            Edge* currentEdge = (*activePlan)[i];
            Edge* nextEdge = (*activePlan)[(i+1) % floorPlanSize];
            if (currentEdge->isParallel(nextEdge)) {
                //Vertex* vertexToRemove = currentEdge->getVertex2();
                Vertex* vertex1 = currentEdge->getVertex1();
                Vertex* vertex2 = nextEdge->getVertex2();
                currentEdge->getVertex1()->setZ(0);
                vertex1->setNeighbor2(vertex2);
                vertex2->setNeighbor1(vertex1);

                nextEdge->setVertex1(vertex1);
                vertex1->setEdge2(nextEdge);

                activePlan->erase(activePlan->begin() + i);
                floorPlanSize--;
                i--;
            }

        }
    }
}

ActivePlan::ActivePlan(ActivePlan* previousActivePlan, std::vector<qglviewer::Vec *> *triangles, bool& chainSizeOne)
    :triangles(triangles)
{
    chainSizeOne = false;
    activePlan = new std::vector< Edge* >;

    previousActivePlan->removeInvalidEdges();

    // The previous active plan can contain several loop, it is no more only one loop.
    // To simplify the cloning process, first construct every chain
    std::vector< std::vector< Edge* >* >* chains = createChain(previousActivePlan);

    if(chains->size() == 1) {
        chainSizeOne = true;
        return;
    }

    // Then for each chain, we can construct the associated active plan
    // and merge them
    foreach(std::vector< Edge* >* currentChain, *chains) {
        unsigned int size = currentChain->size();
        Edge* edgeTmp = (*currentChain)[0];

        ActivePlan activePlanTmp(edgeTmp->getVertex1(), size, triangles, false);
        std::vector< Edge* >* plansTmp = activePlanTmp.getPlan();
        activePlan->insert(activePlan->end(), plansTmp->begin(), plansTmp->end());

        delete currentChain;
    }

    delete chains;
}

ActivePlan::~ActivePlan() {
    delete activePlan;
}

void ActivePlan::removeInvalidEdges() {
    unsigned int size = activePlan->size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = (*activePlan)[i];
        if(!currentEdge->isValid()) {
            activePlan->erase(activePlan->begin() + i);
            i--;
            size--;
        }
    }
}

std::vector< std::vector< Edge* >* >* ActivePlan::createChain(ActivePlan* previousActivePlan) {
    std::vector< std::vector< Edge* >* >* chains = new std::vector< std::vector< Edge* >* >;

    std::vector< Edge* >* edges = previousActivePlan->getPlan();
    unsigned int size = edges->size();

    //create the chains
    Vertex* first = 0;
    Vertex* last = 0;

    for(unsigned int i(0); i < size; ++i) {
        std::vector< Edge* >* currentChain = new std::vector< Edge* >;
        Edge* currentEdge = (*edges)[i];

        first = currentEdge->getVertex1();
        last = currentEdge->getVertex2();
        currentChain->push_back(currentEdge);

        for(unsigned int j(i+1); j < size; ++j) {
            Edge* comparedEdge = (*edges)[j];

            Vertex* vertex1 = comparedEdge->getVertex1();
            Vertex* vertex2 = comparedEdge->getVertex2();

            if(vertex1 == first) {
                currentChain->insert(currentChain->begin(), comparedEdge);
                edges->erase(edges->begin() + j);
                size--;
                j = i;
                first = vertex2;
            } else if(vertex2 == first) {
                currentChain->insert(currentChain->begin(), comparedEdge);
                edges->erase(edges->begin() + j);
                size--;
                j = i;
                first = vertex1;
            } else if(vertex1 == last) {
                currentChain->push_back(comparedEdge);
                edges->erase(edges->begin() + j);
                size--;
                j = i;
                last = vertex2;
            } else if(vertex2 == last) {
                currentChain->push_back(comparedEdge);
                edges->erase(edges->begin() + j);
                size--;
                j = i;
                last = vertex1;
            }
        }

        chains->push_back(currentChain);
    }

    return chains;
}

void ActivePlan::computeDirectionPlan() {
    foreach(Edge* edge, *activePlan) {
        Vertex* vertex1 = edge->getVertex1();
        Plan* plan = new Plan( vertex1, edge->getVertex2(), edge->getProfile());
        plan->computePlanNormal();
        edge->setDirectionPlan(plan);
        GeneralDestructorManager::putObject(plan);
    }
}

std::vector< Edge* >* ActivePlan::getPlan() {
    return activePlan;
}

// return false si l intersection n est pas valid
bool ActivePlan::filteringInvalidEvent(Intersection &intersection) {
    std::vector< Edge* >* edges = intersection.edgeVector;
    Edge* edge = (*edges)[0];
    Edge* neighbor1 = (*edges)[1];
    Edge* neighbor2 = (*edges)[2];

    if(!edge->isValid() || !neighbor1->isValid() || !neighbor2->isValid()) {
        return false;
    }
    if(neighbor1->getVertex2()->distance(edge->getVertex1()) < 0.00001f
            || neighbor1->getVertex2()->distance(edge->getVertex2()) < 0.00001f) {
        return false;
    }

    Plan horizontalPlan(0.0f, 0.0f, intersection.z, 0.0f, 0.0f, 1.0f);

    // v0 <- neighbor1 -> v1 <- neighbor2 -> v2
    Plan* plan1 = neighbor1->getDirectionPlan();
    Plan* plan2 = neighbor2->getDirectionPlan();

    // compute the intersection between the two neighbor and get the point at the corresponding height
    Intersection intersectionBetweenNeighbor = horizontalPlan.intersect3Plans(plan1, plan2);
    if(intersectionBetweenNeighbor.eventType == NoIntersection) {
        Vertex* v = neighbor1->getVertex2();
        intersectionBetweenNeighbor.x = v->getX();
        intersectionBetweenNeighbor.y = v->getY();
        intersectionBetweenNeighbor.z = v->getZ();
    }

    Vertex* vertexAtCurrentHeight = new Vertex(intersectionBetweenNeighbor.x, intersectionBetweenNeighbor.y, intersectionBetweenNeighbor.z);

    //now we compute the edge at the height and find if the distance is small
    //enough to consider we have an intersection
    Edge* previousEdge = edge->getVertex1()->getEdge1();
    Edge* nextEdge = edge->getVertex2()->getEdge2();

    //compute the first vertex of the edge
    plan1 = previousEdge->getDirectionPlan();
    plan2 = edge->getDirectionPlan();
    Intersection intersectionPreviousEdge = horizontalPlan.intersect3Plans(plan1, plan2);
    if(intersectionPreviousEdge.eventType == NoIntersection) {
        Vertex* v = edge->getVertex1();
        intersectionPreviousEdge.x = v->getX();
        intersectionPreviousEdge.y = v->getY();
        intersectionPreviousEdge.z = v->getZ();
    }

    plan1 = edge->getDirectionPlan();
    plan2 = nextEdge->getDirectionPlan();
    Intersection intersectionNextEdge = horizontalPlan.intersect3Plans(plan1, plan2);
    if(intersectionNextEdge.eventType == NoIntersection) {
        Vertex* v = edge->getVertex2();
        intersectionNextEdge.x = v->getX();
        intersectionNextEdge.y = v->getY();
        intersectionNextEdge.z = v->getZ();
    }

    Edge* newEdgeAtIntersectionHeight = new Edge(new Vertex(intersectionPreviousEdge.x, intersectionPreviousEdge.y, intersectionPreviousEdge.z)
                                                 , new Vertex(intersectionNextEdge.x, intersectionNextEdge.y, intersectionNextEdge.z));

    if(newEdgeAtIntersectionHeight->distanceXY(vertexAtCurrentHeight) < /*0.00001f*/ 0.0005f) {
        delete vertexAtCurrentHeight;
        delete newEdgeAtIntersectionHeight;

        return true;
    } else {
        delete vertexAtCurrentHeight;
        delete newEdgeAtIntersectionHeight;

        return false;
    }
}

void ActivePlan::filteringInvalidEvent2(Intersection &intersection) {
    // sa test pour chaque edge, yen a pas que trois ici
    std::vector< Edge* >* edges = intersection.edgeVector;

    Plan horizontalPlan(0.0f, 0.0f, intersection.z, 0.0f, 0.0f, 1.0f);
    Vertex* vertexAtCurrentHeight = new Vertex(intersection.x, intersection.y, intersection.z);
    std::vector< Edge* >* filtredEdges = new std::vector< Edge* >;
    foreach(Edge* edge, *edges) {
        if(edge->isValid()) {
            //we compute the edge at the height and find if the distance is small
            //enough to consider we have an intersection
            Edge* previousEdge = edge->getVertex1()->getEdge1();
            Edge* nextEdge = edge->getVertex2()->getEdge2();

            //compute the first vertex of the edge
            Plan* plan1 = previousEdge->getDirectionPlan();
            Plan* plan2 = edge->getDirectionPlan();
            Intersection intersectionPreviousEdge = horizontalPlan.intersect3Plans(plan1, plan2);
            if(intersectionPreviousEdge.eventType == NoIntersection) {
                Vertex* v1 = edge->getVertex1();
                intersectionPreviousEdge.x = v1->getX();
                intersectionPreviousEdge.y = v1->getY();
                intersectionPreviousEdge.z = v1->getZ();
            }

            plan1 = edge->getDirectionPlan();
            plan2 = nextEdge->getDirectionPlan();
            Intersection intersectionNextEdge = horizontalPlan.intersect3Plans(plan1, plan2);
            if(intersectionNextEdge.eventType == NoIntersection) {
                Vertex* v2 = edge->getVertex2();
                intersectionNextEdge.x = v2->getX();
                intersectionNextEdge.y = v2->getY();
                intersectionNextEdge.z = v2->getZ();
            }

            Edge* newEdgeAtIntersectionHeight = new Edge(new Vertex(intersectionPreviousEdge.x, intersectionPreviousEdge.y, intersectionPreviousEdge.z)
                                                         , new Vertex(intersectionNextEdge.x, intersectionNextEdge.y, intersectionNextEdge.z));

            if(newEdgeAtIntersectionHeight->distanceXY(vertexAtCurrentHeight) < /*0.00001f*/ 0.0005f) {
                filtredEdges->push_back(edge);
            }

            delete newEdgeAtIntersectionHeight;
        }
    }

    delete vertexAtCurrentHeight;

    intersection.edgeVector = filtredEdges;
}

void ActivePlan::insert2Edges(Edge* oldEdge, Edge* newEdge1, Edge* newEdge2) {
    unsigned int size = activePlan->size();
    bool found = false;
    for(unsigned int i(0); (i < size) && !found; ++i) {
        Edge* currentEdge = (*activePlan)[i];
        if(currentEdge == oldEdge) {
            found = true;
            activePlan->erase(activePlan->begin() + i);
            activePlan->insert(activePlan->begin() + i, newEdge1);
            activePlan->insert(activePlan->begin() + i + 1, newEdge2);
        }
    }

    if(found == false) {
        std::cerr << "insert2Edges Error, found = false" << std::endl;
    }
}

void ActivePlan::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    triangles->push_back(triangleVertex1);
    triangles->push_back(triangleVertex2);
    triangles->push_back(triangleVertex3);
}

void ActivePlan::print(bool onlyValidEdge) {
    std::cerr << "Active Plan: " << std::endl;
    if(onlyValidEdge) {
        foreach(Edge* e, *activePlan) {
            if(e->isValid()) {
                std::cerr << "    " << *e << std::endl;
            }
        }
    } else {
        foreach(Edge* e, *activePlan) {
            std::cerr << "    " << *e << std::endl;
        }
    }
}

unsigned int ActivePlan::numberValidEdge() {
    unsigned int numberValidEdgeCounter(0);
    foreach(Edge* e, *activePlan) {
        if(e->isValid()) {
            numberValidEdgeCounter++;
        }
    }

    return numberValidEdgeCounter;
}

void ActivePlan::getActivePlanCopy(std::vector< std::vector< Edge* > >* copy) {
    std::vector< Edge* > currentLevel;
    foreach(Edge* e, *activePlan) {
        Vertex* v1 = e->getVertex1();
        Vertex* v2 = e->getVertex2();
        currentLevel.push_back(new Edge(new Vertex(v1->getX(), v1->getY(), v1->getZ()), new Vertex(v2->getX(), v2->getY(), v2->getZ())));
    }
    copy->push_back(currentLevel);
}

void ActivePlan::updateHeight(float height) {
    Plan horizontalPlan(0.0f, 0.0f, height, 0.0f, 0.0f, 1.0f);

    foreach(Edge* edge, *activePlan) {
        // previousEdge(plan1) - currentEdge(currentPlan) - nextEdge(plan2)
        Vertex* vertex1 = edge->getVertex1();
        Vertex* vertex2 = edge->getVertex2();

        Plan* plan1 = vertex1->getEdge1()->getDirectionPlan();
        Plan* currentPlan = edge->getDirectionPlan();
        Plan* plan2 = vertex2->getEdge2()->getDirectionPlan();

        if(vertex1->getZ() + 0.00001f < height) {
            //std::cerr << vertex1->getZ() << ", " << height << std::endl;
            Intersection newIntersection1 = horizontalPlan.intersect3Plans(plan1, currentPlan);

            Vertex newIntersectionVertex1(newIntersection1.x, newIntersection1.y, newIntersection1.z);
            if (std::abs(vertex1->distance(&newIntersectionVertex1)) > 0.0001f) {
                addNewTriangle(vertex1, vertex2, &newIntersectionVertex1);
                addNewTriangle(vertex1->getEdge1()->getVertex1(), vertex1, &newIntersectionVertex1);
            }

            vertex1->setX(newIntersection1.x);
            vertex1->setY(newIntersection1.y);
            vertex1->setZ(newIntersection1.z);
        }

        if(vertex2->getZ() + 0.00001f < height) {
            //std::cerr << vertex2->getZ() << ", " << height << std::endl;
            Intersection newIntersection2 = horizontalPlan.intersect3Plans(currentPlan, plan2);

            Vertex newIntersectionVertex2(newIntersection2.x, newIntersection2.y, newIntersection2.z);
            if (std::abs(vertex2->distance(&newIntersectionVertex2)) > 0.0001f) {
                addNewTriangle(vertex1, vertex2, &newIntersectionVertex2);
                addNewTriangle(vertex2, vertex2->getEdge2()->getVertex2(), &newIntersectionVertex2);
            }

            vertex2->setX(newIntersection2.x);
            vertex2->setY(newIntersection2.y);
            vertex2->setZ(newIntersection2.z);
        }
    }
}