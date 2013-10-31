#include "MeshManager.h"

MeshManager::MeshManager(): inputMesh(0), floorPlan(0), updateOnMesh(false), longUpdateOnMesh(false), floorPlanSize(0)
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

MeshManager::~MeshManager()
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

    for(unsigned int i(0); i < plans.size(); ++i) {
        std::vector< Vertex* > level = plans[i];

        for(unsigned int j(0); j < level.size(); ++j) {
            Vertex* vertex = level[j];
            QGraphicsEllipseItem* ellipse = vertex->getEllipse();
            QGraphicsLineItem* lineItem = 0;

            if (vertex->getEdge2() != 0) {
                lineItem = vertex->getEdge2()->getLineItem();
            }
            if(ellipse != 0) {
                delete ellipse;
            }
            if(lineItem != 0) {
                delete lineItem;
            }

            delete vertex;
        }
        level.clear();
    }
    plans.clear();

    delete points;
}

void MeshManager::setFloorPlan(Vertex* vertex) {
   floorPlan = vertex;
}

const std::vector<qglviewer::Vec *>& MeshManager::getTriangles()
{
    if (!updateOnMesh && !longUpdateOnMesh) {
        return triangles;
    }



    return triangles;
}

std::vector<qglviewer::Vec*>* MeshManager::getPoints()
{

    if (!updateOnMesh && !longUpdateOnMesh) {
        return points;
    }

    return points;
}


void MeshManager::update()
{
    // hum surement que va reconstruire la liste de triangle en fonction du nouvelle input de point/profile donnée
    qglviewer::Vec* tmp = triangles[0];
    tmp->x -= 0.0001f;
}

unsigned int MeshManager::getFloorPlanSize()
{
    return floorPlanSize;
}

void MeshManager::incrementFloorPlanSize()
{
    floorPlanSize++;
}

void MeshManager::decrementFloorPlanSize()
{
    floorPlanSize--;
}

Profile* MeshManager::getCurrentProfile()
{
    return currentProfile;
}

void MeshManager::setCurrentProfile(Profile* p)
{
    currentProfile = p;
}

void MeshManager::loadMesh(QString fileName)
{
    //delete old data
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

    for(unsigned int i(0); i < plans.size(); ++i) {
        std::vector< Vertex* > level = plans[i];

        for(unsigned int j(0); j < level.size(); ++j) {
            Vertex* vertex = level[j];
            QGraphicsEllipseItem* ellipse = vertex->getEllipse();
            QGraphicsLineItem* lineItem = 0;

            if (vertex->getEdge2() != 0) {
                lineItem = vertex->getEdge2()->getLineItem();
            }
            if(ellipse != 0) {
                delete ellipse;
            }
            if(lineItem != 0) {
                delete lineItem;
            }

            delete vertex;
        }
        level.clear();
    }
    plans.clear();

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
    extractor.extract(inputMesh, floorPlan, currentProfile, floorPlanSize, plans);

    emit newFloorPlan();

    delete inputMesh;
    inputMesh = 0;
}

Vertex* MeshManager::getFloorPlan()
{
    return floorPlan;
}

void MeshManager::setUpdateOnMesh()
{
    updateOnMesh = true;
}

void MeshManager::setLongUpdateOnMesh(bool b) {
    longUpdateOnMesh = b;
}

std::vector< std::vector< Vertex* > >& MeshManager::getPlans()
{
    return plans;
}

