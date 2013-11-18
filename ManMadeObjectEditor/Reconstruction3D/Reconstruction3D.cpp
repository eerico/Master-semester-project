#include "Reconstruction3D.h"

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles), currentChain(0)
{
    priorityQueue = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
}

void Reconstruction3D::reconstruct()
{
    //set the current chain
    currentChain = new Chain(floorPlan, floorPlanSize);
    currentChain->printChain();

    //main loop
    addEdgeDirectionEvent();
    computeIntersection();
    while((priorityQueue->size() > 0) && !currentChain->isEmpty()) {
        Intersection event = priorityQueue->top();
        priorityQueue->pop();
        handleEvent(event);
    }

    //reset to inital state
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        currentVertex->getEdge2()->getProfile()->resetDirectionPlan();
        currentVertex = floorPlan->getNeighbor2();
    }
}

void Reconstruction3D::computeIntersection() ////////////////
{
    std::vector< std::vector< Edge* >* >* chains = currentChain->getChains();
    std::vector< Edge* > unrolledChain;
    unsigned int numberChain = chains->size();
    for(unsigned int i(0); i < numberChain; ++i) {
        std::vector< Edge* >* subChain = (*chains)[i];
        unsigned int numberEdge = subChain->size();
        for(unsigned int j(0); j < numberEdge; ++j) {
            unrolledChain.push_back((*subChain)[j]);
        }
    }

    unsigned int unrolledChainSize = unrolledChain.size();
    for(unsigned int i(0); i < unrolledChainSize; ++i) {
        Edge* edge2 = unrolledChain[i];
        Edge* edge3 = unrolledChain[(i+1) % unrolledChainSize];

        foreach(Edge* edge1, unrolledChain) {
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

void Reconstruction3D::addEdgeDirectionEvent() ////////////////////////
{
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = currentVertex->getEdge2();
        Vertex* currentProfileVertex = currentEdge->getProfile()->getProfileVertex();
        currentProfileVertex = currentProfileVertex->getNeighbor2();

        while(currentProfileVertex != 0 && currentProfileVertex->getNeighbor2() != 0) {
            Intersection edgeDirectionEvent;
            edgeDirectionEvent.edge = currentEdge;
            edgeDirectionEvent.eventType = EdgeDirection;
            edgeDirectionEvent.z = currentProfileVertex->getY();
            priorityQueue->push(edgeDirectionEvent);
            currentProfileVertex = currentProfileVertex->getNeighbor2();
        }
        currentVertex = floorPlan->getNeighbor2();
    }
}

Intersection Reconstruction3D::intersect(Edge *edge1, Edge *edge2, Edge *edge3) ////////////////////////
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

    return intersection;
}


void Reconstruction3D::handleEvent(Intersection& intersection) ////////////////////////////////
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
            Edge* edge = intersection.edge;

            edgeDirectionHandling(intersection);

            Profile* profile = edge->getProfile();
            profile->nextDirectionPlan();

            computeIntersection();

            break;
        }
        case General:
        {
            if(intersection.z == 0.0f) {
                return;
            }
            std::cerr << "intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            eventClustering(intersection);

            Chain* chain = new Chain(intersection.z, currentChain, triangles);
            chain->printChain();

            chain->intraChainHandling();
            chain->interChainHandling();

            delete currentChain;
            currentChain = chain;

            std::cerr << "after update" << std::endl;
            currentChain->printChain();
            std::cerr << "......................................................................." << std::endl;
            break;
        }
    }
}

void Reconstruction3D::edgeDirectionHandling(Intersection &intersection) //////////////////
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

void Reconstruction3D::eventClustering(Intersection& intersection) ////////////////////
{
    float z = intersection.z;

    float delta1(0.01f);

    bool stop(false);

    while(!stop && (priorityQueue->size() > 0)){
        Intersection event = priorityQueue->top();

        //std::cerr << "      MM: " << std::abs(event.y - y) << ", " << Utils::distance(event.x, event.z, intersection.x, intersection.z) << std::endl;
        if ((event.eventType != EdgeDirection) && (std::abs(event.z - z) < delta1)) {
            priorityQueue->pop();
        } else {
            stop = true;
        }
    }
}
