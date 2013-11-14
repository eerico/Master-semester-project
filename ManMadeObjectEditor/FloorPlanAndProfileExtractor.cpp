#include "FloorPlanAndProfileExtractor.h"


FloorPlanAndProfileExtractor::FloorPlanAndProfileExtractor()
    :dy(0.1f), baseY(0.0f), levels(11), border(0.2f)
{

}

void FloorPlanAndProfileExtractor::recenter(std::vector< std::vector< Vertex* > >& plans) {
    //move the mesh such that (0, 0) is at the center of the first floor plan
    std::vector< Vertex* > firstLevel = plans[0];

    // compute the center
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

void FloorPlanAndProfileExtractor::rescale(std::vector< std::vector< Vertex* > >& plans) {
    //scale the mesh such that the first floor plan is between [-1, 1]x[-1, 1]
    std::vector< Vertex* > firstLevel = plans[0];

    // compute the minimum and maximum X and Y
    float minX(FLT_MAX);
    float minY(FLT_MAX);
    
    float maxX(FLT_MIN);
    float maxY(FLT_MIN);
    
    unsigned int firstLevelSize = firstLevel.size();    
    for(unsigned int i(0); i < firstLevelSize; ++i) {
        Vertex* currentVertex = firstLevel[i];
        minX = std::min(currentVertex->getX(), minX);
        minY = std::min(currentVertex->getY(), minY);
        
        maxX = std::max(currentVertex->getX(), maxX);
        maxY = std::max(currentVertex->getY(), maxY);
    }

    // find the rescale factor
    float rescalFactor = std::max(std::max(std::max(maxX, maxY), -minX), -minY);
    // border can be used to avoid a perfect [-1, 1]x[-1, 1]
    rescalFactor += border * rescalFactor;
    
    //Now scale the plans
    for(unsigned int i(0); i < plans.size(); ++i) {
        std::vector< Vertex* > currentLevel = plans[i];
        for(unsigned int j(0); j < currentLevel.size(); ++j) {
            Vertex* currentVertex = currentLevel[j];
            currentVertex->setX(currentVertex->getX() / rescalFactor);
            currentVertex->setY(currentVertex->getY() / rescalFactor);
        }
    }
    
    // also scale the delta height
    dy = dy / rescalFactor;
}

void FloorPlanAndProfileExtractor::profileConstruction(const OMMesh* inputMesh, std::vector<std::vector<Vertex *> > &plans) {

    // For every two vertices, find the faces normals they have in common,
    // construct the edge between them and associate a normal to this edge
    for(unsigned int i(0); i < plans.size(); ++i){
        std::vector<Vertex*> level = plans[i];
        unsigned int numberVertexAtLevel = level.size();
        if (numberVertexAtLevel > 1) {
            for(unsigned int j(0); j < numberVertexAtLevel; ++j) {
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
    
    
    /*
     * Construct the profile:
     The algorithm works as follow: For every edge e1 on the first floor plan we will construct its profile
     Thus we take le floor plan at level 2, then 3 etc.. And for every edge ei at the level i
     if the two edges are not parallel, then they do not define a profile. If they are parallel,
     they can be associated with face in opposite direction. Thus we check the dot product between
     the two edges (e1 and ei) normal and if this dot product is negativ, then the two edges normals doesnt
     face toward the same direction and thus do not define a profile. if the dot product is positiv
     then they can define a profile. In fact, more than one edge ei can pass all of these test, thus we
     keep only the edge that is the nearest to e1.
     Finally, given an edge e1 and an edge ei that define a part of the profile, we can add a vertex to the profile
     where the inclination is the distance between e1 and ei and the height is simply dy * i. The only question remaining
     is the sign of the inclination. To compute it, we use the normal n at e1 and the direction vector from e1 to ei called d.
     The normal n at e1 is oriented toward the exterior, thus if the dot product between n and d is positiv, it means that
     ei is in the direction pointed by the normal of e1 and thus the sign of the inclination is -1, otherwise the sign is 1
    */

    std::vector<Vertex*> firstLevel = plans[0];
    //For every edge on the first floor plan
    for(unsigned int i(0); i < firstLevel.size(); ++i) {
        Edge* currentFirstFloorEdge = firstLevel[i]->getEdge2();
        OMMesh::Normal* currentFirstFloorEdgeNormal = currentFirstFloorEdge->getNormal();
        
        // we will add vertices into this profile associated with the current first floor edge
        Profile* currentProfile = new Profile(true);
        currentFirstFloorEdge->setProfile(currentProfile);
        ProfileDestructorManager::putProfile(currentProfile);

        // we try to find a profile vertex at each level above
        for(unsigned int j(1); j < plans.size(); ++j){
            std::vector<Vertex*> level = plans[j];

            unsigned int numberVertexAtLevel = level.size();
            if (numberVertexAtLevel == 1) {
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
                float minDistance = FLT_MAX;
                float sign = 1.0f;
                for(unsigned int k(0); k < numberVertexAtLevel; ++k) {
                    // for every edge at this level
                    Edge* currentFloorEdge = level[k]->getEdge2();
                    OMMesh::Normal* currentFloorEdgeNormal = currentFloorEdge->getNormal();

                    if(currentFirstFloorEdge->isParallel(currentFloorEdge)) {
                        //check also dot product between normals
                        float dotProduct = (*currentFirstFloorEdgeNormal) | (*currentFloorEdgeNormal);
                        //if the dot product is negativ, then the two edges normals doesnt face toward the same direction
                        if (dotProduct >= 0.0f) {
                            float x = currentFloorEdge->getVertex1()->getX() - currentFirstFloorEdge->getVertex1()->getX();
                            float y = currentFloorEdge->getVertex1()->getY() - currentFirstFloorEdge->getVertex1()->getY();
                            
                            float distance = currentFirstFloorEdge->distance(currentFloorEdge);
                            // if two edge are parallel but should be connected with a profile, we can
                            // first check if there is not a better candidate (that is closer)
                            if(distance < minDistance) {
                                dotProduct = Utils::dotProduct(x, y, (*currentFirstFloorEdgeNormal)[0], (*currentFirstFloorEdgeNormal)[2]);
                                minDistance = distance;
                                if(dotProduct > 0){
                                    sign = -1.0f;
                                } else {
                                    sign = 1.0f;
                                }
                            }
                        }
                    }
                }
                if (minDistance < FLT_MAX) {
                    currentProfile->addVertexEnd(new Vertex(sign * minDistance, dy * j));
                }
            }
        }
        //we can decimate the profile.
        currentProfile->vertexDecimation();
    }
    
}

void FloorPlanAndProfileExtractor::findMinMaxYValueMesh(const OMMesh *inputMesh, float &minY, float &maxY) {
    OMMesh::VertexIter v_it, v_end(inputMesh->vertices_end());
    minY = FLT_MAX;
    maxY = FLT_MIN;
    
    for (v_it = inputMesh->vertices_begin(); v_it != v_end; ++v_it) {
        minY = std::min((inputMesh->point(v_it))[1], minY);
        maxY = std::max((inputMesh->point(v_it))[1], maxY);
    }
}

bool FloorPlanAndProfileExtractor::extractAllPlans(std::vector<std::vector<Vertex *> > &plans, const OMMesh *inputMesh) {

    /*  First we have to find sample on the mesh
     *
        The algorithm works as follow:
        for every edge of the mesh, we will do a plan/edge. We consider the edge as a ray,
        compute the intersection and then test if this intersection is on the edge. If it is,
        then we have a vertex. We do this for every edge anf for plan at every height.
        notation: ray: o + t*d, plan equation: (x - p) dot (n) = 0, we want to find x

        Later, we will have to transform this set of vertices into a chains of vertices,
        defining every floor plan. Thus to know of two vertices are neighbor, we store
        every faces associated with the vertex when we compute it during the plan/edge intersection.
        Because if two vertices are at the same height and have at least one face in common, they
        are neighbor.
    */
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
    
    //find all neighbour in a certain floor using the face information
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
    

    //build chain at every level  //////what if a vertex has only a neighbour???
    foreach (std::vector<Vertex*> level, plans) {
        if (level.size() < 3) {
            if(level.size() > 1){
                level[0]->setNeighbor2(level[1]);
                level[0]->setNeighbor1(level[1]);
                level[1]->setNeighbor2(level[0]);
                level[1]->setNeighbor1(level[0]);
            }
            foreach (Vertex* v, level) {
                v->setValid(true);
            }
            continue;
        }

        while (level.size()!=0){
            Vertex* firstVertex = level[0];
            Vertex* currentVertex = level[0];
            Vertex* nextVertex;
            for(unsigned int i(0); i < level.size(); i++) {
                nextVertex = currentVertex->getNeighbor2();
                if(nextVertex == 0){
                    std::cerr << "neigbour = 0 ; " << i << std::endl;
                    break;

                }
                if(nextVertex->getNeighbor2() == currentVertex){
                    Vertex* tempV = nextVertex->getNeighbor1();
                    nextVertex->setNeighbor1(nextVertex->getNeighbor2());
                    nextVertex->setNeighbor2(tempV);
                }
                if(nextVertex!= 0 && nextVertex->getNeighbor1() != currentVertex){
                    std::cerr << "Error chain construction" << std::endl;
                    return false;
                } else {
                    currentVertex->setValid(true);
                    if(nextVertex->isValid()){
                        break;
                    }
                    nextVertex->setValid(true);
                    currentVertex = nextVertex;

                }

            }
            if(currentVertex->getNeighbor2() != firstVertex){
                std::cerr << level.size()<< "Error chain construction!!!!!!" << std::endl;
                return false;
            }

            std::vector<Vertex*> tempLevel;
            for (unsigned int i(0); i < level.size(); i++){
                if(!level[i]->isValid() && level[i]->getNeighbor2()!=0){
                    tempLevel.push_back(level[i]);
                } else if(!level[i]->isValid()) {
                    level[i]->setValid(true);
                    std::cerr << "top found!" << std::endl;
                }
            }
            level = tempLevel;
        }

    }     
    
    //find vertex of v that have the same face normal delete it and replace with neigbour
    for(unsigned int j(0); j < plans.size(); ++j) {
        std::vector<Vertex*> level = plans[j];
        foreach (Vertex* vertex, level) {
            bool same = true;
            OMMesh::Normal normal = inputMesh->normal((*vertex->getFaces())[0]);
            for (unsigned int i=0 ; i < vertex->getFaces()->size(); i++){
                OMMesh::FaceHandle face = (*vertex->getFaces())[i];
                if (face.idx() < 0){
                    return false;
                }
                OMMesh::Normal comparedNormal = inputMesh->normal(face);
                if(std::abs(comparedNormal[0] - normal[0]) + std::abs(comparedNormal[1] - normal[1]) + std::abs(comparedNormal[2] - normal[2]) > 0.005f){
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
    return true;
}

void FloorPlanAndProfileExtractor::upsideDownCorrection(std::vector<std::vector<Vertex *> > &plans) {
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

bool FloorPlanAndProfileExtractor::extract(const OMMesh* inputMesh, Vertex*& floorPlan,
                                           Profile*& currentProfile, unsigned int& floorPlanSize,
                                           std::vector< std::vector< Vertex* > >& plansAbove) {

    float minYValue(0.0f);
    float maxYValue(0.0f);
    findMinMaxYValueMesh(inputMesh, minYValue, maxYValue);
    
    // define an uniform delta height at which we will search samples
    dy = (maxYValue - minYValue) / (levels - 1);

    // define the height of the first plan
    baseY = minYValue;
    
    std::vector< std::vector<Vertex*> > plans(levels, std::vector<Vertex*>());
    
    // extract all plan
    if(!extractAllPlans(plans, inputMesh)){
        return false;
    }
    
    //check that the first floor plan has at least 3 vertices
    if (plans[0].size() < 3) {
        //Maybe the mesh is upside down, we try to correct it
        upsideDownCorrection(plans);
        if (plans[0].size() < 3) {
            std::cerr << "First floor plans has less than 3 vertices!" << std::endl;
            return false;
        }
    }    
    
    recenter(plans);
    rescale(plans);      
    
    // construct all profiles
    profileConstruction(inputMesh, plans);
    
    std::vector<Vertex*> firstFloorPlanlevel = plans[0];

    //decimate the profiles
    profileDecimation(firstFloorPlanlevel);
    
    floorPlanSize = firstFloorPlanlevel.size();
    floorPlan = firstFloorPlanlevel[0];
    currentProfile = floorPlan->getEdge2()->getProfile();
    
    // store all levels, thus the user will be able to see them
    for(unsigned int i(1); i < plans.size(); ++i) {
        plansAbove.push_back(plans[i]);
    }
    return true;
}


void FloorPlanAndProfileExtractor::profileDecimation(std::vector<Vertex*>& firstFloorPlanlevel) {
    foreach (Vertex* vertexP, firstFloorPlanlevel) {
        foreach (Vertex* vertexC, firstFloorPlanlevel) {
            if(vertexC != vertexP && vertexC->getEdge2()->getProfile()!= vertexP->getEdge2()->getProfile()){
                if(vertexC->getEdge2()->getProfile()->isEqual(vertexP->getEdge2()->getProfile())){
                    vertexC->getEdge2()->setProfile(vertexP->getEdge2()->getProfile());
                }
            }
        }
    }
}
