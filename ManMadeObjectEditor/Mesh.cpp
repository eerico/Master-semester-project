#include "Mesh.h"

Mesh::Mesh(): inputMesh(0), floorPlan(0), updateOnMesh(false), longUpdateOnMesh(false), floorPlanSize(0)
{
    points = new std::vector<qglviewer::Vec*>;


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

    delete points;
}

float Mesh::distance(float x1, float y1, float x2, float y2) {
    float diffX = x1 - x2;
    float diffY = y1 - y2;
    return std::sqrt(diffX * diffX + diffY * diffY);
}

void Mesh::setFloorPlan(FloorVertex* vertex) {
   floorPlan = vertex;
}

const std::vector<qglviewer::Vec *>& Mesh::getTriangles()
{
    // va devoir appliquer algorithme que etant donnée les floorVertex/edges et les profile, reconstruire les triangles
    return triangles;
}

std::vector<qglviewer::Vec*>* Mesh::getPoints(bool moreSample, float ds)
{

    if (!updateOnMesh && !longUpdateOnMesh) {
        return points;
    }

    unsigned int size = points->size();
    for( unsigned int i(0); i < size; ++i)
    {
        qglviewer::Vec* tmp = (*points)[i];
        delete tmp;
    }

    points->clear();

    if (floorPlanSize == 0) {
        return points;
    }

    if (moreSample) {
        getPointWithAdditionnalSampledPoint(points, ds);
    } else {
        getPointBasic(points);
    }

    updateOnMesh = false;

    return points;
}

void Mesh::computeNormals() {
    // if the normal are not defined we will defined them
    // their directions will be from the center to the vertex
    std::cout << "no normals" << std::endl;

    Vertex* tmp = floorPlan;
    float centerX(0.0f);
    float centerY(0.0f);
    for(unsigned int i(0); i < floorPlanSize; ++i) {
        centerX += tmp->getX();
        centerY += tmp->getY();
        tmp = tmp->getNeighbor2();
    }

    centerX = centerX / (float)(floorPlanSize);
    centerY = centerY / (float)(floorPlanSize);

    // assign the new computed normal to each floorplan vertices
    FloorVertex* floorVertexTmp = floorPlan;
    for(unsigned int i(0); i < floorPlanSize; ++i) {
        float nx = floorVertexTmp->getX() - centerX;
        float ny = floorVertexTmp->getY() - centerY;
        // the setNormal method will normalize our normal
        floorVertexTmp->setNormal(nx, ny);

        floorVertexTmp = (FloorVertex*)floorVertexTmp->getNeighbor2();
    }
}

void Mesh::getPointBasic(std::vector<qglviewer::Vec*>* points)
{
    FloorVertex* floorVertexTmp = floorPlan;

    float nx(0.0f);
    float ny(0.0f);

    floorVertexTmp->getNormal(nx, ny);
    if ((nx == 0.0f) && (ny == 0.0f)) {
        computeNormals();
    }

    for(unsigned int i(0); i < floorPlanSize; ++i) {

        Profile* profile = floorVertexTmp->getProfile();
        if (profile != 0) {
           Vertex* pVertex = profile->getProfileVertex();
            while(pVertex != 0)
            {
                float w = pVertex->getX();
                float z = pVertex->getY();

                //float x = floorVertexTmp->getX()* (1.0f - w) + centerX * w;
                //float y = floorVertexTmp->getY()* (1.0f - w) + centerY * w;

                floorVertexTmp->getNormal(nx, ny);
                float x = floorVertexTmp->getX() + (-nx) * w;
                float y = floorVertexTmp->getY() + (-ny) * w;

                points->push_back(new qglviewer::Vec(x, y, z));
                pVertex = pVertex->getNeighbor2();
            }
        }
        floorVertexTmp = (FloorVertex*)floorVertexTmp->getNeighbor2();
    }
}

void Mesh::getPointWithAdditionnalSampledPoint(std::vector<qglviewer::Vec*>* points, float ds)
{
    // very basic. Let say we have two vertices on the floor plan v1 and v2,
    // we interpolate new vertices between these two and take the profile of v1
    // and "associate" this profile to the new vertices. This is the idea
    // used to add new sample in the floor plan
    // To add new sample for the profile, we take vertices of the profile
    // two by two and interpolate
    // This method is only used to show more points on the 3D rendering

    FloorVertex* floorVertexTmp = floorPlan;

    float nx(0.0f);
    float ny(0.0f);

    floorVertexTmp->getNormal(nx, ny);
    if ((nx == 0.0f) && (ny == 0.0f)) {
        computeNormals();
    }

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
           float nx(0.0f);
           float ny(0.0f);
           floorVertexTmp->getNormal(nx, ny);

           float nextNx(0.0f);
           float nextNy(0.0f);
           nextFloorVertexTmp->getNormal(nextNx, nextNy);

           float distanceCurrentToNext = distance(currentX, currentY, nextX, nextY);

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

                do {
                    sampledW = currentW + dirW * t;
                    newZ = currentZ + dirZ * t;

                    float tt = 0.0f;
                    float sampledX(0.0f);
                    float sampledY(0.0f);
                    float dirX = nextX - currentX;
                    float dirY = nextY - currentY;
                    norm = std::sqrt(dirX * dirX + dirY * dirY);
                    dirX = dirX / norm;
                    dirY = dirY / norm;

                    float distanceSampledToNext;
                    do {
                        sampledX = currentX + dirX * tt;
                        sampledY = currentY + dirY * tt;

                        distanceSampledToNext = distance(sampledX, sampledY, nextX, nextY);


                        //float newX = sampledX * (1.0f - sampledW) + centerX * sampledW;
                        //float newY = sampledY * (1.0f - sampledW) + centerY * sampledW;
                        //normal interpolation
                        float normalInterpolationFactor = distanceSampledToNext / distanceCurrentToNext;
                        float currentNx = nx * normalInterpolationFactor + (1.0f - normalInterpolationFactor) * nextNx;
                        float currentNy = ny * normalInterpolationFactor + (1.0f - normalInterpolationFactor) * nextNy;

                        float newX = sampledX + (-currentNx) * sampledW;
                        float newY = sampledY + (-currentNy) * sampledW;

                        points->push_back(new qglviewer::Vec(newX, newY, newZ));

                        tt += ds;
                    // if the distance beween the samplend X,Y and next X,Y is small, stop
                    } while(distanceSampledToNext > ds);

                    t += ds;
                // if the distance beween the samplend W,Z and next W,Z is small, stop
                } while (distance(sampledW, newZ, nextW, nextZ) > ds);

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

    // we will delete the previous profiles after having loaded the new profiles
    ProfileDestructorManager::swap();

    /*inputMesh->request_face_normals();
    inputMesh->request_vertex_normals();

    OpenMesh::IO::Options ropt;
    ropt += OpenMesh::IO::Options::VertexNormal;
    if (OpenMesh::IO::read_mesh(*inputMesh, fileName.toLocal8Bit().constData()),
            ropt)
    {
        inputMesh->update_face_normals();
        inputMesh->update_vertex_normals();
    }*/

    //http://openmesh.org/Documentation/OpenMesh-Doc-Latest/tutorial_05.html
    inputMesh->request_vertex_normals();
    OpenMesh::IO::Options option;
    if (OpenMesh::IO::read_mesh(*inputMesh, fileName.toLocal8Bit().constData(), option)){

		// compute the vertex normals if the mesh doesnt provide them
		if (!option.check(OpenMesh::IO::Options::VertexNormal )) {
			inputMesh->request_face_normals();
			inputMesh->update_normals();
			inputMesh->release_face_normals();
		}
	} else {
		std::cerr << "Error while reading the mesh" << std::endl;
		delete inputMesh;
		inputMesh = 0;
		
		return;
    }

    FloorPlanAndProfileExtractor extractor;

    extractor.extract(inputMesh, floorPlan, currentProfile, floorPlanSize);

    emit newFloorPlan();

    delete inputMesh;
    inputMesh = 0;
}

FloorVertex* Mesh::getFloorPlan()
{
    return floorPlan;
}

void Mesh::setUpdateOnMesh()
{
    updateOnMesh = true;
}

void Mesh::setLongUpdateOnMesh(bool b) {
    longUpdateOnMesh = b;
}


