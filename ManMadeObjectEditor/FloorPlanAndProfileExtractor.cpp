#include "FloorPlanAndProfileExtractor.h"




FloorPlanAndProfileExtractor::FloorPlanAndProfileExtractor()
    :dy(0.1f), baseY(0.0f), levels(11)
{


}

void FloorPlanAndProfileExtractor::recenter(std::vector< std::vector< Vertex* > >& plans)
{
    //move the mesh such that (0, 0) is at the center of the first floor plan
    std::vector< Vertex* > firstLevel = plans[0];
    float centerX(0.0f);
    float centerY(0.0f);

    unsigned int firstLevelSize = firstLevel.size();

    for(unsigned int i(0); i < firstLevelSize; ++i)
    {
        Vertex* currentVertex = firstLevel[i];
        centerX += currentVertex->getX();
        centerY += currentVertex->getY();
    }

    centerX /= firstLevelSize;
    centerY /= firstLevelSize;

    //Now move each vertices
    for(unsigned int i(0); i < plans.size(); ++i) {
        std::vector< Vertex* > currentLevel = plans[i];
        for(unsigned int j(0); j < currentLevel.size(); ++j) {
            Vertex* currentVertex = currentLevel[j];
            currentVertex->setX(currentVertex->getX() - centerX);
            currentVertex->setY(currentVertex->getY() - centerY);
        }
    }
}

void FloorPlanAndProfileExtractor::rescale(std::vector< std::vector< Vertex* > >& plans)
{
    //scale the mesh such that the first floor plan is between [-1, 1]x[-1, 1]
    std::vector< Vertex* > firstLevel = plans[0];
    float minX(FLT_MAX);
    float minY(FLT_MAX);

    float maxX(FLT_MIN);
    float maxY(FLT_MIN);

    unsigned int firstLevelSize = firstLevel.size();

    for(unsigned int i(0); i < firstLevelSize; ++i)
    {
        Vertex* currentVertex = firstLevel[i];
        minX = std::min(currentVertex->getX(), minX);
        minY = std::min(currentVertex->getY(), minY);

        maxX = std::max(currentVertex->getX(), maxX);
        maxY = std::max(currentVertex->getY(), maxY);
    }

    float rescalFactor = std::max(std::max(std::max(maxX, maxY), -minX), -minY);

    //Now scale the mesh
    for(unsigned int i(0); i < plans.size(); ++i) {
        std::vector< Vertex* > currentLevel = plans[i];
        for(unsigned int j(0); j < currentLevel.size(); ++j) {
            Vertex* currentVertex = currentLevel[j];
            currentVertex->setX(currentVertex->getX() / rescalFactor);
            currentVertex->setY(currentVertex->getY() / rescalFactor);
        }
    }

    dy = dy / rescalFactor;
}

void FloorPlanAndProfileExtractor::profileConstruction(const OMMesh* inputMesh, std::vector<std::vector<Vertex *> > &plans)
{
    //For every two vertices, find the faces normals they have in common,
    // construct the edge between them and associate a normal to this edge
    for(unsigned int i(0); i < plans.size(); ++i){
        std::vector<Vertex*> level = plans[i];
        unsigned int numberertexAtLevel = level.size();
        if (numberertexAtLevel > 1) {
            for(unsigned int j(0); j < numberertexAtLevel; ++j) {
                Vertex* current = level[j];
                if (current->isValid()) {
                    Vertex* neighbor = current->getNeighbor2();
                    std::vector< OMMesh::FaceHandle>* currentFaces = current->getFaces();
                    std::vector< OMMesh::FaceHandle>* neighborFaces = neighbor->getFaces();
                    OMMesh::Normal* normal = new OMMesh::Normal(0.0f, 0.0f, 0.0f);

                    for(unsigned int k(0); k < currentFaces->size(); ++k) {
                        OMMesh::FaceHandle currentFace = (*currentFaces)[k];
                        for(unsigned int m(0); m < neighborFaces->size(); ++m) {

                            OMMesh::FaceHandle neighborFace = (*neighborFaces)[m];
                            if (currentFace.idx() == neighborFace.idx()) {
                                (*normal) = (*normal) + inputMesh->normal(currentFace);

                            }
                        }
                    }
                    normal->normalize();

                    Edge* edge = new Edge(current, current->getNeighbor2());
                    edge->setNormal(normal);
                    current->setEdge2(edge);
                    neighbor->setEdge1(edge);
                } else {
                    std::cerr << "Vertex Invalid at level " << i << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }


    //Construct the profile
    std::vector<Vertex*> firstLevel = plans[0];
    for(unsigned int i(0); i < firstLevel.size(); ++i) {
        Edge* currentFirstFloorEdge = firstLevel[i]->getEdge2();
        OMMesh::Normal* currentFirstFloorEdgeNormal = currentFirstFloorEdge->getNormal();

        Profile* currentProfile = new Profile(true);
        currentFirstFloorEdge->setProfile(currentProfile);
        ProfileDestructorManager::putProfile(currentProfile);
        for(unsigned int j(1); j < plans.size(); ++j){
            std::vector<Vertex*> level = plans[j];

            unsigned int numberertexAtLevel = level.size();
            if (numberertexAtLevel == 1) {
                /*Vertex* aloneVertex = level[0];
                std::vector< OMMesh::FaceHandle>* faces = aloneVertex->getFaces();
                OMMesh::Normal aloneVertexNormal(0.0f, 0.0f, 0.0f);
                foreach(OMMesh::FaceHandle face, (*faces)) {
                    aloneVertexNormal += inputMesh->normal(face);
                }
                aloneVertexNormal.normalize();

                //check dot product between normals
                float dotProduct = (*currentFirstFloorEdgeNormal) | aloneVertexNormal;

                //if the dot product is negativ, then the two normals doesnt face toward the same direction
                if (dotProduct >= 0.0f) {
                    currentProfile->addVertexEnd(new Vertex(currentFirstFloorEdge->distance(level[0]), dy * j));
                }*/


                // Maybe we should just do nothing
            } else {
                for(unsigned int k(0); k < numberertexAtLevel; ++k) {
                    Edge* currentFloorEdge = level[k]->getEdge2();
                    OMMesh::Normal* currentFloorEdgeNormal = currentFloorEdge->getNormal();

                    if(currentFirstFloorEdge->isParallel(currentFloorEdge)) {
                        //check also dot product between normals
                        float dotProduct = (*currentFirstFloorEdgeNormal) | (*currentFloorEdgeNormal);

                        //if the dot product is negativ, then the two edges normals doesnt face toward the same direction
                        if (dotProduct >= 0.0f) {
                            currentProfile->addVertexEnd(new Vertex(currentFirstFloorEdge->distance(currentFloorEdge), dy * j));
                        }
                    }
                }
            }
        }
    }

}

void FloorPlanAndProfileExtractor::findMinMaxYValueMesh(const OMMesh *inputMesh, float &minY, float &maxY)
{
    OMMesh::VertexIter v_it, v_end(inputMesh->vertices_end());
    minY = FLT_MAX;
    maxY = FLT_MIN;

    for (v_it = inputMesh->vertices_begin(); v_it != v_end; ++v_it) {
        minY = std::min((inputMesh->point(v_it))[1], minY);
        maxY = std::max((inputMesh->point(v_it))[1], maxY);
    }
}

void FloorPlanAndProfileExtractor::extractAllPlans(std::vector<std::vector<Vertex *> > &plans, const OMMesh *inputMesh)
{
    OpenMesh::Vec3f p(0,0,0);
    OpenMesh::Vec3f n(0,1,0);

    for (OMMesh::EdgeIter e_it=inputMesh->edges_begin(); e_it!=inputMesh->edges_end(); ++e_it) {

        //get the 2 halfedge of this edge:
        OMMesh::HalfedgeHandle halfedge1 = inputMesh->halfedge_handle( e_it, 0);
        OMMesh::HalfedgeHandle halfedge2 = inputMesh->halfedge_handle( e_it, 1);

        OMMesh::FaceHandle face1 = inputMesh->face_handle(halfedge1);
        OMMesh::FaceHandle face2 = inputMesh->face_handle(halfedge2);

        OpenMesh::Vec3f startPoint = inputMesh->point( inputMesh->from_vertex_handle( halfedge1 ) );
        OpenMesh::Vec3f endPoint = inputMesh->point( inputMesh->to_vertex_handle( halfedge1 ) );

        float minY;
        float maxY;
        if(startPoint[1] <= endPoint[1]){
            minY = startPoint[1];
            maxY = endPoint[1];
        } else {
            minY = endPoint[1];
            maxY = startPoint[1];
        }

        // ray - plan intersection, to find the intersection
        // notation: ray: o + t*d, plan equation: (x - p) dot (n) = 0
        OpenMesh::Vec3f d = (endPoint - startPoint).normalized();

        for (int i = 0; i < levels; i++) {
            float actualY = dy * i + baseY;
            //check that our edge is at the current height and is not parallel to our plan
            if((maxY >= actualY) && (minY <= actualY) && (actualY != minY || actualY != maxY)){
                p[1] = actualY;
                //find intersection point: //det o je ne sait pas quoi
                float t = -OpenMesh::dot(startPoint - p,n)/OpenMesh::dot(d,n);

                OpenMesh::Vec3f intersectionPoint = startPoint + t*d;

                //if the intersection is not on our edge, we go to the next iteration
                if ((intersectionPoint[1] > maxY + 0.001f) || (intersectionPoint[1] < minY - 0.001f)) { // how to choose this two variables (0.001f)?
                    continue;
                }

                if(std::abs(intersectionPoint[1] - actualY) > 0.001f){
                    std::cerr << "intersection found at "<< intersectionPoint[1] <<  " but Y we want is: " << actualY << std::endl;
                }

                bool found = false;
                foreach (Vertex* vertex, plans[i]) {
                    if(std::abs( vertex->getX() - intersectionPoint[0]) < 0.001f
                            && std::abs( vertex->getY() - intersectionPoint[2]) < 0.001f ){
                        vertex->addFace(face1);
                        vertex->addFace(face2);

                        found = true;
                        break;
                    }
                }

                if (!found){
                    Vertex* vertex   =  new Vertex(intersectionPoint[0], intersectionPoint[2], true);
                    vertex->addFace(face1);
                    vertex->addFace(face2);
                    plans[i].push_back(vertex);
                }

            }

        }
    }

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
            if (vertex != vertex->getNeighbor2()->getNeighbor1() && vertex == vertex->getNeighbor2()->getNeighbor2()){
                Vertex* otherNeighbour = vertex->getNeighbor2()->getNeighbor1();
                vertex->getNeighbor2()->setNeighbor1(vertex);
                vertex->getNeighbor2()->setNeighbor2(otherNeighbour);
            } else if (vertex != vertex->getNeighbor2()->getNeighbor1() && vertex == vertex->getNeighbor2()->getNeighbor2()) {
                std::cerr << "Error chain construction" << std::endl;
            }

            if (vertex != vertex->getNeighbor1()->getNeighbor2() && vertex == vertex->getNeighbor1()->getNeighbor1()){
                Vertex* otherNeighbour = vertex->getNeighbor1()->getNeighbor2();
                vertex->getNeighbor1()->setNeighbor2(vertex);
                vertex->getNeighbor1()->setNeighbor1(otherNeighbour);
            } else if (vertex != vertex->getNeighbor2()->getNeighbor1() && vertex == vertex->getNeighbor2()->getNeighbor2()) {
                std::cerr << "Error chain construction" << std::endl;
            }
        }
    }


    ///////DEBUT TEST
    std::cerr <<"--------------------------" << std::endl << "avec les invalid" << std::endl;
    int po=0;
    foreach (std::vector<Vertex*> level, plans) {
        std::cerr << "chain " << po << std::endl;
        if (level.size() > 2 ) {
        foreach (Vertex* vertex, level) {
            std::cerr << vertex->getNeighbor1()->getX() << ", " << vertex->getNeighbor1()->getY();
            std::cerr << " || " << vertex->getX() << ", " << vertex->getY() << " || ";
            std::cerr << vertex->getNeighbor2()->getX() << ", " << vertex->getNeighbor2()->getY() << std::endl;
        }
        }
        po++;
    }
    ///////FIN TEST


    //find vertex of v that have the same face normal delete it and replace with neigbour
    for(unsigned int j(0); j < plans.size(); ++j) {
        std::vector<Vertex*> level = plans[j];
        foreach (Vertex* vertex, level) {
            bool same = true;
            OMMesh::Normal normal = inputMesh->normal((*vertex->getFaces())[0]);
            for (unsigned int i=0 ; i < vertex->getFaces()->size(); i++){
                OMMesh::Normal comparedNormal = inputMesh->normal((*vertex->getFaces())[i]);
                if(std::abs(comparedNormal[0] - normal[0]) + std::abs(comparedNormal[1] - normal[1]) + std::abs(comparedNormal[2] - normal[2]) > 0.001f){
                    same = false;
                    break;
                }
            }
            if(same){
                vertex->getNeighbor1()->setNeighbor2(vertex->getNeighbor2());
                vertex->getNeighbor2()->setNeighbor1(vertex->getNeighbor1());

                vertex->getNeighbor1()->addFace((*vertex->getFaces())[0]);
                vertex->getNeighbor2()->addFace((*vertex->getFaces())[0]);

                vertex->invalid();
            }
        }
    }


    ///////DEBUT TEST
    std::cerr <<"--------------------------" << std::endl << "sans les invalid" << std::endl;
    po=0;
    foreach (std::vector<Vertex*> level, plans) {
        std::cerr << "chain " << po << std::endl;
        if (level.size() > 2 ) {
        foreach (Vertex* vertex, level) {
            if (vertex->isValid()) {
            std::cerr << vertex->getNeighbor1()->getX() << ", " << vertex->getNeighbor1()->getY();
            std::cerr << " || " << vertex->getX() << ", " << vertex->getY() << " || ";
            std::cerr << vertex->getNeighbor2()->getX() << ", " << vertex->getNeighbor2()->getY() << std::endl;
            }
        }
        }
        po++;
    }
    ///////FIN TEST

    //remove invalid vertex
    for(unsigned int i(0); i < plans.size(); ++i){
        std::vector<Vertex*> level = plans[i];
        std::vector<Vertex*> newLevel;
        for(unsigned int j(0); j < level.size(); ++j) {
            Vertex* current = level[j];
            if (current->isValid()) {
                newLevel.push_back(current);
            }
        }
        plans[i] = newLevel;
    }
}

void FloorPlanAndProfileExtractor::upsideDownCorrection(std::vector<std::vector<Vertex *> > &plans)
{
    unsigned int planSize = plans.size();
    unsigned int plansSize_2 = std::ceil((float)planSize / 2.0f);
    for(unsigned int i(0); i < plansSize_2; ++i) {
        unsigned int j = planSize - 1 - i;
        std::vector<Vertex *> level1 = plans[i];
        std::vector<Vertex *> level2 = plans[j];
        plans[i] = level2;
        plans[j] = level1;
    }
}

void FloorPlanAndProfileExtractor::extract(const OMMesh* inputMesh, Vertex*& floorPlan,
                                           Profile*& currentProfile, unsigned int& floorPlanSize)
{
    float minYValue(0.0f);
    float maxYValue(0.0f);
    findMinMaxYValueMesh(inputMesh, minYValue, maxYValue);

    dy = (maxYValue - minYValue) / (levels - 1);
    baseY = minYValue;

    std::vector< std::vector<Vertex*> > plans(levels, std::vector<Vertex*>());

    extractAllPlans(plans, inputMesh);

    //check that the first floor plan has at least 3 vertices
    if (plans[0].size() < 3) {
        //Maybe the mesh is upside down, we try to correct it
        upsideDownCorrection(plans);
        if (plans[0].size() < 3) {
            std::cerr << "First floor plans has less than 3 vertices!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }


    recenter(plans);
    rescale(plans);





    ///////DEBUT TEST
    std::cerr << "-----------------------------" << std::endl;
    int ii = 1;
    foreach (std::vector<Vertex*> level, plans) {
        std::cerr << "level " << ii << std::endl;
        if(level.size() < 3) {
            continue;
        }
        foreach (Vertex* vertex, level) {
            std::cerr << vertex->getNeighbor1()->getX() << ", " << vertex->getNeighbor1()->getY() << " || " << vertex->getX() << ", " << vertex->getY() << " || " << vertex->getNeighbor2()->getX() << ", " << vertex->getNeighbor2()->getY() << std::endl;
            std::cerr << "Valid ?" << std::endl;
            std::cerr << vertex->getNeighbor1()->isValid() << ", " << vertex->isValid() << ", " << vertex->getNeighbor2()->isValid() << std::endl;
        }
        std::cerr <<std:: endl;
        ii++;
    }
    std::cerr << "-----------------------------" << std::endl;
    ////////FIN TEST





    profileConstruction(inputMesh, plans);

    std::vector<Vertex*> firstFloorPlanlevel = plans[0];
    floorPlanSize = firstFloorPlanlevel.size();
    floorPlan = firstFloorPlanlevel[0];
    currentProfile = floorPlan->getEdge2()->getProfile();

}
