#include "Chains.h"

#define DEBUG

// Cette classe doit creer la chaines en faisant le truc des orientation ET implementer le inter et intra chain handling
// qui va update l active plan ET ceer des triangles (? ou pas ?)
Chains::Chains(Intersection* intersection, std::vector<qglviewer::Vec *> *triangles)
    :intersection(intersection), triangles(triangles)
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

}

void Chains::interChainHandling() {

}

void Chains::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    triangles->push_back(triangleVertex1);
    triangles->push_back(triangleVertex2);
    triangles->push_back(triangleVertex3);
}
