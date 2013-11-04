#include "Test.h"
#include <iostream>

using namespace std;

Test::Test()
{
}

void Test::Reconstruction3DTest()
{
    //must make all elements of reconsutrcion3D public !!
    //set a simple model
    Vertex* a = new Vertex(-1.0f, 1.0f);
    Vertex* b = new Vertex(1.0f, 1.0f);
    Vertex* c = new Vertex(1.0f, -1.0f);
    Vertex* d = new Vertex(-1.0f, -1.0f);

    a->setNeighbor1(d);
    a->setNeighbor2(b);
    b->setNeighbor1(a);
    b->setNeighbor2(c);
    c->setNeighbor1(b);
    c->setNeighbor2(d);
    d->setNeighbor1(c);
    d->setNeighbor2(a);

    Profile* p = new Profile(false);
    Edge* ab = new Edge(a, b, p);
    Edge* bc = new Edge(b, c, p);
    Edge* cd = new Edge(c, d, p);
    Edge* da = new Edge(d, a, p);

    a->setEdge1(da);
    a->setEdge2(ab);
    b->setEdge1(ab);
    b->setEdge2(bc);
    c->setEdge1(bc);
    c->setEdge2(cd);
    d->setEdge1(cd);
    d->setEdge2(da);

    int size = 4;

    Reconstruction3D test = Reconstruction3D(a, size, NULL);






    //test copy, commentermain loop et reset to initial state


    test.reconstruct();
    cout << (test.allActivePlan)->size() << endl;

    vector< Edge* >* plan = (*test.allActivePlan)[0];
    for(unsigned int i(0); i < plan->size(); ++i) {
        Edge* e = (*plan)[i];
        Vertex* v1 = e->getVertex1();
        Vertex* v2 = e->getVertex2();

        cout << "(" << v1->getX() << ", " << v1->getY() << ") - (" << v2->getX() << ", " << v2->getY() << ")    " << e << endl;
    }





    //test normal

    cout << "plan normal: " << endl;
    float nx, ny, nz;
    test.computePlanNormal(ab->getVertex1(), ab->getVertex2(), ab->getProfile(), nx, ny, nz);
    cout << "normal: " << nx << ", " << ny << ", " << nz << endl;
    test.computePlanNormal(bc->getVertex1(), bc->getVertex2(), bc->getProfile(), nx, ny, nz);
    cout << "normal: " << nx << ", " << ny << ", " << nz << endl;
    test.computePlanNormal(cd->getVertex1(), cd->getVertex2(), cd->getProfile(), nx, ny, nz);
    cout << "normal: " << nx << ", " << ny << ", " << nz << endl;
    test.computePlanNormal(da->getVertex1(), da->getVertex2(), da->getProfile(), nx, ny, nz);
    cout << "normal: " << nx << ", " << ny << ", " << nz << endl;





    //test simple intersection

    cout << "Intersection type: 0 No intersection, 1 General, 2 Edge dircetion" << endl;
    Reconstruction3D::Intersection i = test.intersect(ab, bc, cd, 0.0f);
    cout << "TMP intersection type: " << i.eventType << ", x = " << i.x << ", y = " << i.y << " , z = " << i.z << endl;





    //test all intersection

    std::cout << "compute intersection" << std::endl;
    //test.addEdgeDirectionEvent(); //remetre profile a 2 sample!!
    test.computeIntersection();
    std::priority_queue<Reconstruction3D::Intersection, std::vector<Reconstruction3D::Intersection>, Reconstruction3D::IntersectionComparison>* Q = test.priorityQueue;

    std::cout << "nombre d intersection : " << Q->size() << std::endl;

    /*while(!Q->empty()) {
        Reconstruction3D::Intersection ci = Q->top();
        Q->pop();
        cout << "intersection type: " << ci.eventType << ", x = " << ci.x << ", y = " << ci.y << " , z = " << ci.z << endl;

        std::vector< Edge* >* ee = ci.edgeVector;
        for(int i= 0; i < ee->size(); ++i) {
            std::cout << (*ee)[i] << std::endl;
        }
        std::cout<< std::endl;
    }*/





    //test remove invalid, must take an intersection of type edgeDirection

    /*Reconstruction3D::Intersection ci = Q->top();
    Q->pop();
    cout << "intersection type: " << ci.eventType << ", x = " << ci.x << ", y = " << ci.y << " , z = " << ci.z << endl;
    std::priority_queue<Reconstruction3D::Intersection, std::vector<Reconstruction3D::Intersection>, Reconstruction3D::IntersectionComparison>* Q2 = new std::priority_queue<Reconstruction3D::Intersection, std::vector<Reconstruction3D::Intersection>, Reconstruction3D::IntersectionComparison>;

    //show the intersections
    while(!Q->empty()) {
        Reconstruction3D::Intersection ci = Q->top();
        Q->pop();
        Q2->push(ci);
    }
    while(!Q2->empty()) {
        Reconstruction3D::Intersection ci = Q2->top();
        Q2->pop();
        cout << "intersection type: " << ci.eventType << ", x = " << ci.x << ", y = " << ci.y << " , z = " << ci.z << endl;
        Q->push(ci);
    }
    std::cout << "before remove invalid " << Q->size()<< std::endl;

    test.removeInvalidIntersection((*ci.edgeVector)[0], ci.y);
    //must retake it each time we call removeInvalidIntersection
    Q = test.priorityQueue;
    //show result
    while(!Q->empty()) {
        Reconstruction3D::Intersection ci = Q->top();
        Q->pop();
        Q2->push(ci);
    }
    while(!Q2->empty()) {
        Reconstruction3D::Intersection ci = Q2->top();
        Q2->pop();
        cout << "intersection type: " << ci.eventType << ", x = " << ci.x << ", y = " << ci.y << " , z = " << ci.z << endl;
        Q->push(ci);
    }
    //just to be sure, to it one more time
    ci = Q->top();
    Q->pop();
    test.removeInvalidIntersection((*ci.edgeVector)[0], ci.y);

    Q = test.priorityQueue;
    std::cout << "after remove invalid " << Q->size()<< std::endl;
    while(!Q->empty()) {
        Reconstruction3D::Intersection ci = Q->top();
        Q->pop();
        cout << "intersection type: " << ci.eventType << ", x = " << ci.x << ", y = " << ci.y << " , z = " << ci.z << endl;
    }
    std::cout<< std::endl;*/







    // test clustering
    /*cout << "clustering event" << endl;
    Reconstruction3D::Intersection in = Q->top();
    Q->pop();

    cout << in.edgeVector->size() << " = edge in intersection, Q size = " << Q->size() << endl;
    test.eventClustering(in);
    cout << "after clustering" << endl;
    cout << in.edgeVector->size() << " = edge in intersection, Q size = " << Q->size()<< endl;
    std::vector< std::vector< Edge* >* > chains;
    test.chainConstruction(in, chains);
    for(int i(0); i < chains.size(); ++i) {
        std::vector< Edge* >* c = chains[i];
        cout << "new chain" << endl;
        for(int j(0); j < c->size(); ++j) {
            Edge* e = (*c)[j];
            Vertex* v1 = e->getVertex1();
            Vertex* v2 = e->getVertex2();
            cout << "(" << v1->getX() << ", " << v1->getY() << ") - (" << v2->getX() << ", " << v2->getY() << ")" << endl;
        }
    }*/

}
