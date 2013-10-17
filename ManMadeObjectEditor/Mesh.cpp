#include "Mesh.h"

Mesh::Mesh(): inputMesh(0), floorPlan(0)
{
    // simple test
    triangles.push_back(new qglviewer::Vec(-1.0f, 0.0f, 0.5f));
    triangles.push_back(new qglviewer::Vec(-1.0f, 1.0f, 0.5f));
    triangles.push_back(new qglviewer::Vec(1.0f, 0.0f, 0.5f));

    triangles.push_back(new qglviewer::Vec(1.0f, 0.0f, 0.5f));
    triangles.push_back(new qglviewer::Vec(-1.0f, 1.0f, 0.5f));
    triangles.push_back(new qglviewer::Vec(1.0f, 1.0f, 0.5f));

}

Mesh::~Mesh()
{
    unsigned int size = triangles.size();
    for( unsigned int i(0); i < size; ++i)
    {
        qglviewer::Vec* tmp = triangles[i];
        delete tmp;
    }

    Vertex* currentVertex = floorPlan;
    for( unsigned int i(0); i < floorPlanSize; ++i)
    {
        Vertex* next = currentVertex->getNeighbor2();
        if (currentVertex != 0) {
            //Not needed because (Qt documentation):
            // "Removes and deletes all items from the scene object before destroying the scene object.
            // The scene object is removed from the application's global scene list, and it is removed
            // from all associated views."

            /*if (currentVertex->getEdge2() != 0) {
                delete currentVertex->getEdge2();
            }
            if (currentVertex->getEllipse() != 0){
                delete currentVertex->getEllipse();
            }*/
            delete currentVertex;
            currentVertex = 0;
        }
        currentVertex = next;
    }
}

void Mesh::setFloorPlan(FloorVertex* vertex) {
   floorPlan = vertex;
}

const std::vector<qglviewer::Vec *>& Mesh::getTriangles()
{
    // va devoir appliquer algorithme que etant donnée les floorVertex/edges et les profile, reconstruire les triangles
    return triangles;
}

const std::vector<qglviewer::Vec*> Mesh::getPoints(bool moreSample, float ds)
{

    std::vector<qglviewer::Vec*> points;

    if (floorPlanSize == 0) {
        return points;
    }

    if (moreSample) {
        getPointWithAdditionnalSampledPoint(points, ds);
    } else {
        getPointBasic(points);
    }

    return points;
}

void Mesh::getPointBasic(std::vector<qglviewer::Vec*>& points)
{
    float centerX(0.0f);
    float centerY(0.0f);

    Vertex* tmp = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i) {
        centerX += tmp->getX();
        centerY += tmp->getY();
        tmp = tmp->getNeighbor2();
    }

    centerX = centerX / (float)(floorPlanSize);
    centerY = centerY / (float)(floorPlanSize);

    FloorVertex* floorVertexTmp = floorPlan;

    for(unsigned int i(0); i < floorPlanSize; ++i) {

        Profile* profile = floorVertexTmp->getProfile();
        if (profile != 0) {
           Vertex* pVertex = profile->getProfileVertex();
            while(pVertex != 0)
            {
                float w = pVertex->getX();
                float z = pVertex->getY();

                float x = floorVertexTmp->getX()* (1.0f - w) + centerX * w;
                float y = floorVertexTmp->getY()* (1.0f - w) + centerY * w;

                points.push_back(new qglviewer::Vec(x, y, z));
                pVertex = pVertex->getNeighbor2();
            }
        }
        floorVertexTmp = (FloorVertex*)floorVertexTmp->getNeighbor2();
    }
}

// First try
/*void Mesh::getPointWithAdditionnalSampledPoint(std::vector<qglviewer::Vec*>& points, float ds)
{
    // very basic. Let say we have two vertices on the floor plan v1 and v2,
    // we interpolate new vertices between these two and take the profile of v1
    // and "associate" this profile to the new vertices. This is the idea
    // used to add new sample in the floor plan
    // To add new sample for the profile, we take vertices of the profile
    // two by two and interpolate
    // This method is only used to show more points on the 3D rendering
    float centerX(0.0f);
    float centerY(0.0f);

    Vertex* tmp = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i) {
        centerX += tmp->getX();
        centerY += tmp->getY();
        tmp = tmp->getNeighbor2();
    }

    centerX = centerX / (float)(floorPlanSize);
    centerY = centerY / (float)(floorPlanSize);

    FloorVertex* floorVertexTmp = floorPlan;
    FloorVertex* nextFloorVertexTmp = (FloorVertex*)floorVertexTmp->getNeighbor2();

    if (nextFloorVertexTmp == 0) {
        return;
    }

    for(unsigned int i(0); i < floorPlanSize; ++i) {

        Profile* profile = floorVertexTmp->getProfile();
        if (profile != 0) {
           Vertex* pVertex = profile->getProfileVertex();

           float nextX = nextFloorVertexTmp->getX();
           float nextY = nextFloorVertexTmp->getY();


            while(pVertex->getNeighbor2() != 0)
            {
                float currentW = pVertex->getX();

                Vertex* nextPVertex = pVertex->getNeighbor2();
                float nextW = nextPVertex->getX();
                float currentZ = pVertex->getY();
                float nextZ = nextPVertex->getY();

                float t = 0.0f;
                while(t < 1.0f) {
                    float t_1 = 1.0f - t;
                    float w = currentW * t + nextW * t_1;
                    float z = currentZ * t + nextZ * t_1;

                    // we take a new sample at every ds between two neighbor
                    // and use the profile of the first neighbor
                    float tt = 0.0f;
                    while(tt < 1.0f) {
                        float tt_1 = 1.0f - tt;
                        float sampledX = floorVertexTmp->getX() * tt + tt_1 * nextX;
                        float sampledY = floorVertexTmp->getY() * tt + tt_1 * nextY;

                        float newX = sampledX * (1.0f - w) + centerX * w;
                        float newY = sampledY * (1.0f - w) + centerY * w;

                        points.push_back(new qglviewer::Vec(newX, newY, z));

                        tt += ds;
                    }
                    t += ds;
                }
                pVertex = pVertex->getNeighbor2();
            }
        }
        floorVertexTmp = (FloorVertex*)floorVertexTmp->getNeighbor2();
        nextFloorVertexTmp = (FloorVertex*)nextFloorVertexTmp->getNeighbor2();
    }
}*/

void Mesh::getPointWithAdditionnalSampledPoint(std::vector<qglviewer::Vec*>& points, float ds)
{
    // very basic. Let say we have two vertices on the floor plan v1 and v2,
    // we interpolate new vertices between these two and take the profile of v1
    // and "associate" this profile to the new vertices. This is the idea
    // used to add new sample in the floor plan
    // To add new sample for the profile, we take vertices of the profile
    // two by two and interpolate
    // This method is only used to show more points on the 3D rendering
    float centerX(0.0f);
    float centerY(0.0f);

    Vertex* tmp = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i) {
        centerX += tmp->getX();
        centerY += tmp->getY();
        tmp = tmp->getNeighbor2();
    }

    centerX = centerX / (float)(floorPlanSize);
    centerY = centerY / (float)(floorPlanSize);

    FloorVertex* floorVertexTmp = floorPlan;
    FloorVertex* nextFloorVertexTmp = (FloorVertex*)floorVertexTmp->getNeighbor2();

    if (nextFloorVertexTmp == 0) {
        return;
    }

    for(unsigned int i(0); i < floorPlanSize; ++i) {

        Profile* profile = floorVertexTmp->getProfile();
        if (profile != 0) {
           Vertex* pVertex = profile->getProfileVertex();

           float nextX = nextFloorVertexTmp->getX();
           float nextY = nextFloorVertexTmp->getY();
           float currentX = floorVertexTmp->getX();
           float currentY = floorVertexTmp->getY();

           // we take a new sample at every ds between two neighbor
           // and use the profile of the first neighbor
            while(pVertex->getNeighbor2() != 0)
            {
                float currentW = pVertex->getX();

                Vertex* nextPVertex = pVertex->getNeighbor2();
                float nextW = nextPVertex->getX();
                float currentZ = pVertex->getY();
                float nextZ = nextPVertex->getY();

                float dirW = nextW - currentW;
                float dirZ = nextZ - currentZ;
                float norm = std::sqrt(dirW * dirW + dirZ * dirZ);
                dirW = dirW / norm;
                dirZ = dirZ / norm;

                float sampledW(0.0f);
                float newZ(0.0f);
                float t = 0.0f;

                float deltaW = sampledW - nextW;
                float deltaZ = newZ - nextZ;

                // if the distance beween the samplend W,Z and next W,Z is small, stop
                do {
                    sampledW = currentW + dirW * t;
                    newZ = currentZ + dirZ * t;

                    deltaW = sampledW - nextW;
                    deltaZ = newZ - nextZ;

                    float tt = 0.0f;
                    float sampledX(0.0f);
                    float sampledY(0.0f);
                    float dirX = nextX - currentX;
                    float dirY = nextY - currentY;
                    norm = std::sqrt(dirX * dirX + dirY * dirY);
                    dirX = dirX / norm;
                    dirY = dirY / norm;

                    // if the distance beween the samplend X,Y and next X,Y is small, stop
                    float deltaX = sampledX - nextX;
                    float deltaY = sampledY - nextY;
                    do {
                        sampledX = currentX + dirX * tt;
                        sampledY = currentY + dirY * tt;

                        deltaX = sampledX - nextX;
                        deltaY = sampledY - nextY;

                        float newX = sampledX * (1.0f - sampledW) + centerX * sampledW;
                        float newY = sampledY * (1.0f - sampledW) + centerY * sampledW;

                        points.push_back(new qglviewer::Vec(newX, newY, newZ));

                        tt += ds;
                    } while(std::sqrt(deltaX*deltaX + deltaY*deltaY) > ds);

                    t += ds;

                } while (std::sqrt(deltaW*deltaW + deltaZ*deltaZ) > ds);

                pVertex = pVertex->getNeighbor2();
            }
        }
        floorVertexTmp = (FloorVertex*)floorVertexTmp->getNeighbor2();
        nextFloorVertexTmp = (FloorVertex*)nextFloorVertexTmp->getNeighbor2();
    }
}


void Mesh::update()
{
    // hum surement que va reconstruire la liste de triangle en fonction du nouvelle input de point/profile donnée
    qglviewer::Vec* tmp = triangles[0];
    tmp->x -= 0.0001f;
}

unsigned int Mesh::getFloorPlanSize()
{
    return floorPlanSize;
}

void Mesh::incrementFloorPlanSize()
{
    floorPlanSize++;
}

void Mesh::decrementFloorPlanSize()
{
    floorPlanSize--;
}

Profile* Mesh::getCurrentProfile()
{
    return currentProfile;
}

void Mesh::setCurrentProfile(Profile* p)
{
    currentProfile = p;
}

void Mesh::loadMesh(QString fileName)
{
    if(inputMesh != 0)
    {
        delete inputMesh;
    }
    inputMesh = new OMMesh;

    Vertex* currentVertex = floorPlan;
    for( unsigned int i(0); i < floorPlanSize; ++i)
    {
        Vertex* next = currentVertex->getNeighbor2();
        delete currentVertex->getEdge2();
        delete currentVertex->getEllipse();
        delete currentVertex;
        currentVertex = next;
    }
    floorPlanSize = 0;
    currentProfile = 0;

    inputMesh->request_face_normals();
    inputMesh->request_vertex_normals();

    OpenMesh::IO::Options ropt;
    ropt += OpenMesh::IO::Options::VertexNormal;
    if (OpenMesh::IO::read_mesh(*inputMesh, fileName.toLocal8Bit().constData()),
            ropt)
    {
        inputMesh->update_face_normals();
        inputMesh->update_vertex_normals();
    }
    floorplanAndProfileExtraction();
}

void Mesh::floorplanAndProfileExtraction()
{
    OMMesh::VertexIter v_it, v_end(inputMesh->vertices_end());

    // array of plan
    std::vector<std::vector<OMMesh::VertexIter>* > plan;

    //TODO trouver comment computer ces chiffre magique
    // la c est exemple avec le cone
    float dz(0.1f); //le delta entre 2 plan
    int totalNumPlan(11); // nombre de plan jusqu au sommet
    float baseZ(-0.5f); // le z minimum ou se trouve le floor plan

    // pas le vrai floorplan, un cran au dessus afin d eviter d'avoir un fond plein donc que 2 neighbor au meme
    // plan par vertex!
    int floorPlanIndex(0); //0

    for(int i(0); i < totalNumPlan; ++i){
        std::vector<OMMesh::VertexIter>* planTmp = new std::vector<OMMesh::VertexIter>();
        plan.push_back(planTmp);
    }

    struct TmpFloorVertex {
        OMMesh::Point point;
        OMMesh::Point neighbor1;
        OMMesh::Point neighbor2;
        bool neighbor1Used;
        bool neighbor2Used;
    };
    std::vector<TmpFloorVertex> tmpFloorPlanVector;

    OMMesh::VertexVertexIter vv_it;

    // iterate over the vertices to construct a floor plan using the TmpFloorVertex struct
    // and store for each plan all vertices (iterator) associated with the plan
    // it will be usefull to construct the profiles
    for (v_it=inputMesh->vertices_begin(); v_it!=v_end; ++v_it) {
            OMMesh::Point point = inputMesh->point(v_it);
            float planNumTmp = (point[2] - baseZ) / dz;
            int planNum = floor(planNumTmp + 0.5f);

            // check if planNum is really a correct plan number
            // 0.001f because of precision problem
            if ((abs(planNumTmp - planNum) < 0.001f) && (planNum >= floorPlanIndex)) {

                // store the vertices with the associated plan
                std::vector<OMMesh::VertexIter>* planTmp = plan[planNum];
                planTmp->push_back(v_it);
                point = inputMesh->point(v_it);

                // check is the current vertex is on the floor plan
                if (abs(planNum - floorPlanIndex) < 0.001f) {
                    TmpFloorVertex currentVertex;
                    currentVertex.point = point;
                    currentVertex.neighbor1Used = false;
                    currentVertex.neighbor2Used = false;

                    // find the two neighbor of this floor plan vertex
                    for (vv_it=inputMesh->vv_iter(v_it); vv_it; ++vv_it) {
                        OMMesh::Point neighborTmp = inputMesh->point(vv_it);
                        planNumTmp = (neighborTmp[2] - baseZ) / dz;
                        planNum = floor(planNumTmp + 0.5);
                        if (abs(planNumTmp - planNum) < 0.001f ) {
                            if (abs(planNum - floorPlanIndex) < 0.001f) {
                                if (currentVertex.neighbor1Used == false) {
                                    currentVertex.neighbor1 = neighborTmp;
                                    currentVertex.neighbor1Used = true;
                                } else {
                                    currentVertex.neighbor2 = neighborTmp;
                                    currentVertex.neighbor2Used = true;
                                }
                            }
                        }

                    }
                    // store the current floor plan vertex with his neighbors information,
                    // used later to construct our floor plan on which we will work
                    tmpFloorPlanVector.push_back(currentVertex);
                }
            }
        }

    ////////////////////////////////////////////
    /*for(unsigned int i(0); i < tmpFloorPlanVector.size(); ++i) {
        TmpFloorVertex aa = tmpFloorPlanVector[i];

        std::cout << "vertex " << i << ": ";
        std::cout << "(" << aa.point[0] << ", " << aa.point[1] << ", " << aa.point[2] << ") ";
        std::cout << "; N1: " << "(" << (aa.neighbor1)[0] << ", " << (aa.neighbor1)[1] << ", " << (aa.neighbor1)[2] << ") ";
        std::cout << "; N2: " << "(" << (aa.neighbor2)[0] << ", " << (aa.neighbor2)[1] << ", " << (aa.neighbor2)[2] << ") ";
        std::cout << std::endl;
    }*/
    ////////////////////////////////////////////

    // with the previous computed information, we will construct the floor plan
    // on which we will work
    // The idea is to pick a vertex, then find one neighbor and thus defining a neighboring
    // relationship between them and iterate to construct our floor plan like this
    unsigned int size = tmpFloorPlanVector.size();

    // take a vertex
    TmpFloorVertex currentVertex = tmpFloorPlanVector[0];
    OMMesh::Point currentPoint = currentVertex.point;;
    OMMesh::Point currentVertexNeighbor1 = currentVertex.neighbor1;
    OMMesh::Point currentVertexNeighbor2 = currentVertex.neighbor2;

    FloorVertex* firstPoint;
    FloorVertex* previousFloorVertexNeighbor;
    TmpFloorVertex possibleNeighbor;
    bool found(false);

    // find the neighbors of the vertex we have just taken
    for(unsigned int j(0); j < size && !found; ++j) {
        possibleNeighbor = tmpFloorPlanVector[j];
        OMMesh::Point possibleNeighborPoint = possibleNeighbor.point;

        if ((possibleNeighborPoint == currentVertexNeighbor1) || (possibleNeighborPoint == currentVertexNeighbor2)) {
            FloorVertex* currentFloorVertexPoint = new FloorVertex(currentPoint[0], currentPoint[1]);
            incrementFloorPlanSize();

            FloorVertex* currentFloorVertexNeighbor = new FloorVertex(possibleNeighborPoint[0], possibleNeighborPoint[1]);

            currentFloorVertexPoint->setNeighbor2(currentFloorVertexNeighbor);//addNeighbor
            currentFloorVertexNeighbor->setNeighbor1(currentFloorVertexPoint);//addNeighbor

            if (possibleNeighbor.neighbor1 == currentPoint) {
                possibleNeighbor.neighbor1Used = false;
            } else {
                possibleNeighbor.neighbor2Used = false;
            }

            previousFloorVertexNeighbor = currentFloorVertexNeighbor;
            firstPoint = currentFloorVertexPoint;
            found = true;
        }
    }

    // our floor plan is a pointer on a vertex of the floor plan
    floorPlan = firstPoint;

    // we will then apply the same logic with the neighbor. It becomes our current vertex
    // and we will find his neighbors and iterate to construct our floor plan
    found = false;
    for (unsigned int i(1); i < size - 1; ++i) {
        currentVertex = possibleNeighbor;
        currentPoint = currentVertex.point;
        currentVertexNeighbor1 = currentVertex.neighbor1;
        currentVertexNeighbor2 = currentVertex.neighbor2;
        found = false;

        for(unsigned int j(0); (j < size) && !found; ++j) {
            possibleNeighbor = tmpFloorPlanVector[j];
            OMMesh::Point possibleNeighborPoint = possibleNeighbor.point;

            if (((currentVertex.neighbor2Used == false) && (currentVertex.neighbor1Used != false) && (possibleNeighborPoint == currentVertexNeighbor1))
                || ((currentVertex.neighbor1Used == false) && (currentVertex.neighbor2Used != false) && (currentVertexNeighbor2 == possibleNeighborPoint)) ) {

                FloorVertex* currentFloorVertexNeighbor = new FloorVertex(possibleNeighborPoint[0], possibleNeighborPoint[1]);
                incrementFloorPlanSize();

                previousFloorVertexNeighbor->setNeighbor2(currentFloorVertexNeighbor);//addNeighbor
                currentFloorVertexNeighbor->setNeighbor1(previousFloorVertexNeighbor);//addNeighbor

                if (possibleNeighbor.neighbor1 == currentPoint) {
                    possibleNeighbor.neighbor1Used = false;
                } else {
                    possibleNeighbor.neighbor2Used = false;
                }

                previousFloorVertexNeighbor = currentFloorVertexNeighbor;
                found = true;
            }
        }
    }

    // define the neighboring relationship between the last and first vertex
    incrementFloorPlanSize();
    previousFloorVertexNeighbor->setNeighbor2(firstPoint);//addNeighbor
    firstPoint->setNeighbor1(previousFloorVertexNeighbor);//addNeighbor

    // now create profiles

    // we have the floorplan, so now we must compute the profiles

    // mass center -> profile oriented toward the mass center
    float cx(0.0f);
    float cy(0.0f);

    Vertex* vMC = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i){
        cx += vMC->getX();
        cy += vMC->getY();
        vMC = vMC->getNeighbor2();
    }

    cx = cx / floorPlanSize;
    cy = cy / floorPlanSize;

    int totalNumberPlan = plan.size();
    int numberFloorPlanVertex = floorPlanSize;
    Profile* p;

    // will compute a profile for each vertex in the floor plan
    FloorVertex* v = floorPlan;
    for(int i(0); i < numberFloorPlanVertex; ++i){

        float phi1;
        float r1;

        // get the angle defining the 2D position on the floor vertex
        inversePolar(v->getX(), v->getY(), r1, phi1);
        p = new Profile(true);
        p->addProfileVertex(distance(v->getX(), v->getY(), cx, cy), floorPlanIndex * dz);

        for (int j = floorPlanIndex + 1; j < totalNumberPlan; ++j ) {
            std::vector<OMMesh::VertexIter>* planJ = plan[j];
            int numVertices = planJ->size();

            // for each vertex of the current plan, we want to find the one that has the most similar
            // angle with the current floor plan vertex (because if it is the same angle, it is on the same profile)
            for (int k(0); k < numVertices; ++k) {
                OMMesh::VertexIter v_it = (*planJ)[k];
                OMMesh::Point point = inputMesh->point(v_it);

                float r2;
                float phi2;
                // get the angle defining the 2D position on the current plan
                inversePolar(point[0], point[1], r2, phi2);

                if (abs(phi2 - phi1) < 0.001f || phi2 == 0.0f) {// je ne sait pas si tu veut mettre +- 0.001 comme d'abitude
                   p->addProfileVertex(distance(point[0], point[1], cx, cy), point[2] - baseZ);
                    break;
                }
            }
        }
        v->setProfile(p);
        v = (FloorVertex*)v->getNeighbor2();
    }
    setCurrentProfile(p);


    for(int i(0); i < totalNumPlan; ++i){
        std::vector<OMMesh::VertexIter>* planTmp = plan[i];
        delete planTmp;
    }


    emit newFloorPlan();

    delete inputMesh;
    inputMesh = 0;
}

float Mesh::distance(float x1, float y1, float x2, float y2)
{
    float diffX = x1 - x2;
    float diffY = y1 - y2;
    // -1.0f car: quand on calcul avec le profil pour generer 3D, en 0 s est droit et 1 s est pencher (fermer)
    // donc si distance 1.0f alors le cone est ouvert donc -1.0f
    // SA revient a dire que w est la distance entre le bord et le point x, y
    return 1.0f - sqrt(diffX * diffX + diffY * diffY);
}

void Mesh::inversePolar(float x, float y, float& r, float& phi)
{
    //http://en.wikipedia.org/wiki/Polar_coordinate_system
    r = sqrt(x*x + y*y);
    phi = atan2f(y, x);

}

FloorVertex* Mesh::getFloorPlan()
{
    return floorPlan;
}
