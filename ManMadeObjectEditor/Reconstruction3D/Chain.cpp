#include "Chain.h"

//#define DEBUG

Chain::Chain(Vertex* floorPlan, unsigned int floorPlanSize)
    : chainHasChanged(false)
{
    Vertex* currentVertex = floorPlan;
    //std::vector< std::vector< Edge* >* > chains;
    std::vector< Edge* >* chain = new std::vector< Edge* >;

    Vertex* firstCloneVertex;
    Vertex* previousVertex;
    Edge* previousEdge;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = currentVertex->getEdge2();

        Edge* cloneEdge = new Edge(currentEdge->getVertex1(), currentEdge->getVertex2(), currentEdge->getProfile());
        Vertex* cloneVertex1 = new Vertex(currentVertex->getX(), currentVertex->getY(), currentVertex->getZ());

        cloneVertex1->setEdge2(cloneEdge);
        cloneEdge->setVertex1(cloneVertex1);

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

        chain->push_back(cloneEdge);
        currentVertex = currentVertex->getNeighbor2();
    }

    firstCloneVertex->setEdge1(previousEdge);
    firstCloneVertex->setNeighbor1(previousVertex);
    previousVertex->setNeighbor2(firstCloneVertex);
    previousEdge->setVertex2(firstCloneVertex);


    // check if two edge are parallel
    // if there are, we remove the connection between them
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = (*chain)[i];
        Edge* nextEdge = (*chain)[(i+1) % floorPlanSize];

        if (currentEdge->isParallel(nextEdge)) {
            //Vertex* vertexToRemove = currentEdge->getVertex2();
            Vertex* vertex1 = currentEdge->getVertex1();
            Vertex* vertex2 = nextEdge->getVertex2();

            vertex1->setNeighbor2(vertex2);
            vertex2->setNeighbor1(vertex1);

            nextEdge->setVertex1(vertex1);
            vertex1->setEdge2(nextEdge);

            chain->erase(chain->begin() + i);
            floorPlanSize--;
            i--;
        }
    }

    chains = new std::vector< std::vector< Edge* >* >;
    chains->push_back(chain);
}

Chain::Chain(float height, Chain* previousChains, std::vector< qglviewer::Vec * >* triangles)
    :previousChains(previousChains), triangles(triangles), chainHasChanged(false)
{
    // Peu etre que sa marche pas correctement de comment on creer les sub chaine, a verifier !!

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
        Edge* firstPreviousEdge;

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

                previousEdge->setChild1(newEdge);
                newEdge->setParent(previousEdge);

                #ifdef DEBUG
                    addNewTriangle(newEdge->getVertex1(), newEdge->getVertex2(), new Vertex(newEdge->getVertex1()->getX() + 0.05f, newEdge->getVertex1()->getY() + 0.05f, newEdge->getVertex1()->getZ() + 0.01f));
                #endif


                previousNewVertex->setEdge2(newEdge);
                previousNewVertex->setNeighbor2(newVertex);
                newVertex->setEdge1(newEdge);
                newVertex->setNeighbor1(previousNewVertex);


                currentSubChain->push_back(newEdge);
            } else {
                firstNewVertex = newVertex;
                firstPreviousEdge = previousEdge;
            }

            previousNewVertex = newVertex;
            previousProfile = currentEdge->getProfile();
        }

        newEdge = new Edge(previousNewVertex, firstNewVertex, previousProfile);

        #ifdef DEBUG
            addNewTriangle(newEdge->getVertex1(), newEdge->getVertex2(), new Vertex(newEdge->getVertex1()->getX() + 0.05f, newEdge->getVertex1()->getY() + 0.05f, newEdge->getVertex1()->getZ() + 0.01f));
        #endif

        firstPreviousEdge->setChild1(newEdge);
        newEdge->setParent(firstPreviousEdge);

        previousNewVertex->setEdge2(newEdge);
        previousNewVertex->setNeighbor2(firstNewVertex);
        firstNewVertex->setEdge1(newEdge);
        firstNewVertex->setNeighbor1(previousNewVertex);
        currentSubChain->push_back(newEdge);

        chains->push_back(currentSubChain);
    }
}

// test if there is some edge collapsing
void Chain::intraChainHandling()
{
    unsigned int numberSubChains = chains->size();
    for(unsigned int subChainIndex(0); subChainIndex < numberSubChains; ++subChainIndex) {
        std::vector< Edge* >* edges = (*chains)[subChainIndex];
        unsigned int numberEdges = edges->size();

        for(unsigned int edgeIndex(0); edgeIndex < numberEdges; ++edgeIndex) {
            Edge* currentEdge = (*edges)[edgeIndex];
            if(currentEdge->getVertex1()->distance(currentEdge->getVertex2()) < 0.001f) {
                edges->erase(edges->begin() + edgeIndex);
                edgeIndex--;
                numberEdges--;

                #ifdef DEBUG
                    addNewTriangle(new Vertex(currentEdge->getVertex1()->getX() - 0.05f, currentEdge->getVertex1()->getY() + 0.05f, currentEdge->getVertex1()->getZ() + 0.01f),
                                   new Vertex(currentEdge->getVertex1()->getX() + 0.05f, currentEdge->getVertex1()->getY() + 0.05f, currentEdge->getVertex1()->getZ() + 0.01f),
                                   new Vertex(currentEdge->getVertex1()->getX() + 0.05f, currentEdge->getVertex1()->getY() - 0.05f, currentEdge->getVertex1()->getZ() + 0.01f));
                #endif

                chainHasChanged = true;

                Vertex* vertex1 = currentEdge->getVertex1();
                Vertex* vertex2 = currentEdge->getVertex2();
                Vertex* newCorner = new Vertex(vertex1->getX(), vertex1->getY(), vertex1->getZ());

                vertex1->getNeighbor1()->setNeighbor2(newCorner);
                vertex2->getNeighbor2()->setNeighbor1(newCorner);
                vertex1->getEdge1()->setVertex2(newCorner);
                vertex2->getEdge2()->setVertex1(newCorner);

                newCorner->setEdge1(vertex1->getEdge1());
                newCorner->setEdge2(vertex2->getEdge2());
                newCorner->setNeighbor1(vertex1->getNeighbor1());
                newCorner->setNeighbor2(vertex2->getNeighbor2());

            }
        }

        // if the current chain collapse to a point, then this chain disapear, thus edges->size == 0
        // if there is only two edge or one, they close the object with an edge
        if(edges->size() < 3) {
            chains->erase(chains->begin() + subChainIndex);
            subChainIndex--;
            numberSubChains--;
            chainHasChanged = true;
        } else {
            //check if the remaining edges are all on a same line and thus close the chain
            Edge* edge = (*edges)[0];
            numberEdges = edges->size();
            bool allParallel = true;
            for(unsigned int i(1); (i < numberEdges) && allParallel; ++i) {
                Edge* comparedEdge = (*edges)[i];
                if(!edge->isParallel(comparedEdge)) {
                    allParallel = false;
                }
            }
            //if all edge inside a chain are parallel, they closed the chain
            if(allParallel) {
                chains->erase(chains->begin() + subChainIndex);
                subChainIndex--;
                numberSubChains--;
                chainHasChanged = true;
            }
        }
    }
}

// test if there is some edge splitting
void Chain::interChainHandling()
{
    unsigned int numberSubChains = chains->size();
    for(unsigned int subChainIndex(0); subChainIndex < numberSubChains; ++subChainIndex) {
        std::vector< Edge* >* edges = (*chains)[subChainIndex];
        unsigned int numberEdges = edges->size();

        bool newChainCreated = false;

        for(unsigned int edgeIndex(0); (edgeIndex < numberEdges) && !newChainCreated; ++edgeIndex) {
            Edge* currentEdge = (*edges)[edgeIndex];
            Vertex* currentVertex = currentEdge->getVertex1();
            Edge* previousEdge = currentVertex->getEdge1();


            for(unsigned int edgeIndex2(0); (edgeIndex2 < numberEdges) && !newChainCreated; ++edgeIndex2) {
                Edge* edge = (*edges)[edgeIndex2];
                if(edge == currentEdge || edge == previousEdge) {
                    continue;
                }

                float distance = edge->distance(currentVertex);

                if(distance < 0.001f) {
                    //printChain();
                    // then the current chain contain at least 2 chain that must be splitted
                    // first remove the current chain
                    chains->erase(chains->begin() + subChainIndex);
                    // then create the two new chains
                    createTwoChain(edge, currentVertex);
                    newChainCreated = true;
                    subChainIndex--;
                    numberSubChains++;
                    chainHasChanged = true;
                }
            }
        }
    }
}

void Chain::createTwoChain(Edge* edgeToSplit, Vertex* corner) {

    Vertex* newCorner1 = new Vertex(corner->getX(), corner->getY(), corner->getZ());
    Vertex* newCorner2 = new Vertex(corner->getX(), corner->getY(), corner->getZ());

    Edge* newEdge1 = new Edge(edgeToSplit->getVertex1(), newCorner1, edgeToSplit->getProfile());
    edgeToSplit->getVertex1()->setEdge2(newEdge1);
    edgeToSplit->getVertex1()->setNeighbor2(newCorner1);

    corner->getNeighbor2()->setNeighbor1(newCorner1);
    corner->getEdge2()->setVertex1(newCorner1);

    newCorner1->setEdge1(newEdge1);
    newCorner1->setEdge2(corner->getEdge2());
    newCorner1->setNeighbor1(edgeToSplit->getVertex1());
    newCorner1->setNeighbor2(corner->getNeighbor2());


    Edge* newEdge2 = new Edge(newCorner2, edgeToSplit->getVertex2(), edgeToSplit->getProfile());
    edgeToSplit->getVertex2()->setEdge1(newEdge2);
    edgeToSplit->getVertex2()->setNeighbor1(newCorner2);

    corner->getNeighbor1()->setNeighbor2(newCorner2);
    corner->getEdge1()->setVertex2(newCorner2);

    newCorner2->setEdge2(newEdge2);
    newCorner2->setEdge1(corner->getEdge1());
    newCorner2->setNeighbor1(corner->getNeighbor1());
    newCorner2->setNeighbor2(edgeToSplit->getVertex2());

    Edge* parent = edgeToSplit->getParent();
    parent->setChild1(newEdge1);
    parent->setChild2(newEdge2);
    newEdge1->setParent(parent);
    newEdge2->setParent(parent);

    //Now create the two sub chains
    std::vector< Edge* >* subChains1 = new std::vector< Edge* >;

    Vertex* vertexIterator = newCorner1;
    do {
        subChains1->push_back(vertexIterator->getEdge2());
        vertexIterator = vertexIterator->getNeighbor2();
    } while(vertexIterator != newCorner1);

    std::vector< Edge* >* subChains2 = new std::vector< Edge* >;
    vertexIterator = newCorner2;
    do {
        subChains2->push_back(vertexIterator->getEdge2());
        vertexIterator = vertexIterator->getNeighbor2();
    } while(vertexIterator != newCorner2);

    chains->push_back(subChains1);
    chains->push_back(subChains2);
}

std::vector<std::vector<Edge *> *>* Chain::getChains() {
    return chains;
}

bool Chain::isEmpty() {
    return chains->size() == 0;
}

bool Chain::hasChanged() {
    return chainHasChanged;
}

void Chain::computeTriangle() {
    std::vector< std::vector< Edge* >* >* chainToDraw = previousChains->getChains();
    unsigned int numberSubChains = chainToDraw->size();
    for(unsigned int subChainIndex(0); subChainIndex < numberSubChains; ++subChainIndex) {
        std::vector< Edge* >* edges = (*chainToDraw)[subChainIndex];
        unsigned int numberEdges = edges->size();

        bool newChainCreated = false;

        for(unsigned int edgeIndex(0); (edgeIndex < numberEdges) && !newChainCreated; ++edgeIndex) {
            Edge* currentEdge = (*edges)[edgeIndex];
            Edge* child1 = currentEdge->getChild1();
            Edge* child2 = currentEdge->getChild2();
            if(child1->getVertex1()->distance(child1->getVertex2()) < 0.001f) {
                #ifndef DEBUG
                    addNewTriangle(currentEdge->getVertex1(), currentEdge->getVertex2(), child1->getVertex2());
                #endif
            } else if (child2 != 0) {
                #ifndef DEBUG
                    addNewTriangle(currentEdge->getVertex1(), child1->getVertex2(), child1->getVertex1());
                    addNewTriangle(currentEdge->getVertex2(), child2->getVertex2(), child2->getVertex1());
                    addNewTriangle(currentEdge->getVertex1(), currentEdge->getVertex2(), child1->getVertex2());
                #endif
            } else {
                #ifndef DEBUG
                    addNewTriangle(currentEdge->getVertex1(), currentEdge->getVertex2(), child1->getVertex1());
                    addNewTriangle(child1->getVertex1(), currentEdge->getVertex2(), child1->getVertex2());
                #endif
            }
        }
    }
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
            std::cerr << *edge->getVertex1() << "\n";
        }
        i++;
        std::cerr << std::endl;
    }
}
