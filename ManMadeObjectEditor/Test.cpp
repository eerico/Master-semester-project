#include "Test.h"
#include <iostream>

using namespace std;

Test::Test()
{
}

void Test::Reconstruction3DTest()
{
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
    test.reconstruct();
    cout << (test.allActivePlan)->size() << endl;

    vector< Edge* >* plan = (*test.allActivePlan)[0];
    for(unsigned int i(0); i < plan->size(); ++i) {
        Edge* e = (*plan)[i];
        Vertex* v1 = e->getVertex1();
        Vertex* v2 = e->getVertex2();

        cout << "(" << v1->getX() << ", " << v1->getY() << ") - (" << v2->getX() << ", " << v2->getY() << ")    " << e << endl;
    }

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

    cout << "Intersection type: 0 No intersection, 1 General, 2 Edge dircetion" << endl;
    Reconstruction3D::Intersection i = test.intersect(ab, bc, cd, 0.0f);
    cout << "TMP intersection type: " << i.eventType << ", x = " << i.x << ", y = " << i.y << " , z = " << i.z << endl;

    test.computeIntersection();
    std::priority_queue<Reconstruction3D::Intersection, std::vector<Reconstruction3D::Intersection>, Reconstruction3D::IntersectionComparison>* Q = test.priorityQueue;

    while(!Q->empty()) {
        Reconstruction3D::Intersection ci = Q->top();
        Q->pop();
        cout << "intersection type: " << ci.eventType << ", x = " << ci.x << ", y = " << ci.y << " , z = " << ci.z << endl;
    }
}
