#include "Reconstruction3D.h"

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles)
{
    priorityQueue = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
}

void Reconstruction3D::reconstruct()
{
    //set the current ActivePlan
    activePlan = new ActivePlan(floorPlan, floorPlanSize, triangles);

    //main loop
    addEdgeDirectionEvent();


    computeIntersection();
    while((priorityQueue->size() > 0)) {
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

void Reconstruction3D::eventClustering(Intersection& intersection)
{
    float z(intersection.z);

    float delta1(0.0001f);
    float delta2(0.000001f);

    bool stop(false);

    std::vector<Edge*>* intersectionEdges = intersection.edgeVector;

    while(!stop && (priorityQueue->size() > 0)){
        Intersection event = priorityQueue->top();

        if ((std::abs(event.z - z) < delta1) && (Utils::distance(event.x, event.y, intersection.x, intersection.y) < delta2)) {
            priorityQueue->pop();
            std::vector<Edge*>* eventEdges = event.edgeVector;

            for(unsigned int i(0); i < eventEdges->size(); ++i) {
                intersectionEdges->push_back((*eventEdges)[i]);
            }
        } else {
            stop = true;
        }
    }
}

void Reconstruction3D::computeIntersection()
{

    std::vector< Edge* >* plans = activePlan->getPlans();



    unsigned int numberEdges = plans->size();

    for(unsigned int i(0); i < numberEdges; ++i) {
        Edge* edge2 = (*plans)[i];
        Edge* edge3 = edge2->getVertex2()->getEdge2();

        foreach(Edge* edge1, (*plans)) {
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
    intersection.edgeVector = new std::vector< Edge* >;
    intersection.edgeVector->push_back(edge1);
    intersection.edgeVector->push_back(edge2);
    intersection.edgeVector->push_back(edge3);

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

            std::cerr << "intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;

            std::cerr << "......................................................................." << std::endl;
            break;
        }
    }
}

void Reconstruction3D::edgeDirectionHandling(Intersection &intersection) //////////////////
{

}
