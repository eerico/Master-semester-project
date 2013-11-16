#include "Reconstruction3D.h"

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles)
{
    priorityQueue = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;
    activePlan = new std::vector< Edge* >;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
    delete activePlan;
}

void Reconstruction3D::reconstruct()
{
    //set the active plan
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        activePlan->push_back(currentVertex->getEdge2());
        currentVertex = currentVertex->getNeighbor2();
    }

    //main loop
    addEdgeDirectionEvent();
    computeIntersection();
    while(priorityQueue->size() > 0) {
        Intersection event = priorityQueue->top();
        priorityQueue->pop();
        handleEvent(event);
    }

    //reset to inital state
    currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        currentVertex->getEdge2()->getProfile()->resetDirectionPlan();
        currentVertex = floorPlan->getNeighbor2();
    }
}

void Reconstruction3D::computeIntersection()
{
    activePlan = cloneActivePlan(); // vraiment utile de le faire a chaque fois ?

    unsigned int activePlanSize = activePlan->size();
    for(unsigned int i(0); i < activePlanSize; ++i) {
        Edge* edge2 = (*activePlan)[i];
        Edge* edge3 = (*activePlan)[(i+1) % activePlanSize];

        foreach(Edge* edge1, *activePlan) {
            if(edge1 == edge2 || edge1 == edge3) {
                continue;
            }

            Intersection intersection = intersect(edge1, edge2, edge3);
            if(intersection.eventType != NoIntersection) {
                priorityQueue->push(intersection);
            }
        }
    }
}

void Reconstruction3D::addEdgeDirectionEvent()
{
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = currentVertex->getEdge2();
        Vertex* currentProfileVertex = currentEdge->getProfile()->getProfileVertex();
        currentProfileVertex = currentProfileVertex->getNeighbor2();

        while(currentProfileVertex != 0 && currentProfileVertex->getNeighbor2() != 0) {
            Intersection edgeDirectionEvent;
            edgeDirectionEvent.edgeVector = new std::vector< Edge* >;
            edgeDirectionEvent.edgeVector->push_back(currentEdge);
            edgeDirectionEvent.eventType = EdgeDirection;
            edgeDirectionEvent.z = currentProfileVertex->getY();
            priorityQueue->push(edgeDirectionEvent);
            currentProfileVertex = currentProfileVertex->getNeighbor2();
        }
        currentVertex = floorPlan->getNeighbor2();
    }
}

Intersection Reconstruction3D::intersect(Edge *edge1, Edge *edge2, Edge *edge3)
{
    Vertex* vertex1 = edge1->getVertex1();
    Plan plan1( vertex1->getX(), vertex1->getY(), vertex1->getZ());
    plan1.computePlanNormal(vertex1, edge1->getVertex2(), edge1->getProfile());

    Vertex* vertex2 = edge2->getVertex1();
    Plan plan2(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    plan2.computePlanNormal(vertex2, edge2->getVertex2(), edge2->getProfile());

    Vertex* vertex3 = edge3->getVertex1();
    Plan plan3(vertex3->getX(), vertex3->getY(), vertex3->getZ());
    plan3.computePlanNormal(vertex3, edge3->getVertex2(), edge3->getProfile());

    Intersection intersection = plan1.intersect3Plans(plan2, plan3);

    intersection.edgeVector = new std::vector< Edge* >;
    intersection.edgeVector->push_back(edge1);
    intersection.edgeVector->push_back(edge2);
    intersection.edgeVector->push_back(edge3);

    return intersection;
}


void Reconstruction3D::handleEvent(Intersection& intersection)
{
    switch(intersection.eventType){
        case EdgeDirection:
        {

            /*
             * TODO: Le probleme ici c est que un profil peut etre le meme sur different edge
             * et donc on doit pas faire le nextDirectionPlan pour chaque edge qui on le meme profile
             * mais pour que ceux qui ont des profiles differents. Une idée serait de prendre tous les edges
             * qui on le meme profiles et d appliquer les changement une seul fois et des creer pour chacun de ses edges
             * les triangles
             */
            Edge* edge = (*intersection.edgeVector)[0];

            edgeDirectionHandling(intersection);

            Profile* profile = edge->getProfile();
            profile->nextDirectionPlan();

            removeInvalidIntersection(edge, intersection.y);

            computeIntersection();

            break;
        }
        case General:
        {
        std::cerr << "intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            eventClustering(intersection);

            removeDuplicateEdges(intersection);

            // if the number of edges in f is less than 3, the event is ignored
            if(intersection.edgeVector->size() < 3) {
                std::cerr << "intersection ignored" << std::endl;
                return;
            }

            std::cerr << "before update" << std::endl;
            printActivePlan();

            Chain currentChain(intersection, activePlan, triangles);

            std::vector< Edge* > oldActivePlan;
            pointerCloneActivePlan(oldActivePlan);


            unsigned int numberTrianglesAdded = triangles->size();

            currentChain.intraChainHandling();
            removeInvalidEdge(activePlan);
            currentChain.interChainHandling();

            //test if self intersection
            bool interectionValid(true);
            for(unsigned int i(0); i < activePlan->size() && interectionValid; ++i) {
                Edge* edge = (*activePlan)[i];
                for(unsigned int j(i+1); j < activePlan->size() && interectionValid; ++j) {
                    Edge* comparedEdge = (*activePlan)[j];
                    if(edge->isParallel(comparedEdge)) {
                        if(edge->distance(comparedEdge) < 0.001f) {
                            interectionValid = false;
                            std::cerr << "Self intersection" << std::endl;
                        }
                    }
                }
            }


            //the intersection is not valid, thus we revert the active plan
            // and remove the added triangles
            if(!interectionValid) {
                unsigned int size = oldActivePlan.size();
                activePlan->clear();
                for(unsigned int i(0); i < size; ++i) {
                    Edge* edge = oldActivePlan[i];
                    edge->revert();
                    activePlan->push_back(edge);
                }

                numberTrianglesAdded = triangles->size() - numberTrianglesAdded;
                for(unsigned int i(0); i < numberTrianglesAdded; ++i) {
                   triangles->pop_back();
                }
            }

            std::cerr << "after update wih correction" << std::endl;
            printActivePlan();
            std::cerr << "......................................................................." << std::endl;
            break;
        }
    }
}

void Reconstruction3D::printActivePlan() {
    std::vector<Edge* > ap = *activePlan;
    unsigned int size = ap.size();

    std::cerr << "ACTIVE PLAN" << std::endl;
    for(unsigned int i(0); i < size; ++i) {
        std::cerr << *ap[i] << std::endl;
    }
    std::cerr << std::endl;
}

void Reconstruction3D::pointerCloneActivePlan(std::vector<Edge *> &clone)
{
    unsigned int size = activePlan->size();
    for(unsigned int i(0); i < size; ++i) {
        clone.push_back((*activePlan)[i]);
    }
}

void Reconstruction3D::edgeDirectionHandling(Intersection &intersection)
{
    // we first compute the intersections with the two neighbor profiles
    // and then create the triangles

    /*Edge* edge = (*intersection.edgeVector)[0];

    // we compute the intersection using the plan defined with the edge+profile associated with the edge direction event,
    // the neighbor edge+profile and an horizontal plan

    // the edge+profile associated with the edge direction event
    Vertex* vertex1 = edge->getVertex1();
    Vertex* vertex2 = edge->getVertex2();
    Plan plan1;
    plan1.pointX = vertex1->getX();
    plan1.pointY = vertex1->getY();
    plan1.pointZ = vertex1->getZ();
    computePlanNormal(vertex1, vertex2, edge->getProfile(), plan1.normalX, plan1.normalY, plan1.normalZ);

    // the neighbor edge+profile
    Edge* neighborEdge = edge->getVertex1()->getEdge1();
    Vertex* neighborVertex = neighborEdge->getVertex1();
    Plan plan2;
    plan2.pointX = neighborVertex ->getX();
    plan2.pointY = neighborVertex ->getY();
    plan2.pointZ = neighborVertex ->getZ();
    computePlanNormal(neighborVertex , neighborEdge->getVertex2(), neighborEdge->getProfile(), plan2.normalX, plan2.normalY, plan2.normalZ);

    // the horizontal plan
    Plan plan3;
    plan2.pointX = 0.0f;
    plan2.pointY = 0.0f;
    plan2.pointZ = intersection.z;
    plan2.normalX = 0.0f;
    plan2.normalY = 0.0f;
    plan2.normalZ = 1.0f;

    //compute the intersection and add the triangle
    Intersection newIntersection = intersect3Plans(plan1, plan2, plan3);
    Vertex* intersectionVertex = new Vertex(newIntersection.x, newIntersection.y, newIntersection.z);

    addNewTriangle(vertex1, vertex2, intersectionVertex);


    //Now do the same but for the other neighbor
    // the neighbor edge+profile
    neighborEdge = edge->getVertex2()->getEdge2();
    neighborVertex = neighborEdge->getVertex1();
    plan2.pointX = neighborVertex ->getX();
    plan2.pointY = neighborVertex ->getY();
    plan2.pointZ = neighborVertex ->getZ();
    computePlanNormal(neighborVertex , neighborEdge->getVertex2(), neighborEdge->getProfile(), plan2.normalX, plan2.normalY, plan2.normalZ);

    //compute the intersection and add the triangle
    newIntersection = intersect3Plans(plan1, plan2, plan3);
    Vertex* intersectionVertex2 = new Vertex(newIntersection.x, newIntersection.y, newIntersection.z);

    addNewTriangle(vertex2, intersectionVertex, intersectionVertex2);

    edge->setVertex1(intersectionVertex);
    edge->setVertex2(intersectionVertex2);*/
}

void Reconstruction3D::removeInvalidIntersection(Edge *edge, float height)
{
    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>* priorityQueue2 =
            new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;

    while(priorityQueue->size() > 0) {
        Intersection event = priorityQueue->top();
        priorityQueue->pop();
        std::vector<Edge*>* edges = event.edgeVector;
        Edge* edge1 = (*edges)[0];
        Edge* edge2 = (*edges)[1];
        Edge* edge3 = (*edges)[2];

        if(event.eventType == EdgeDirection) {
            priorityQueue2->push(event);
        }else{
            if ((edge1 == edge) || (edge2 == edge) || (edge3 == edge)) {
                if (event.z < height) {
                    priorityQueue2->push(event);
                }
            } else {
                priorityQueue2->push(event);
            }
        }
    }

    delete priorityQueue;
    priorityQueue = priorityQueue2;
}

void Reconstruction3D::eventClustering(Intersection& intersection)
{
    float y(intersection.y);

    float delta1(0.01f);
    float delta2(0.01f);

    bool stop(false);

    std::vector<Edge*>* intersectionEdges = intersection.edgeVector;
    removeInvalidEdge(intersectionEdges);

    while(!stop && (priorityQueue->size() > 0)){
        Intersection event = priorityQueue->top();

        //std::cerr << "      MM: " << std::abs(event.y - y) << ", " << Utils::distance(event.x, event.z, intersection.x, intersection.z) << std::endl;
        if ((std::abs(event.y - y) < delta1) && (Utils::distance(event.x, event.z, intersection.x, intersection.z) < delta2)) {
            priorityQueue->pop();
            std::vector<Edge*>* eventEdges = event.edgeVector;

            removeInvalidEdge(eventEdges);

            for(unsigned int i(0); i < eventEdges->size(); ++i) {
                intersectionEdges->push_back((*eventEdges)[i]);
            }
        } else {
            stop = true;
        }
    }
}

void Reconstruction3D::removeInvalidEdge(std::vector<Edge *> *edges){
    unsigned int size = edges->size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = (*edges)[i];
        if(!currentEdge->isValid()) {
            edges->erase(edges->begin() + i);
            i--;
            size--;
        }
    }
}

// this will allow us to manipulate edges while working on the activePlan
std::vector< Edge* >* Reconstruction3D::cloneActivePlan()
{
    std::vector<Edge* >* cloneActivePlan = new std::vector< Edge* >;
    unsigned int size = activePlan->size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = (*activePlan)[i];
        Edge* cloneEdge = new Edge(currentEdge->getVertex1(), currentEdge->getVertex2(), currentEdge->getProfile());
        cloneActivePlan->push_back(cloneEdge);
    }
    return cloneActivePlan;
}

void Reconstruction3D::removeDuplicateEdges(Intersection& intersection)
{
    std::vector< Edge* >* edges = intersection.edgeVector;
    //remove duplicate
    unsigned int size = edges->size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = (*edges)[i];
        for(unsigned int j(i+1); j < size; ++j) {
            Edge* comparedEdge = (*edges)[j];
            if (currentEdge == comparedEdge) {
                edges->erase(edges->begin() + j);
                j--;
                size--;
            }
        }
    }
}



