#include "Chains.h"

//#define DEBUG
//#define NOCHAIN

// Cette classe doit creer la chaines en faisant le truc des orientation ET implementer le inter et intra chain handling
// qui va update l active plan ET ceer des triangles (? ou pas ?)
Chains::Chains(Intersection* intersection, std::vector<qglviewer::Vec *> *triangles, ActivePlan* activePlan)
    :intersection(intersection), triangles(triangles), activePlan(activePlan)
{
    std::vector< Edge* >* edges = intersection->edgeVector;
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

        chainList.push_back(currentChain);
    }

    // sort the list of chain around the intersection
    ChainOrientationComparator chainOrientationComparator;
    std::sort(chainList.begin(), chainList.end(), chainOrientationComparator);


#ifdef DEBUG
    int chainIndex = 0;
    foreach(std::vector< Edge* >* v, chainList) {
        chainIndex++;
        std::cerr << "chain no: " << chainIndex << std::endl;
        foreach(Edge* e, *v) {
            Vertex* v1 = e->getVertex1();
            v1->setZ(intersection->z);
            Vertex* v2 = e->getVertex2();
            v2->setZ(intersection->z);
            Vertex* vv = new Vertex((v1->getX() + v2->getX()) / 2.0f + 0.002f, (v1->getY() + v2->getY()) / 2.0f + + 0.002f, v1->getZ());
            addNewTriangle(v1, v2, vv);
            std::cerr << *v1 << ", " << *v2 << std::endl;
        }
    }
#endif
}

void Chains::intraChainHandling() {

    unsigned int chainsSize = chainList.size();
    for(unsigned int j(0); j < chainsSize; ++j) {
        std::vector< Edge* >* currentChain = chainList[j];
        unsigned int currentChainSize = currentChain->size();

        if (currentChainSize > 2) {

            Edge* firstEdge = (*currentChain)[0];
            Edge* lastEdge = (*currentChain)[currentChainSize - 1];

            Edge* firstNeighbor = (*currentChain)[1];
            Edge* lastNeighbor = (*currentChain)[currentChainSize - 2];

            if(!firstEdge->isValid() || !lastEdge->isValid() || !firstNeighbor->isValid() || !lastNeighbor->isValid()) {
                break;
            }

            Vertex* intersectionVertex = new Vertex(intersection->x, intersection->y, intersection->z);

            // first check if the chain is circular. If it is the case, all edges are interior edges that will shrink to 0
            if (firstEdge->getVertex1() == lastEdge->getVertex2()) {
                //invalid all interior edges
                while(!currentChain->empty()){
                    Edge* edgeInvalid = (*currentChain)[currentChain->size()-1];
                    currentChain->pop_back();
                    edgeInvalid->invalid();

                    #ifndef DEBUG
                        addNewTriangle(edgeInvalid->getVertex1(), edgeInvalid->getVertex2(), intersectionVertex);
                    #endif
                }
                chainList.erase(chainList.begin() + j);
                continue;
            }

            #ifndef DEBUG
                addNewTriangle(firstEdge->getVertex1(), firstEdge->getVertex2(), intersectionVertex);
                addNewTriangle(lastEdge->getVertex1(), lastEdge->getVertex2(), intersectionVertex);
            #endif

            //reassign the vertices
            Vertex* vertex1 = firstEdge->getVertex1();
            if ((vertex1 != firstNeighbor->getVertex1()) && (vertex1 != firstNeighbor->getVertex2())) {
                firstEdge->setVertex2(intersectionVertex);
                vertex1->setNeighbor2(intersectionVertex);
                intersectionVertex->setEdge1(firstEdge);
                intersectionVertex->setNeighbor1(vertex1);
            } else {
                std::cerr << "Case 1 that should not happen in intraChainHandling" << std::endl;
            }

            //reassign the vertices
            Vertex* vertex2 = lastEdge->getVertex2();
            if ((vertex2 != lastNeighbor->getVertex1()) && (vertex2 != lastNeighbor->getVertex2())) {
                lastEdge->setVertex1(intersectionVertex);
                vertex2->setNeighbor1(intersectionVertex);
                intersectionVertex->setEdge2(lastEdge);
                intersectionVertex->setNeighbor2(vertex2);
            } else {
                std::cerr << "Case 2 that should not happen in intraChainHandling" << std::endl;
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
                chainList.erase(chainList.begin() + j);
            }
        } else if (currentChainSize == 2) {

            Edge* firstEdge = (*currentChain)[0];
            Edge* lastEdge = (*currentChain)[1];

            if(!firstEdge->isValid() || !lastEdge->isValid()) {
                break;
            }

            // first check if the chain is circular. If it is the case, all edges are interior edges that will shrink to 0
            if (firstEdge->getVertex1() == lastEdge->getVertex2()) {
                //invalid all interior edges
                firstEdge->invalid();
                lastEdge->invalid();
                chainList.erase(chainList.begin() + j);
                continue;
            }

            Vertex* intersectionVertex = new Vertex(intersection->x, intersection->y, intersection->z);

            addNewTriangle(firstEdge->getVertex1(), firstEdge->getVertex2(), intersectionVertex);
            addNewTriangle(lastEdge->getVertex1(), lastEdge->getVertex2(), intersectionVertex);

            //reassign the vertices
            Vertex* vertex1 = firstEdge->getVertex1();
            Vertex* vertex2 = lastEdge->getVertex2();
            Vertex* vertexToReplaceFrstEdge = firstEdge->getVertex2();
            Vertex* vertexToReplaceScndEdge = lastEdge->getVertex1();

            if(vertexToReplaceFrstEdge != vertexToReplaceScndEdge) {
                std::cerr << "Case 3 that should not happen in intraChainHandling" << std::endl;
            }

            vertex1->setNeighbor2(intersectionVertex);
            firstEdge->setVertex2(intersectionVertex);
            vertex2->setNeighbor1(intersectionVertex);
            lastEdge->setVertex1(intersectionVertex);
            intersectionVertex->setNeighbor1(vertex1);
            intersectionVertex->setNeighbor2(vertex2);
            intersectionVertex->setEdge1(firstEdge);
            intersectionVertex->setEdge2(lastEdge);

        }
    }
}

// return true si il y a eu un split lors de l'algorithme car
// si c est le cas, il faut recompute le active plan ET les intersection!
// (voir paper page 5, paragraphe Outline, la fin)
bool Chains::interChainHandling() {
    unsigned int chainsSize = chainList.size();
    if (chainsSize < 2) {
        return false;
    }

    std::vector< std::vector< Edge* >* > allChainAfterAlgorithm;

    bool edgeSplitted = false;
    for(unsigned int i(0); i < chainsSize; i++) {
        std::vector< Edge* >* chain1 = chainList[i];
        std::vector< Edge* >* chain2 = chainList[(i+1) % chainsSize];

        Vertex* intersectionVertex = new Vertex(intersection->x, intersection->y, intersection->z);

        Edge* lastEdgeChain1(0);
        Edge* firstEdgeChain2(0);

        if(chain1->size() == 1) {
            Edge* newEdge1(0);
            Edge* newEdge2(0);

            Edge* chainEdge = (*chain1)[0];
            splitEdgeAtCorner(chainEdge, newEdge1, newEdge2);

            chainEdge->invalid();
            #ifndef DEBUG
                addNewTriangle(chainEdge->getVertex1(), chainEdge->getVertex2(), newEdge1->getVertex2());
            #endif

            chain1->pop_back();
            chain1->push_back(newEdge1);
            chain1->push_back(newEdge2);
            lastEdgeChain1 = newEdge2;

            edgeSplitted = true;
        } else {
            lastEdgeChain1 = (*chain1)[1];
        }

        if(chain2->size() == 1) {
            Edge* newEdge1(0);
            Edge* newEdge2(0);

            Edge* chainEdge = (*chain2)[0];
            splitEdgeAtCorner(chainEdge, newEdge1, newEdge2);

            chainEdge->invalid();
            #ifndef DEBUG
                addNewTriangle(chainEdge->getVertex1(), chainEdge->getVertex2(), newEdge1->getVertex2());
            #endif

            chain2->pop_back();
            chain2->push_back(newEdge1);
            chain2->push_back(newEdge2);
            firstEdgeChain2 = newEdge1;

            edgeSplitted = true;
        } else {
            firstEdgeChain2 = (*chain2)[0];
        }

        // set the edge and vertex information
        lastEdgeChain1->setVertex1(intersectionVertex);
        firstEdgeChain2->setVertex2(intersectionVertex);
        lastEdgeChain1->getVertex2()->setNeighbor1(intersectionVertex);
        firstEdgeChain2->getVertex1()->setNeighbor2(intersectionVertex);

        intersectionVertex->setEdge1(firstEdgeChain2);
        intersectionVertex->setNeighbor1(firstEdgeChain2->getVertex1());
        intersectionVertex->setEdge2(lastEdgeChain1);
        intersectionVertex->setNeighbor2(lastEdgeChain1->getVertex2());


        std::vector< Edge* >* chainAfterAlgorithm = new std::vector< Edge* >;
        chainAfterAlgorithm->push_back(lastEdgeChain1);
        chainAfterAlgorithm->push_back(firstEdgeChain2);

        allChainAfterAlgorithm.push_back(chainAfterAlgorithm);
    }

    chainList = allChainAfterAlgorithm;

    return edgeSplitted;
}

void Chains::splitEdgeAtCorner(Edge *edgeToSplit, Edge*& newEdge1, Edge*& newEdge2)
{
    Vertex* cornerVertex = new Vertex(intersection->x, intersection->y, intersection->z);

    newEdge1 = new Edge(edgeToSplit->getVertex1(), cornerVertex, edgeToSplit->getProfile());
    newEdge2 = new Edge(cornerVertex, edgeToSplit->getVertex2(), edgeToSplit->getProfile());

    newEdge1->setDirectionPlan(edgeToSplit->getDirectionPlan());
    newEdge2->setDirectionPlan(edgeToSplit->getDirectionPlan());

    cornerVertex->setEdge1(newEdge1);
    cornerVertex->setEdge2(newEdge2);
    newEdge1->getVertex1()->setNeighbor2(cornerVertex);
    newEdge1->getVertex1()->setEdge2(newEdge1);
    newEdge2->getVertex2()->setNeighbor1(cornerVertex);
    newEdge2->getVertex2()->setEdge1(newEdge2);
    cornerVertex->setNeighbor1(newEdge1->getVertex1());
    cornerVertex->setNeighbor2(newEdge2->getVertex2());


    // we update the current active plan. We have to remove the old edge and insert the two new one
    activePlan->insert2Edges(edgeToSplit, newEdge1, newEdge2);
}

void Chains::getChains(std::vector< std::vector<std::vector<Edge *> > >* chainsAllLevel) {
    std::vector< std::vector< Edge* >* > ;
    std::vector<std::vector<Edge *> > chains;

    foreach(std::vector< Edge* >* currentChain, chainList) {
        std::vector< Edge* > edgeVector;
        foreach(Edge* e, *currentChain) {
            Vertex* vertex1 = e->getVertex1();
            Vertex* vertex2 = e->getVertex2();

            Edge* copy = new Edge(new Vertex(vertex1->getX(), vertex1->getY()), new Vertex(vertex2->getX(), vertex2->getY()));
            edgeVector.push_back(copy);
        }
        chains.push_back(edgeVector);
    }
    chainsAllLevel->push_back(chains);
}

void Chains::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
#ifndef NOCHAIN
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    triangles->push_back(triangleVertex1);
    triangles->push_back(triangleVertex2);
    triangles->push_back(triangleVertex3);
#endif
}
