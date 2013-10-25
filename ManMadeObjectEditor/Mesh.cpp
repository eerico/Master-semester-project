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
            if (currentVertex->getEdge2() != 0) {
                delete currentVertex->getEdge2();
            }
            // (Qt documentation):
            // "Removes and deletes all items from the scene object before destroying the scene object.
            // The scene object is removed from the application's global scene list, and it is removed
            // from all associated views."
            /*if (currentVertex->getEllipse() != 0){
                delete currentVertex->getEllipse();
            }*/
            delete currentVertex;
            currentVertex = 0;
        }
        currentVertex = next;
    }

    delete points;
}

void Mesh::setFloorPlan(Vertex* vertex) {
   floorPlan = vertex;
}

const std::vector<qglviewer::Vec *>& Mesh::getTriangles()
{
    // va devoir appliquer algorithme que etant donnée les floorVertex/edges et les profile, reconstruire les triangles
    return triangles;
}

std::vector<qglviewer::Vec*>* Mesh::getPoints()
{

    if (!updateOnMesh && !longUpdateOnMesh) {
        return points;
    }


    return points;
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
        delete currentVertex->getEdge2()->getLineItem();
        delete currentVertex->getEdge2();
        delete currentVertex->getEllipse();
        delete currentVertex;
        currentVertex = next;
    }
    floorPlanSize = 0;
    currentProfile = 0;

    // we will delete the previous profiles after having loaded the new profiles
    ProfileDestructorManager::swap();

    //load the mesh
    //http://openmesh.org/Documentation/OpenMesh-Doc-Latest/tutorial_05.html
    inputMesh->request_vertex_normals();
    inputMesh->request_face_normals();
    OpenMesh::IO::Options option;
    if (OpenMesh::IO::read_mesh(*inputMesh, fileName.toLocal8Bit().constData(), option)){

		// compute the vertex normals if the mesh doesnt provide them
		if (!option.check(OpenMesh::IO::Options::VertexNormal )) {
			inputMesh->update_normals();
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

Vertex* Mesh::getFloorPlan()
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


