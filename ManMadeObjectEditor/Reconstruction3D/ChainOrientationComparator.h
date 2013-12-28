#ifndef CHAINORIENTATIONCOMPARATOR_H
#define CHAINORIENTATIONCOMPARATOR_H
#include <vector>
#include "../Geometry/Edge.h"
#include "../Geometry/Vertex.h"
#include "../Utils.h"

/**
 * @brief The ChainOrientationComparator class
 * This class is used to sort the chains using their orientation
 */
class ChainOrientationComparator
{
public:
    bool operator()(std::vector< Edge* >* chain1, std::vector< Edge* >* chain2){
        Edge* firstEdgeChain1 = (*chain1)[0];
        Edge* firstEdgeChain2 = (*chain2)[0];

        Vertex* vertex1Chain1 = firstEdgeChain1->getVertex1();
        Vertex* vertex2Chain1 = firstEdgeChain1->getVertex2();

        Vertex* vertex1Chain2 = firstEdgeChain2->getVertex1();
        Vertex* vertex2Chain2 = firstEdgeChain2->getVertex2();

        float vector1X = vertex2Chain1->getX() - vertex1Chain1->getX();
        float vector1Y = vertex2Chain1->getY() - vertex1Chain1->getY();

        float vector2X = vertex2Chain2->getX() - vertex1Chain2->getX();
        float vector2Y = vertex2Chain2->getY() - vertex1Chain2->getY();


        float orientation1 = atan2(vector1Y, vector1X);
        float orientation2 = atan2(vector2Y, vector2X);

        return orientation2 < orientation1;
    }
};

#endif // CHAINORIENTATIONCOMPARATOR_H
