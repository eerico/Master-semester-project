#include "FloorPlanAndProfileExtractor.h"




FloorPlanAndProfileExtractor::FloorPlanAndProfileExtractor()
{

}

void FloorPlanAndProfileExtractor::extract(const OMMesh* inputMesh, Vertex*& floorPlan,
                                           Profile*& currentProfile, unsigned int& floorPlanSize)
{

    float dz= 0.1;
    int levels = 11;
    std::vector< std::vector<Vertex*> > vec(levels, std::vector<Vertex*>());

    OpenMesh::Vec3f p(0,0,0);
    OpenMesh::Vec3f n(0,0,0);

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
            minZ =startPoint[2];
            maxZ = endPoint[2];
        } else {
            minZ = endPoint[2];
            maxZ = startPoint[2];
        }

        OpenMesh::Vec3f d = (endPoint - startPoint).normalized();


        for (int i = 0; i < levels; i++) {
            if(maxZ <= actualZ && minZ >= actualZ){
                p[2]=actualZ;
                //find intersection point: //det o je ne sait pas quoi
                float t = - OpenMesh::dot(startPoint - p,n)/OpenMesh::dot(d,n);

                OpenMesh::Vec3f v = startPoint + t*d;

                if(std::abs(v[3] - actualZ) > 0.001){
                    std::cerr << "intersection found at "<< v[3] <<  " but Z we want is: " << actualZ << std::endl;
                }


                bool found = false;
                foreach (Vertex* vertex, vec[i]) {
                    if(vertex->getX() == v[0] && vertex->getY() == v[1] ){
                        vertex->addFace(face1);
                        vertex->addFace(face2);
                        found = true;
                        break;
                    }
                }

                if (!found){
                  Vertex* vertex   =  new Vertex(v[0], v[1]);
                  vertex->addFace(face1);
                  vertex->addFace(face2);
                  vec[i].push_back(vertex);
                }
            }

        }
    }


    //find all neighbour in a certain floor
    foreach (std::vector<Vertex*> level, vec) {
        foreach (Vertex* vertex, level) {
            int found = 0;
            foreach (Vertex* neighbour, level) {
                foreach (OMMesh::FaceHandle faceVertex, *vertex->getFaces()) {
                    foreach (OMMesh::FaceHandle faceNeighbour, *neighbour->getFaces()) {
                        if( faceVertex.idx() == faceNeighbour.idx() ){//check id if the face is the same it must be a neighbour
                            if(found ==0){
                                vertex->setNeighbor1(neighbour);
                            } else if (found == 1){
                                vertex->setNeighbor2(neighbour);
                            }
                            found++;
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
    foreach (std::vector<Vertex*> level, vec) {
        foreach (Vertex* vertex, level) {
            if (!(vertex == vertex->getNeighbor2()->getNeighbor1())){
                    Vertex* otherNeighbour = vertex->getNeighbor2()->getNeighbor1();
                    vertex->getNeighbor2()->setNeighbor1(vertex->getNeighbor2()->getNeighbor2());
                    vertex->getNeighbor2()->setNeighbor2(otherNeighbour);
        }
        }
    }

    //find vertex of v that have the same face normal delete it and replace with neigbour

    foreach (std::vector<Vertex*> level, vec) {
        foreach (Vertex* vertex, level) {
            bool same = true;
           OMMesh::Normal normal = inputMesh->normal(vertex->getFaces()->at(0));
           for (int i=0 ; i < vertex->getFaces()->size(); i++){
           if(inputMesh->normal(vertex->getFaces()->at(i)) != normal){//normal??????
                   same = false;
                   break;
               }
           }
           if(same){
               vertex->getNeighbor1()->setNeighbor2(vertex->getNeighbor2());
               vertex->getNeighbor2()->setNeighbor1(vertex->getNeighbor1());
               delete vertex;
           }
        }

    }
    //now the vetors are broken!! should stop to use them!!!


}
