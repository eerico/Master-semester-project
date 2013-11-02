#include "Reconstruction3D.h"

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *>* triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles), currentHeight(0)
{
    priorityQueue = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparison>;
    activePlan = new std::vector< Vertex* >;
    allActivePlan = new std::vector< std::vector< Vertex* >* >;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
    unsigned int size = allActivePlan->size();
    std::vector< Vertex* >* currentPlan;
    for(unsigned int i(0); i < size; ++i) {
        currentPlan = (*allActivePlan)[i];
        delete currentPlan;
    }
    allActivePlan->clear();
    delete allActivePlan;
}

void Reconstruction3D::reconstruct()
{
    //set the active plan
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        activePlan->push_back(currentVertex);
        currentVertex = floorPlan->getNeighbor2();
    }
    allActivePlan->push_back(activePlan);

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

// voir fig. 8, calcul intersection pour l active plan
// et met dans la priority queue. Doit etre
// appeler si edge direction event
void Reconstruction3D::computeIntersection()
{
    // LA meilleur idée est de faire la copy du active plan ici, avant les 2 foreach

    foreach(Vertex* vertex, *activePlan) {
        foreach(Vertex* vertex2, *activePlan) {
            Edge* edge1 = vertex2->getEdge2();
            Edge* edge2 = vertex->getEdge1();
            Edge* edge3 = vertex->getEdge2();

            Intersection intersection = intersect(edge1, edge2, edge3, currentHeight);
            priorityQueue->push(intersection);
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

            edgeDirectionEvent.edgeVector.push_back(currentEdge);
            edgeDirectionEvent.eventType = EdgeDirection;
            edgeDirectionEvent.y = currentProfileVertex->getY();
            priorityQueue->push(edgeDirectionEvent);
        }
        currentVertex = floorPlan->getNeighbor2();
    }
}

Reconstruction3D::Intersection Reconstruction3D::intersect(Edge *edge1, Edge *edge2, Edge *edge3, float currentHeight)
{
    Vertex* vertex1 = edge1->getVertex1();
    float p1 = vertex1->getX();
    float p2 = currentHeight;
    float p3 = vertex1->getY();
    float n1(0.0f);
    float n2(0.0f);
    float n3(0.0f);
    computePlanNormal(vertex1, edge1->getVertex2(), edge1->getProfile(), n1, n2, n3);

    Vertex* vertex2 = edge2->getVertex1();
    float p1_p = vertex2->getX();
    float p2_p = currentHeight;
    float p3_p = vertex2->getY();
    float n1_p(0.0f);
    float n2_p(0.0f);
    float n3_p(0.0f);
    computePlanNormal(vertex2, edge2->getVertex2(), edge2->getProfile(), n1_p, n2_p, n3_p);

    Vertex* vertex3 = edge3->getVertex1();
    float p1_pp = vertex3->getX();
    float p2_pp = currentHeight;
    float p3_pp = vertex3->getY();
    float n1_pp(0.0f);
    float n2_pp(0.0f);
    float n3_pp(0.0f);
    computePlanNormal(vertex3, edge3->getVertex2(), edge3->getProfile(), n1_pp, n2_pp, n3_pp);

    float A = n1 - n1_p;
    float B = p1 * n1 / A;
    float C = p2 * n2 / A;
    float D = p3 * n3 / A;
    float E = p1_p * n1_p / A;
    float F = p2_p * n2_p / A;
    float G = p3_p * n3_p / A;
    float H = B + C + D - E - F - G;
    float I = (n2_p - n2) / A;
    float J = (n3_p - n3) / A;
    float K = I * n1 + n2 - I * n1_pp - n2_pp;
    float L = (-H * n1 + p1 * n1 + p2 * n2 + p3 * n3 + H * n1_pp - p1_pp * n1_pp - p2_pp * n2_pp - p3_pp * n3_pp) / K;
    float M = -J * n1 - n3 + J * n1_pp + n3_pp;
    float N = H + L * I;
    float Q = M * I + J;
    float R = N * n1_p - p1_p * n1_p + L * n2_p - p2_p * n2_p - p3_p * n3_p - N * n1_pp + p1_pp * n1_pp - L * n2_pp + p2_pp * n2_pp + p3_pp * n3_pp;
    float S = Q * n1_p + M * n2_p + n3_p - Q * n1_pp - M * n2_pp - n3_pp;

    float x3 = -R / S;
    float x2 = L + x3 * M;
    float x1 = N + x3 * Q;

    Intersection intersection;
    intersection.edgeVector.push_back(edge1);
    intersection.edgeVector.push_back(edge2);
    intersection.edgeVector.push_back(edge3);
    intersection.eventType = General;
    intersection.x = x1;
    intersection.y = x2;
    intersection.z = x3;

    return intersection;
}


void Reconstruction3D::computePlanNormal(Vertex* vertex1, Vertex* vertex2, Profile* profile, float& nx, float& ny, float& nz)
{
    float a = vertex2->getX() - vertex1->getX();
    float b = 0.0f;
    float c = vertex2->getY() - vertex1->getY();

    Utils::normalize(a, c);

    Vertex* profileVertex = profile->getProfileVertex();
    Vertex* nextProfileVertex = profileVertex->getNeighbor2();

    float w = nextProfileVertex->getX() - profileVertex->getX();
    float y = nextProfileVertex->getY() - profileVertex->getY();

    //est ce correct ou bien sa joue pas avec orientation des edges ?
    float wx = c;
    float wz = -a;

    float d = w * wx;
    float e = y;
    float f = w * wz;

    Utils::crossProduct(d, e, f, a, b, c, nx, ny, nz);
    Utils::normalize(nx, ny, nz);
}

// faire les truc magique. Pas oublier de update le active plan, et de set les profiles
// (pVertex) a la bonne hauteur
void Reconstruction3D::handleEvent(Intersection& intersection)
{
    switch(intersection.eventType){
        case EdgeDirection:
        {
            Edge* edge = intersection.edgeVector[0];
            Profile* profile = edge->getProfile();
            profile->nextDirectionPlan();
            currentHeight = intersection.y;
            removeInvalidIntersection(edge, intersection.y);
            computeIntersection();

            break;
        }
        case General:
        {
            // on va toucher au edge donc faire une copy!!! a la fin on devrai alors avoir un noveau active plan
            // voir intersect pour copy
            eventClustering(intersection);
            std::vector< std::vector< Edge* > > chains;
            chainConstruction(intersection, chains);
            intraChainHandling(chains);
            interChainHandling(chains);


            // maintenant en theorie il doit y avoir moyen d avoir un nouveau active plan et de le push
            // dans all active plan
            break;
        }
    }
}

void Reconstruction3D::removeInvalidIntersection(Edge *edge, float height)
{
    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparison>* priorityQueue2 =
            new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparison>;

    while(priorityQueue->size() > 0) {
        Intersection event = priorityQueue->top();
        priorityQueue->pop();
        std::vector<Edge*> edges = event.edgeVector;
        Edge* edge1 = edges[0];
        Edge* edge2 = edges[1];
        Edge* edge3 = edges[2];

        if ((edge1 == edge) || (edge2 == edge) || (edge3 == edge)) {
            if (event.y < height) {
                priorityQueue2->push(event);
            }
        } else {
            priorityQueue2->push(event);
        }
    }

    delete priorityQueue;
    priorityQueue = priorityQueue2;
}

void Reconstruction3D::eventClustering(Intersection& intersection)
{
    float y(intersection.y);
    float delta1(0.001f);
    float delta2(0.00001f);
    bool stop(false);
    std::vector<Edge*> edges = intersection.edgeVector;
    do {
        Intersection event = priorityQueue->top();
        if ((std::abs(event.y - y) < delta1) && (Utils::distance(event.x, event.z, intersection.x, intersection.z) < delta2)) {
            priorityQueue->pop();
            std::vector<Edge*> eventEdges = event.edgeVector;
            for(unsigned int i(0); i < eventEdges.size(); ++i) {
                edges.push_back(eventEdges[i]);
            }
        } else {
            stop = true;
        }

    } while (!stop);
}


std::vector< Vertex* >* Reconstruction3D::clonePlan(std::vector<Vertex *> *activePlan)
{
///////////////TODO
   std::vector<Vertex *>* clone = new std::vector< Vertex* >;
   unsigned int size = activePlan->size();
       for(unsigned int i(0); i < size; ++i) {
           Vertex* currentVertex = (*activePlan)[i];
           Vertex* neighbor = currentVertex->getNeighbor2();

       }
    return 0;
}

void Reconstruction3D::chainConstruction(Intersection& intersection, std::vector< std::vector< Edge* > >& chains)
{
    std::vector< Edge* > edges = intersection.edgeVector;
    //first remove duplicate
    unsigned int size = edges.size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = edges[i];
        for(unsigned int j(i+1); j < size; ++j) {
            Edge* comparedEdge = edges[i];
            if (currentEdge == comparedEdge) {
                edges.erase(edges.begin() + j);
                j--;
                size--;
            }
        }
    }

    //create the chains
    Vertex* first = 0;
    Vertex* last = 0;

    for(unsigned int i(0); i < size; ++i) {
        std::vector< Edge* > currentChain;
        Edge* currentEdge = edges[i];
        first = currentEdge->getVertex1();
        last = currentEdge->getVertex2();
        currentChain.push_back(currentEdge);

        for(unsigned int j(i+1); j < size; ++j) {
            Edge* comparedEdge = edges[i];

            Vertex* vertex1 = comparedEdge->getVertex1();
            Vertex* vertex2 = comparedEdge->getVertex2();

            if(vertex1 == first) {
                currentChain.insert(currentChain.begin(), comparedEdge);
                edges.erase(edges.begin() + j);
                size--;
                j = i + 1;
                first = vertex1;
            } else if(vertex2 == first) {
                currentChain.insert(currentChain.begin(), comparedEdge);
                edges.erase(edges.begin() + j);
                size--;
                j = i + 1;
                first = vertex2;
            } else if(vertex1 == last) {
                currentChain.push_back(comparedEdge);
                edges.erase(edges.begin() + j);
                size--;
                j = i + 1;
                last = vertex1;
            } else if(vertex2 == last) {
                currentChain.push_back(comparedEdge);
                edges.erase(edges.begin() + j);
                size--;
                j = i + 1;
                last = vertex2;
            }
        }

        chains.push_back(currentChain);
    }
}

void Reconstruction3D::interChainHandling(std::vector< std::vector< Edge* > >& chains)
{

}

void Reconstruction3D::intraChainHandling(std::vector< std::vector< Edge* > >& chains)
{
    //mettre un boolean dans Edge pour valid/invalid et ensuite effacer tous ceux qui sont invalid après qu on a traiter les intraChain
}
