#include "Reconstruction3D.h"

/***
 *
 *
 *
 *
 * reste a implemente: un moyen de stock tout les edge/vertex que je creer pour les delete a la fin..
 *                      creer triangle et mettre dans le vector correpsondant
 * pas implementer: near horizontal edge detection, profile offset event, filtering invalid event, post inter chain intersection
 *
 *
 **/

/*
 * Here, the z value is the up vector (in order to match the 3D representation used with QGLViewer)
 *
 */


Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *> *triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles)
{
    priorityQueue = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;
    activePlan = new std::vector< Edge* >;
    allActivePlan = new std::vector< std::vector< Edge* >* >; // vraiment utile ?
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
    unsigned int size = allActivePlan->size();
    std::vector< Edge* >* currentPlan;
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
        activePlan->push_back(currentVertex->getEdge2());
        currentVertex = currentVertex->getNeighbor2();
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

void Reconstruction3D::computeIntersection()
{
    activePlan = cloneActivePlan(); // vraiment utile de le faire a chaque fois ?
    allActivePlan->push_back(activePlan);

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

    float normalX(0.0f);
    float normalY(0.0f);
    float normalZ(0.0f);
    computePlanNormal(vertex1, edge1->getVertex2(), edge1->getProfile(), normalX, normalY, normalZ);
    plan1.setNormal(normalX, normalY, normalZ);


    Vertex* vertex2 = edge2->getVertex1();
    Plan plan2(vertex2->getX(), vertex2->getY(), vertex2->getZ());

    normalX = 0.0f;
    normalY = 0.0f;
    normalZ = 0.0f;
    computePlanNormal(vertex2, edge2->getVertex2(), edge2->getProfile(), normalX, normalY, normalZ);
    plan2.setNormal(normalX, normalY, normalZ);


    Vertex* vertex3 = edge3->getVertex1();
    Plan plan3(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    normalX = 0.0f;
    normalY = 0.0f;
    normalZ = 0.0f;
    computePlanNormal(vertex3, edge3->getVertex2(), edge3->getProfile(), normalX, normalY, normalZ);
    plan3.setNormal(normalX, normalY, normalZ);


    Intersection intersection = plan1.intersect3Plans(plan2, plan3);

    intersection.edgeVector = new std::vector< Edge* >;
    intersection.edgeVector->push_back(edge1);
    intersection.edgeVector->push_back(edge2);
    intersection.edgeVector->push_back(edge3);

    return intersection;
}



void Reconstruction3D::computePlanNormal(Vertex* vertex1, Vertex* vertex2, Profile* profile, float& nx, float& ny, float& nz)
{
    float a = vertex2->getX() - vertex1->getX();
    float b = vertex2->getY() - vertex1->getY();
    float c = 0.0f;

    Utils::normalize(a, b);

    Vertex* profileVertex = profile->getProfileVertex();
    Vertex* nextProfileVertex = profileVertex->getNeighbor2();

    float w = nextProfileVertex->getX() - profileVertex->getX();
    float z = nextProfileVertex->getY() - profileVertex->getY();

    //est ce correct ou bien sa joue pas avec orientation des edges ?
    float wx = b;
    float wy = -a;

    float d = w * wx;
    float e = w * wy;
    float f = z;

    Utils::crossProduct(d, e, f, a, b, c, nx, ny, nz);
    Utils::normalize(nx, ny, nz);















    // show the profile orientation
    //////////////////////////////////////////////////////////////////////////////////////

    /*float aa = vertex2->getX() + vertex1->getX();
    float bb = vertex2->getY() + vertex1->getY();
    float cc = 0.0f;

    Vertex* v3  = new Vertex(d+ 0.5f*aa, e + 0.5f*bb);
    v3->setZ(f + 0.5f*cc);
    Vertex* v  = new Vertex(0.5f*aa, 0.5f*bb);
    v->setZ(cc);
    addNewTriangle(v, v3, v);*/

    //////////////////////////////////////////////////////////////////////////////////////
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
        std::cerr << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            eventClustering(intersection);

            removeDuplicateEdges(intersection);

            // if the number of edges in f is less than 3, the event is ignored
            if(intersection.edgeVector->size() < 3) {
                std::cerr << "intersection ignored" << std::endl;
                return;
            }

            std::vector< std::vector< Edge* >* > chains;
            chainConstruction(intersection, chains);

            intraChainHandling(chains, intersection);
            interChainHandling(chains, intersection);

            break;
        }
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

/*
bool Reconstruction3D::isIntersectionExternToEdges(Intersection &intersection)
{
    std::vector< Edge* >* edges = intersection.edgeVector;
    foreach(Edge* edge, (*edges)){
        Vertex* vertex1 = edge->getVertex1();
        Vertex* vertex2 = edge->getVertex2();

        float x1 = vertex1->getX();
        float y1 = vertex1->getY();

        float x2 = vertex2->getX();
        float y2 = vertex2->getY();

        float x = intersection.x;
        float y = intersection.y;

        float distance1 = Utils::distance(x1, y1, x, y);
        float distance2 = Utils::distance(x2, y2, x, y);

        float vx(0.0f);
        float vy(0.0f);
        float vertexToIntersectionX(0.0f);
        float vertexToIntersectionY(0.0f);

        if (distance2 > distance1) {
            vx = x1 - x2;
            vy = y1 - y2;

            vertexToIntersectionX = x - x2;
            vertexToIntersectionY = y - y2;
        } else {
            vx = x2 - x1;
            vy = y2 - y1;

            vertexToIntersectionX = x - x1;
            vertexToIntersectionY = y - y1;
        }

        Utils::normalize(vx, vy);

        float projectedIntersectionDistance = Utils::dotProduct(vx, vy, vertexToIntersectionX, vertexToIntersectionY);

        if(projectedIntersectionDistance > Utils::distance(x2, y2, x1, y1)){
            return true;
        }
    }

    return false;
}*/

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

// hum les chaines devrait etre consecutive d apres comment on fait mais s adepend de plein de truc...
void Reconstruction3D::chainConstruction(Intersection& intersection, std::vector< std::vector< Edge* >* >& chains)
{
    std::vector< Edge* >* edges = intersection.edgeVector;
    unsigned int size = edges->size();

    ///////////////////////////////////////////////////////
    std::cerr << "avant chain" << std::endl;
    size = edges->size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = (*edges)[i];
        std::cerr << currentEdge << " - ";
    }
    std::cerr << std::endl;
    ///////////////////////////////////////////////////////

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

        chains.push_back(currentChain);
    }


    ///////////////////////////////////////////////////////
    std::cerr << "apres chain" << std::endl;
    foreach(std::vector< Edge* >* c, chains) {
        foreach(Edge* e, *c) {
            std::cerr << e << " - ";
        }
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
    ///////////////////////////////////////////////////////
}

void Reconstruction3D::splitEdgeAtCorner(Edge *edgeToSplit, Intersection& cornerIntersection, Edge*& newEdge1, Edge*& newEdge2)
{
    Vertex* cornerVertex = new Vertex(cornerIntersection.x, cornerIntersection.y, cornerIntersection.z);
    edgeToSplit->invalid();

    newEdge1 = new Edge(edgeToSplit->getVertex1(), cornerVertex, edgeToSplit->getProfile());
    newEdge2 = new Edge(cornerVertex, edgeToSplit->getVertex2(), edgeToSplit->getProfile());

    cornerVertex->setEdge1(newEdge1);
    cornerVertex->setEdge2(newEdge2);

    //remove invalid edge from the active plan and add the two new edges
    unsigned int activePlanSize = activePlan->size();
    bool found(false);
    for(unsigned int i(0); (i < activePlanSize) && !found; ++i) {
        Edge* currentEdge = (*activePlan)[i];
        if(!currentEdge->isValid()) {
            activePlan->erase(activePlan->begin() + i);
            delete currentEdge;

            activePlan->insert(activePlan->begin() + i, newEdge1);
            activePlan->insert(activePlan->begin() + i + 1, newEdge2);
            found = true;
        }
    }
}

void Reconstruction3D::interChainHandling(std::vector< std::vector< Edge* >* >& chains, Intersection& intersection)
{
    // le code est faux mais idée est juste. 1 edge se split en 2 et on reconstruit en dessus mais comment update dans
    // la priority queue pour chaque intersection ?

//

    // on suppose que les chaines sont bien successive...
    unsigned int chainsSize = chains.size();
    if (chainsSize < 2) {
        return;
    }
    for(unsigned int i(0); i < chainsSize; i++) {
        std::vector< Edge* >* chain1 = chains[i];
        std::vector< Edge* >* chain2 = chains[(i+1) % chainsSize];

        Vertex* intersectionVertex = new Vertex(intersection.x, intersection.y, intersection.z);

        Edge* lastEdgeChain1(0);
        Edge* firstEdgeChain2(0);

        if(chain1->size() == 1) {
            Edge* newEdge1(0);
            Edge* newEdge2(0);

            Edge* chainEdge = (*chain1)[0];
            splitEdgeAtCorner(chainEdge, intersection, newEdge1, newEdge2);

            chain1->pop_back();
            chain1->push_back(newEdge1);
            chain1->push_back(newEdge2);
            lastEdgeChain1 = newEdge2;
        } else {
            lastEdgeChain1 = (*chain1)[1];
        }

        if(chain2->size() == 1) {
            Edge* newEdge1(0);
            Edge* newEdge2(0);

            Edge* chainEdge = (*chain2)[0];
            splitEdgeAtCorner(chainEdge, intersection, newEdge1, newEdge2);

            chain2->pop_back();
            chain2->push_back(newEdge1);
            chain2->push_back(newEdge2);
            firstEdgeChain2 = newEdge1;
        } else {
            firstEdgeChain2 = (*chain2)[0];
        }
        lastEdgeChain1->setVertex1(intersectionVertex);
        firstEdgeChain2->setVertex2(intersectionVertex);

        intersectionVertex->setEdge1(firstEdgeChain2);
        intersectionVertex->setEdge2(lastEdgeChain1);
    }
}

void Reconstruction3D::intraChainHandling(std::vector< std::vector< Edge* >* >& chains, Intersection& intersection)
{

    unsigned int chainsSize = chains.size();
    for(unsigned int j(0); j < chainsSize; ++j) {
        std::vector< Edge* >* currentChain = chains[j];
        unsigned int currentChainSize = currentChain->size();

        if (currentChainSize > 2) {

            Edge* firstEdge = (*currentChain)[0];
            Edge* lastEdge = (*currentChain)[currentChainSize - 1];

            Edge* firstNeighbor = (*currentChain)[1];
            Edge* lastNeighbor = (*currentChain)[currentChainSize - 2];

            if(!firstEdge->isValid() || !lastEdge->isValid() || !firstNeighbor->isValid() || !lastNeighbor->isValid()) {
                break;
            }

            Vertex* intersectionVertex = new Vertex(intersection.x, intersection.y, intersection.z);

            // first check if the chain is circular. If it is the case, all edges are interior edges that will shrink to 0
            if (firstEdge->getVertex1() == lastEdge->getVertex2()) {
                //invalid all interior edges
                while(!currentChain->empty()){
                    Edge* edgeInvalid = (*currentChain)[currentChain->size()-1];
                    currentChain->pop_back();
                    edgeInvalid->invalid();

                    addNewTriangle(edgeInvalid->getVertex1(), edgeInvalid->getVertex2(), intersectionVertex);
                }
                chains.erase(chains.begin() + j);
                continue;
            }


            addNewTriangle(firstEdge->getVertex1(), firstEdge->getVertex2(), intersectionVertex);
            addNewTriangle(lastEdge->getVertex1(), lastEdge->getVertex2(), intersectionVertex);

            //find which vertex will be reassigned and reassigned it
            Vertex* vertex1 = firstEdge->getVertex1();
            if ((vertex1 != firstNeighbor->getVertex1()) && (vertex1 != firstNeighbor->getVertex2())) {
                firstEdge->setVertex2(intersectionVertex);
                intersectionVertex->setEdge1(firstEdge);
            } else {
                firstEdge->setVertex1(intersectionVertex);
                intersectionVertex->setEdge2(firstEdge);
            }

            //find which vertex will be reassigned and reassigned it
            Vertex* vertex2 = lastEdge->getVertex1();
            if ((vertex2 != lastNeighbor->getVertex1()) && (vertex2 != lastNeighbor->getVertex2())) {
                lastEdge->setVertex2(intersectionVertex);
                intersectionVertex->setEdge1(lastEdge);
            } else {
                lastEdge->setVertex1(intersectionVertex);
                intersectionVertex->setEdge2(lastEdge);
            }

            //invalid all interior edges
            for(unsigned int i(1); i < currentChainSize - 1; ++i) {
                Edge* edgeInvalid = (*currentChain)[i];
                currentChain->erase(currentChain->begin() + i);
                i--;
                currentChainSize--;
                edgeInvalid->invalid();

                addNewTriangle(edgeInvalid->getVertex1(), edgeInvalid->getVertex2(), intersectionVertex);
            }
            if(currentChain->size() == 0) {
                chains.erase(chains.begin() + j);
            }
        } else if (currentChainSize == 2) {

            Edge* firstEdge = (*currentChain)[0];
            Edge* lastEdge = (*currentChain)[1];

            if(!firstEdge->isValid() || !lastEdge->isValid()) {
                break;
            }

            Vertex* intersectionVertex = new Vertex(intersection.x, intersection.y, intersection.z);

            addNewTriangle(firstEdge->getVertex1(), firstEdge->getVertex2(), intersectionVertex);
            addNewTriangle(lastEdge->getVertex1(), lastEdge->getVertex2(), intersectionVertex);

            //find which vertex will be reassigned and reassigned it
            Vertex* vertex1 = firstEdge->getVertex1();
            Vertex* vertex2 = firstEdge->getVertex2();
            if ((vertex1 != lastEdge->getVertex1()) && (vertex1 != lastEdge->getVertex2())) {
                firstEdge->setVertex2(intersectionVertex);
                intersectionVertex->setEdge1(firstEdge);
            } else {
                firstEdge->setVertex1(intersectionVertex);
                intersectionVertex->setEdge2(firstEdge);
            }

            Vertex* lastEdgeVertex = lastEdge->getVertex1();
            if ((vertex1 != lastEdgeVertex) && (vertex2 != lastEdgeVertex)) {
                lastEdge->setVertex2(intersectionVertex);
                intersectionVertex->setEdge1(lastEdge);
            } else {
                lastEdge->setVertex1(intersectionVertex);
                intersectionVertex->setEdge2(lastEdge);
            }
        } else {
            //que faire ici ?
            // On doit faire ce triangle la. Si le gars est seul on va le split ensuite
            /*Edge* firstEdge = (*currentChain)[0];

            if(!firstEdge->isValid()) {
                break;
            }

            std::cerr << currentChainSize  << ": " << firstEdge->getVertex1()->getX() << ", " << firstEdge->getVertex1()->getY() << ", " << firstEdge->getVertex1()->getZ()
                      << " - " << firstEdge->getVertex2()->getX() << ", " << firstEdge->getVertex2()->getY() << ", " << firstEdge->getVertex2()->getZ() << std::endl;

            Vertex* intersectionVertex = new Vertex(intersection.x, intersection.y, intersection.z);

            addNewTriangle(firstEdge->getVertex1(), firstEdge->getVertex2(), intersectionVertex);
            firstEdge->invalid();*/
        }
    }

    //Finally, remove invalid edge from the active plan
    removeInvalidEdge(activePlan);
}

void Reconstruction3D::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3)
{

    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    triangles->push_back(triangleVertex1);
    triangles->push_back(triangleVertex2);
    triangles->push_back(triangleVertex3);

    /*std::cerr << "triangles: (" << (*triangleVertex1)[0] << ", " << (*triangleVertex1)[1] << ", " << (*triangleVertex1)[2] << ") - ("
                 << (*triangleVertex2)[0] << ", " << (*triangleVertex2)[1] << ", " << (*triangleVertex2)[2] << ") - ("
                    << (*triangleVertex3)[0] << ", " << (*triangleVertex3)[1] << ", " << (*triangleVertex3)[2] << ")" << std::endl;*/
}

