#include "FloorPlanAndProfileExtractor.h"




FloorPlanAndProfileExtractor::FloorPlanAndProfileExtractor()
{

}

void FloorPlanAndProfileExtractor::extract(const OMMesh* inputMesh, Vertex*& floorPlan,
                                           Profile*& currentProfile, unsigned int& floorPlanSize)
{

    float dz= 0.1f;
    int levels = 11;
    std::vector< std::vector<Vertex*> > plans(levels, std::vector<Vertex*>());

    OpenMesh::Vec3f p(0,0,0);
    OpenMesh::Vec3f n(0,0,1);

    for (OMMesh::EdgeIter e_it=inputMesh->edges_begin(); e_it!=inputMesh->edges_end(); ++e_it) {

        float actualZ = dz;

        //get the 2 halfedge of this edge:
        OMMesh::HalfedgeHandle halfedge1 = inputMesh->halfedge_handle( e_it, 0);
        OMMesh::HalfedgeHandle halfedge2 = inputMesh->halfedge_handle( e_it, 1);

        OMMesh::FaceHandle face1 = inputMesh->face_handle(halfedge1);
        OMMesh::FaceHandle face2 = inputMesh->face_handle(halfedge2);

        OpenMesh::Vec3f startPoint = inputMesh->point( inputMesh->from_vertex_handle( halfedge1 ) );
        OpenMesh::Vec3f endPoint = inputMesh->point( inputMesh->to_vertex_handle( halfedge1 ) );

        float minZ;
        float maxZ;
        if(startPoint[2] <= endPoint[2]){
            minZ = startPoint[2];
            maxZ = endPoint[2];
        } else {
            minZ = endPoint[2];
            maxZ = startPoint[2];
        }

        // ray - plan intersection, to find the intersection
        // notation: ray: o + t*d, plan equation: (x - p) dot (n) = 0
        OpenMesh::Vec3f d = (endPoint - startPoint).normalized();

        for (int i = 0; i < levels; i++) {
            actualZ = dz * i;
            //check that our edge is at the current height and is not parallel to our plan
            if((maxZ >= actualZ) && (minZ <= actualZ) && (actualZ != minZ || actualZ != maxZ)){
                p[2] = actualZ;
                //find intersection point: //det o je ne sait pas quoi
                float t = -OpenMesh::dot(startPoint - p,n)/OpenMesh::dot(d,n);

                OpenMesh::Vec3f intersectionPoint = startPoint + t*d;

                //if the intersection is ont on our edge, we go to the next iteration
                if ((intersectionPoint[2] > maxZ + 0.001f) || (intersectionPoint[2] < minZ - 0.001f)) { // how to choose this two variables (0.00f)?
                    continue;
                }

                if(std::abs(intersectionPoint[2] - actualZ) > 0.001f){
                    std::cerr << "intersection found at "<< intersectionPoint[2] <<  " but Z we want is: " << actualZ << std::endl;
                }


                bool found = false;
                foreach (Vertex* vertex, plans[i]) {
                    if(std::abs( vertex->getX() - intersectionPoint[0]) < 0.001f
                            && std::abs( vertex->getY() - intersectionPoint[1]) < 0.001f ){
                        vertex->addFace(face1);
                        vertex->addFace(face2);

                        found = true;
                        break;
                    }
                }

                if (!found){
                  Vertex* vertex   =  new Vertex(intersectionPoint[0], intersectionPoint[1], true);
                  vertex->addFace(face1);
                  vertex->addFace(face2);
                  plans[i].push_back(vertex);
                }

            }

        }
    }

    /*std::cerr << "&& ";
    std::vector<Vertex*> level2 = plans[1];
    for(unsigned int i(0); i < level2.size(); ++i) {
        Vertex* aaaa = level2[i];
        std::cerr << aaaa << ", ";
    }
    std::cerr << std::endl;*/

    //find all neighbour in a certain floor
    foreach (std::vector<Vertex*> level, plans) {
        foreach (Vertex* vertex, level) {
            int found = 0;
            foreach (Vertex* neighbour, level) {
                if(vertex == neighbour) {
                    continue;
                }

                foreach (OMMesh::FaceHandle faceVertex, *vertex->getFaces()) {
                    foreach (OMMesh::FaceHandle faceNeighbour, *neighbour->getFaces()) {
                        if( faceVertex.idx() == faceNeighbour.idx() ){//check id if the face is the same it must be a neighbour
                            if(found ==0){
                                vertex->setNeighbor1(neighbour);
                                found++;
                            } else if (found == 1){
                                if (vertex->getNeighbor1() != neighbour){
                                    vertex->setNeighbor2(neighbour);
                                    found++;
                                }
                                //vertex->setNeighbor2(neighbour);
                            }

                            if(found==2){
                                break;
                            }
                        }
                        if(found==2){
                            break;
                        }
                    }
                    if(found==2){
                        break;
                    }
                }
                if(found==2){
                    break;
                }
            }
        }
    }

    //build chain at every level
    foreach (std::vector<Vertex*> level, plans) {
        if (level.size() < 3) {
            continue;
        }
        foreach (Vertex* vertex, level) {
            if (!(vertex == vertex->getNeighbor2()->getNeighbor1())){
                    Vertex* otherNeighbour = vertex->getNeighbor2()->getNeighbor1();
                    //vertex->getNeighbor2()->getNeighbor2() == vertex ??
                    vertex->getNeighbor2()->setNeighbor1(vertex->getNeighbor2()->getNeighbor2());
                    vertex->getNeighbor2()->setNeighbor2(otherNeighbour);
            }
        }
    }

    //find vertex of v that have the same face normal delete it and replace with neigbour
    for(unsigned int j(0); j < plans.size(); ++j) {
        std::vector<Vertex*> level = plans[j];
    //foreach (std::vector<Vertex*> level, plans) {
        foreach (Vertex* vertex, level) {
            bool same = true;
            OMMesh::Normal normal = inputMesh->normal((*vertex->getFaces())[0]);
            for (unsigned int i=0 ; i < vertex->getFaces()->size(); i++){
                if(inputMesh->normal((*vertex->getFaces())[i]) != normal){//normal??????
                   same = false;
                   break;
                }
            }
            if(same){
                vertex->getNeighbor1()->setNeighbor2(vertex->getNeighbor2());
                vertex->getNeighbor2()->setNeighbor1(vertex->getNeighbor1());
                vertex->invalid();
                //delete vertex;
            }
        }

    }


    std::vector<Vertex*> level = plans[0];
    Profile* pp = new Profile(false);
    ProfileDestructorManager::putProfile(pp);
    for(unsigned int i(0); i < level.size(); ++i) {
        Vertex* current = level[i];
        if (current->isValid()) {
            Edge* e2 = new Edge(current, current->getNeighbor2(), pp);
            Edge* e1 = new Edge(current->getNeighbor1(), current, pp);
            current->setEdge2(e2);
            current->getNeighbor2()->setEdge1(e2);
            current->setEdge1(e1);
            current->getNeighbor1()->setEdge2(e1);
            floorPlanSize++;
            floorPlan = current;
        }
    }
    currentProfile = pp;

    /*for(unsigned int i(0); i < levels; ++i ) {
        std::vector<Vertex*> level = plans[i];
        std::cout << level.size() << std::endl;
    }*/


    std::cerr << "-------------------- " << floorPlanSize << std::endl;
    for(unsigned int i(0); i < floorPlanSize; ++i) {
        std::cerr << floorPlan->getNeighbor1() << ", " << floorPlan << ", " << floorPlan->getNeighbor2() << std::endl;
        floorPlan = floorPlan->getNeighbor2();
    }
}
