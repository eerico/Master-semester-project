#include "Chain.h"

Chain::Chain(Vertex* floorPlan, unsigned int floorPlanSize)
{
    Vertex* currentVertex = floorPlan;
    //std::vector< std::vector< Edge* >* > chains;
    std::vector< Edge* >* chain = new std::vector< Edge* >;

    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = currentVertex->getEdge2();
        Edge* clone = new Edge(currentEdge->getVertex1(), currentEdge->getVertex2(), currentEdge->getProfile());
        chain->push_back(clone);
        currentVertex = currentVertex->getNeighbor2();
    }
    chains = new std::vector< std::vector< Edge* >* >;
    chains->push_back(chain);
}

Chain::Chain(float height, Chain* previousChains, std::vector< qglviewer::Vec * >* triangles)
    :previousChains(previousChains), triangles(triangles)
{
    chains = new std::vector< std::vector< Edge* >* >;
    std::vector< Edge* >* currentSubChain;

    std::vector< std::vector< Edge* >* >* previousChainsVector = previousChains->getChains();
    unsigned int numberSubChains = previousChainsVector->size();

    Plan horizontalPlan(0.0f, 0.0f, height, 0.0f, 0.0f, 1.0f);

    for(unsigned int subChainIndex(0); subChainIndex < numberSubChains; ++subChainIndex) {
        currentSubChain = new std::vector< Edge* >;

        std::vector< Edge* >* edges = (*previousChainsVector)[subChainIndex];
        unsigned int numberEdge = edges->size();
        Vertex* firstNewVertex;
        Vertex* previousNewVertex;
        Profile* previousProfile;
        Vertex* newVertex;
        Edge* newEdge;
        for(unsigned int edgeIndex(0); edgeIndex < numberEdge; ++edgeIndex) {

            Edge* currentEdge = (*edges)[edgeIndex];

            Vertex* vertex1 = currentEdge->getVertex1();
            Vertex* vertex2 = currentEdge->getVertex2();

            Edge* previousEdge = vertex1->getEdge1();
            Vertex* vertex0 = previousEdge->getVertex1();

            Plan plan1(vertex0->getX(), vertex0->getY(), vertex0->getZ());
            plan1.computePlanNormal(vertex0, vertex1, previousEdge->getProfile());

            Plan plan2(vertex1->getX(), vertex1->getY(), vertex1->getZ());
            plan2.computePlanNormal(vertex1, vertex2, currentEdge->getProfile());

            Intersection newIntersection = horizontalPlan.intersect3Plans(plan1, plan2);
            newVertex = new Vertex(newIntersection.x, newIntersection.y, newIntersection.z);

            if (edgeIndex > 0) {
                newEdge = new Edge(previousNewVertex, newVertex, previousProfile);
                previousNewVertex->setEdge2(newEdge);
                previousNewVertex->setNeighbor2(newVertex);
                newVertex->setEdge1(newEdge);
                newVertex->setNeighbor1(previousNewVertex);

                currentSubChain->push_back(newEdge);
            } else {
                firstNewVertex = newVertex;
            }

            previousNewVertex = newVertex;
            previousProfile = currentEdge->getProfile();
        }

        newEdge = new Edge(previousNewVertex, firstNewVertex, previousProfile);
        previousNewVertex->setEdge2(newEdge);
        previousNewVertex->setNeighbor2(firstNewVertex);
        firstNewVertex->setEdge1(newEdge);
        firstNewVertex->setNeighbor1(previousNewVertex);
        currentSubChain->push_back(newEdge);

        chains->push_back(currentSubChain);
    }
}


void Chain::interChainHandling()
{
    // le code est faux mais idée est juste. 1 edge se split en 2 et on reconstruit en dessus mais comment update dans
    // la priority queue pour chaque intersection ?
    //



    // on suppose que les chaines sont bien successive...
    /*unsigned int chainsSize = chains.size();
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
            splitEdgeAtCorner(chainEdge, newEdge1, newEdge2);

            chainEdge->invalid();
            addNewTriangle(chainEdge->getVertex1(), chainEdge->getVertex2(), newEdge1->getVertex2());

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
            splitEdgeAtCorner(chainEdge, newEdge1, newEdge2);

            chainEdge->invalid();
            addNewTriangle(chainEdge->getVertex1(), chainEdge->getVertex2(), newEdge1->getVertex2());

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
    }*/
}

void Chain::intraChainHandling()
{

    /*unsigned int chainsSize = chains.size();
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
        /*}
    }*/
}

void Chain::splitEdgeAtCorner(Edge *edgeToSplit, Edge*& newEdge1, Edge*& newEdge2)
{
    /*Vertex* cornerVertex = new Vertex(intersection.x, intersection.y, intersection.z);

    newEdge1 = new Edge(edgeToSplit->getVertex1(), cornerVertex, edgeToSplit->getProfile());
    newEdge2 = new Edge(cornerVertex, edgeToSplit->getVertex2(), edgeToSplit->getProfile());

    cornerVertex->setEdge1(newEdge1);
    cornerVertex->setEdge2(newEdge2);

    //remove invalid edge from the active plan and add the two new edges
    unsigned int activePlanSize = activePlan->size();
    bool found(false);
    for(unsigned int i(0); (i < activePlanSize) && !found; ++i) {
        Edge* currentEdge = (*activePlan)[i];
        if(currentEdge == edgeToSplit) {
            activePlan->erase(activePlan->begin() + i);
            //delete currentEdge;

            currentEdge->setChild1(newEdge1);
            currentEdge->setChild2(newEdge2);

            activePlan->insert(activePlan->begin() + i, newEdge1);
            activePlan->insert(activePlan->begin() + i + 1, newEdge2);
            found = true;
        }
    }*/
}

std::vector<std::vector<Edge *> *>* Chain::getChains() {
    return chains;
}

void Chain::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    triangles->push_back(triangleVertex1);
    triangles->push_back(triangleVertex2);
    triangles->push_back(triangleVertex3);
}

void Chain::printChain() {
    std::cerr << "Chain: " << std::endl;
    unsigned int i = 1;
    foreach(std::vector< Edge* >* vector, *chains) {
        std::cerr << "chain " << i << std::endl;
        foreach(Edge* edge, *vector) {
            std::cerr << *edge << "::";
        }
        i++;
        std::cerr << std::endl;
    }
}
